#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

int compare_double(const void* a, const void* b) {
    double diff = *(double*)a - *(double*)b;
    if (fabs(diff) < 1e-12) return 0;
    return (diff > 0) ? 1 : -1;
}

void calculate_statistics(double* arithmetic_mean,
    double* geometric_mean,
    double* harmonic_mean,
    double* median,
    double epsilon,
    size_t count, ...) {
    
    if (count == 0 || arithmetic_mean == NULL || geometric_mean == NULL ||
        harmonic_mean == NULL || median == NULL) {
        return;
    }

    va_list args;
    va_start(args, count);

    double* numbers = malloc(count * sizeof(double));
    if (numbers == NULL) {
        va_end(args);
        return;
    }

    double sum = 0.0;
    double product = 1.0;
    double harmonic_sum = 0.0;
    bool has_zero_or_negative_for_harmonic = false;

    for (size_t i = 0; i < count; i++) {
        numbers[i] = va_arg(args, double);
        sum += numbers[i];
        product *= numbers[i];

        if (fabs(numbers[i]) < epsilon) {
            has_zero_or_negative_for_harmonic = true;
        } else {
            harmonic_sum += 1.0 / numbers[i];
        }
    }
    va_end(args);

    *arithmetic_mean = sum / count;

    if (product >= 0) {
        *geometric_mean = pow(product, 1.0 / count);
    }
    else {
        *geometric_mean = -pow(-product, 1.0 / count);
    }

    if (!has_zero_or_negative_for_harmonic && fabs(harmonic_sum) > epsilon) {
        *harmonic_mean = count / harmonic_sum;
    }
    else {
        *harmonic_mean = NAN;
    }

    qsort(numbers, count, sizeof(double), compare_double);
    if (count % 2 == 0) {
        *median = (numbers[count / 2 - 1] + numbers[count / 2]) / 2.0;
    }
    else {
        *median = numbers[count / 2];
    }

    free(numbers);
}

void print_double(double value) {
    if (isnan(value)) {
        printf("не определено");
    } else {
        if (fabs(value) < 0.0001 || fabs(value) > 1000000) {
            printf("%.6e", value);
        } else {
            printf("%.10f", value);
        }
    }
}

void run_test(const char* test_name, double epsilon, size_t count, ...) {
    printf("\n%s\n", test_name);
    printf("Числа: ");
    
    va_list args;
    va_start(args, count);
    for (size_t i = 0; i < count; i++) {
        double num = va_arg(args, double);
        printf("%.2f ", num);
        if (i < count - 1) printf(", ");
    }
    va_end(args);
    printf("\n");
    
    va_start(args, count);
    double arithmetic, geometric, harmonic, median;
    calculate_statistics(&arithmetic, &geometric, &harmonic, &median, 
                        epsilon, count, 
                        count == 5 ? 1.0 : 0, 
                        count == 5 ? 2.0 : 0, 
                        count == 5 ? 3.0 : 0, 
                        count == 5 ? 4.0 : 0, 
                        count == 5 ? 5.0 : 0);
    va_end(args);
    
    printf("Среднее арифметическое: ");
    print_double(arithmetic);
    printf("\n");
    
    printf("Среднее геометрическое: ");
    print_double(geometric);
    printf("\n");
    
    printf("Среднее гармоническое:  ");
    print_double(harmonic);
    printf("\n");
    
    printf("Медиана:                ");
    print_double(median);
    printf("\n");
}

void interactive_mode() {
    printf("\n=== ИНТЕРАКТИВНЫЙ РЕЖИМ ===\n");
    
    int count;
    printf("Введите количество чисел: ");
    scanf("%d", &count);
    
    if (count <= 0) {
        printf("Ошибка: количество должно быть положительным числом!\n");
        return;
    }
    
    double* numbers = malloc(count * sizeof(double));
    if (numbers == NULL) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    
    printf("Введите %d чисел (через пробел или каждое с новой строки):\n", count);
    for (int i = 0; i < count; i++) {
        scanf("%lf", &numbers[i]);
    }
    
    double arithmetic, geometric, harmonic, median;
    double epsilon = 1e-12;
    
    printf("\nВведенные числа: ");
    for (int i = 0; i < count; i++) {
        printf("%.2f ", numbers[i]);
    }
    printf("\n");
    
    double sum = 0.0, product = 1.0, harmonic_sum = 0.0;
    bool has_zero = false;
    
    for (int i = 0; i < count; i++) {
        sum += numbers[i];
        product *= numbers[i];
        if (fabs(numbers[i]) < epsilon) {
            has_zero = true;
        } else {
            harmonic_sum += 1.0 / numbers[i];
        }
    }
    
    arithmetic = sum / count;
    
    if (product >= 0) {
        geometric = pow(product, 1.0 / count);
    } else {
        geometric = -pow(-product, 1.0 / count);
    }
    
    if (!has_zero && fabs(harmonic_sum) > epsilon) {
        harmonic = count / harmonic_sum;
    } else {
        harmonic = NAN;
    }
    
    qsort(numbers, count, sizeof(double), compare_double);
    if (count % 2 == 0) {
        median = (numbers[count / 2 - 1] + numbers[count / 2]) / 2.0;
    } else {
        median = numbers[count / 2];
    }
    
    printf("Среднее арифметическое: ");
    print_double(arithmetic);
    printf("\n");
    
    printf("Среднее геометрическое: ");
    print_double(geometric);
    printf("\n");
    
    printf("Среднее гармоническое:  ");
    print_double(harmonic);
    printf("\n");
    
    printf("Медиана:                ");
    print_double(median);
    printf("\n");
    
    free(numbers);
}

