#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include<locale.h> 

// Функция сравнения для qsort
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
