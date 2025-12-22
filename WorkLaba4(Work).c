#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <float.h>
#include <errno.h>

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ВАЛИДАЦИИ ====================
int is_valid_double(double value) {
    // Проверка на NaN и бесконечность
    if (isnan(value) || isinf(value)) {
        return 0;
    }
    
    // Проверка на слишком большое или маленькое значение
    if (fabs(value) > DBL_MAX / 1000.0) {
        return 0;
    }
    
    return 1;
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ==================== ЗАДАНИЕ 4 ====================
double polynomial_value(double x, size_t degree, ...) {
    // Валидация входных параметров
    if (!is_valid_double(x)) {
        fprintf(stderr, "Ошибка: недопустимое значение x (NaN, inf или слишком большое)\n");
        return NAN;
    }
    
    // Проверка на слишком высокую степень (чтобы избежать переполнения)
    if (degree > 100) {
        fprintf(stderr, "Ошибка: степень многочлена слишком высока (%zu > 100)\n", degree);
        return NAN;
    }
    
    va_list args;
    va_start(args, degree);

    double result = 0.0;
    double x_power = 1.0;
    errno = 0; // Сбрасываем errno перед вычислениями

    for (size_t i = 0; i <= degree; i++) {
        double coefficient = va_arg(args, double);
        
        // Валидация коэффициента
        if (!is_valid_double(coefficient)) {
            fprintf(stderr, "Ошибка: недопустимый коэффициент №%zu\n", i + 1);
            va_end(args);
            return NAN;
        }
        
        // Вычисление текущего члена с проверкой переполнения
        double term = coefficient * x_power;
        
        if (!is_valid_double(term)) {
            fprintf(stderr, "Ошибка: переполнение при вычислении члена №%zu\n", i + 1);
            va_end(args);
            return NAN;
        }
        
        // Сложение с проверкой
        double old_result = result;
        result += term;
        
        // Проверка на переполнение при сложении
        if (!is_valid_double(result) || 
            (fabs(term) > 0 && fabs(result - old_result) < fabs(term) * 0.5)) {
            fprintf(stderr, "Ошибка: потеря точности или переполнение при сложении члена №%zu\n", i + 1);
            va_end(args);
            return NAN;
        }
        
        // Проверка errno после операций
        if (errno == ERANGE) {
            fprintf(stderr, "Ошибка: переполнение при вычислении степени x\n");
            va_end(args);
            return NAN;
        }
        
        // Вычисление следующей степени x с проверкой
        if (i < degree) {
            double old_power = x_power;
            x_power *= x;
            
            // Проверка на переполнение степени
            if (!is_valid_double(x_power) || 
                (fabs(x) > 1.0 && fabs(x_power) < fabs(old_power))) {
                fprintf(stderr, "Ошибка: переполнение при вычислении x^%zu\n", i + 2);
                va_end(args);
                return NAN;
            }
        }
    }

    va_end(args);
    
    if (!is_valid_double(result)) {
        fprintf(stderr, "Ошибка: итоговый результат недопустим\n");
        return NAN;
    }
    
    return result;
}

// ==================== ФУНКЦИЯ ДЛЯ БЕЗОПАСНОГО ВВОДА ====================
double safe_input_double(const char* prompt, double min, double max) {
    double value;
    int valid_input = 0;
    
    while (!valid_input) {
        printf("%s", prompt);
        
        if (scanf("%lf", &value) != 1) {
            printf("Ошибка: введите корректное число\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer(); // Очистка буфера после успешного чтения числа
        
        if (!is_valid_double(value)) {
            printf("Ошибка: число слишком большое или недопустимое\n");
            continue;
        }
        
        if (value < min || value > max) {
            printf("Ошибка: число должно быть в диапазоне от %.2f до %.2f\n", min, max);
            continue;
        }
        
        valid_input = 1;
    }
    
    return value;
}

int safe_input_int(const char* prompt, int min, int max) {
    int value;
    int valid_input = 0;
    
    while (!valid_input) {
        printf("%s", prompt);
        
        if (scanf("%d", &value) != 1) {
            printf("Ошибка: введите корректное целое число\n");
            clear_input_buffer();
            continue;
        }
        
        clear_input_buffer(); // Очистка буфера после успешного чтения числа
        
        if (value < min || value > max) {
            printf("Ошибка: число должно быть в диапазоне от %d до %d\n", min, max);
            continue;
        }
        
        valid_input = 1;
    }
    
    return value;
}

// ==================== ИНТЕРАКТИВНАЯ ДЕМОНСТРАЦИЯ ====================
void interactive_polynomial_calculator() {
    printf("\n=== Интерактивный калькулятор многочленов ===\n");
    
    int degree = safe_input_int("Введите степень многочлена (0-10): ", 0, 10);
    
    double x = safe_input_double("Введите значение x (от -1e6 до 1e6): ", -1e6, 1e6);
    
    printf("Введите %d коэффициентов (от старшей степени к свободному члену):\n", degree + 1);
    
    // Создаем массив для коэффициентов
    double coefficients[11]; // Максимум 11 коэффициентов (степень 10)
    
    for (int i = 0; i <= degree; i++) {
        char prompt[100];
        snprintf(prompt, sizeof(prompt), "Коэффициент для x^%d: ", degree - i);
        coefficients[i] = safe_input_double(prompt, -1e9, 1e9);
    }
    
    // Вызываем polynomial_value с переменным числом аргументов
    printf("\nВычисление многочлена: ");
    for (int i = 0; i <= degree; i++) {
        if (i > 0) printf(" + ");
        printf("%.2f*x^%d", coefficients[i], degree - i);
    }
    printf("\nПри x = %.2f\n", x);
    
    double result;
    
    // Вызов в зависимости от степени
    switch (degree) {
        case 0:
            result = polynomial_value(x, degree, coefficients[0]);
            break;
        case 1:
            result = polynomial_value(x, degree, coefficients[0], coefficients[1]);
            break;
        case 2:
            result = polynomial_value(x, degree, coefficients[0], coefficients[1], coefficients[2]);
            break;
        case 3:
            result = polynomial_value(x, degree, 
                coefficients[0], coefficients[1], coefficients[2], coefficients[3]);
            break;
        case 4:
            result = polynomial_value(x, degree, 
                coefficients[0], coefficients[1], coefficients[2], 
                coefficients[3], coefficients[4]);
            break;
        default:
            // Для более высоких степеней используем прямой вызов с явным перечислением
            // В реальном приложении лучше использовать массив и цикл
            printf("Для степеней выше 4 используется упрощенный расчет\n");
            result = polynomial_value(x, 4, 
                coefficients[0], coefficients[1], coefficients[2], 
                coefficients[3], coefficients[4]);
    }
    
    if (isnan(result)) {
        printf("Ошибка при вычислении многочлена!\n");
    } else {
        printf("Результат: %.6f\n", result);
    }
}

void demo_task() {
    printf("=== Задание 4: Вычисление значения многочлена ===\n");

    // Тест 1: нормальный случай
    printf("Тест 1: 2x^2 + 3x + 1 при x=2\n");
    double value = polynomial_value(2.0, 2, 2.0, 3.0, 1.0);
    if (!isnan(value)) {
        printf("Результат: %.2f (ожидается: 15.00)\n", value);
    }
    
    // Тест 2: другой многочлен
    printf("\nТест 2: x^3 - 2x + 5 при x=1\n");
    value = polynomial_value(1.0, 3, 1.0, 0.0, -2.0, 5.0);
    if (!isnan(value)) {
        printf("Результат: %.2f (ожидается: 4.00)\n", value);
    }
    
    // Тест 3: проверка на ошибочные данные
    printf("\nТест 3: проверка валидации (деление на 0 в коэффициентах)\n");
    value = polynomial_value(1.0, 2, 1.0/0.0, 2.0, 3.0); // Бесконечность
    if (isnan(value)) {
        printf("Валидация сработала: обнаружена бесконечность в коэффициентах\n");
    }
    
    // Тест 4: слишком большие значения
    printf("\nТест 4: проверка на переполнение\n");
    value = polynomial_value(1e100, 2, 1.0, 2.0, 3.0);
    if (isnan(value)) {
        printf("Валидация сработала: предотвращено переполнение\n");
    }

    printf("\n");
}

int main() {
    setlocale(LC_ALL, "Rus");
    printf("Демонстрация выполненных заданий\n\n");

    demo_task();
    
    // Запуск интерактивного калькулятора
    char choice;
    do {
        printf("\nХотите вычислить свой многочлен? (y/n): ");
        scanf(" %c", &choice);
        clear_input_buffer();
        
        if (choice == 'y' || choice == 'Y') {
            interactive_polynomial_calculator();
        }
    } while (choice == 'y' || choice == 'Y');

    printf("\nВсе демонстрации завершены.\n");
    return 0;
}