void main() {
    setlocale(LC_ALL, "Rus");
    
    printf("=== СТАТИСТИЧЕСКИЕ ВЫЧИСЛЕНИЯ ===\n");
    
    int choice;
    do {
        printf("\nМеню:\n");
        printf("1. Запустить стандартные тесты\n");
        printf("2. Интерактивный режим\n");
        printf("3. Выход\n");
        printf("Выберите опцию: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1: {
                double epsilon = 1e-12;
                run_test("Тест 1: 1.0, 2.0, 3.0, 4.0, 5.0", epsilon, 5, 1.0, 2.0, 3.0, 4.0, 5.0);
                run_test("Тест 2: 10.0, 20.0, 30.0", epsilon, 3, 10.0, 20.0, 30.0);
                run_test("Тест 3: 1.0, 1.0, 1.0, 1.0, 1.0", epsilon, 5, 1.0, 1.0, 1.0, 1.0, 1.0);
                run_test("Тест 4: 0.1, 0.2, 0.3, 0.4, 0.5", epsilon, 5, 0.1, 0.2, 0.3, 0.4, 0.5);
                run_test("Тест 5: -5.0, -4.0, -3.0, -2.0, -1.0", epsilon, 5, -5.0, -4.0, -3.0, -2.0, -1.0);
                break;
            }
            case 2:
                interactive_mode();
                break;
            case 3:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (choice != 3);
);
    double* numbers = malloc(count * sizeof(double));
    if (numbers == NULL) {
        va_end(args);
        return;
    }

    // Чтение чисел и вычисление сумм
    double sum = 0.0;
    double product = 1.0;
    double harmonic_sum = 0.0;

    for (size_t i = 0; i < count; i++) {
        numbers[i] = va_arg(args, double);
        sum += numbers[i];
        product *= numbers[i];

        if (fabs(numbers[i]) > epsilon) {
            harmonic_sum += 1.0 / numbers[i];
        }
    }
    va_end(args);

    // Среднее арифметическое
    *arithmetic_mean = sum / count;

    // Среднее геометрическое
    if (product >= 0) {
        *geometric_mean = pow(product, 1.0 / count);
    }
    else {
        // Обработка отрицательных произведений
        *geometric_mean = -pow(-product, 1.0 / count);
    }

    // Среднее гармоническое
    if (fabs(harmonic_sum) > epsilon && count > 0) {
        *harmonic_mean = count / harmonic_sum;
    }
    else {
        *harmonic_mean = 0.0;
    }

    // Медиана
    qsort(numbers, count, sizeof(double), compare_double);
    if (count % 2 == 0) {
        *median = (numbers[count / 2 - 1] + numbers[count / 2]) / 2.0;
    }
    else {
        *median = numbers[count / 2];
    }

    free(numbers);
}

// Демонстрация работы
void main() {
    setlocale(LC_ALL, "Rus");
    printf("=== Задание 1: Статистические вычисления ===\n");

    double arithmetic, geometric, harmonic, median;
    double epsilon = 1e-12;

    calculate_statistics(&arithmetic, &geometric, &harmonic, &median,
        epsilon, 5, 1.0, 2.0, 3.0, 4.0, 5.0);

    printf("Числа: 1.0, 2.0, 3.0, 4.0, 5.0\n");
    printf("Среднее арифметическое: %.6f\n", arithmetic);
    printf("Среднее геометрическое: %.6f\n", geometric);
    printf("Среднее гармоническое: %.6f\n", harmonic);
    printf("Медиана: %.6f\n", median);

    printf("\n");
}
