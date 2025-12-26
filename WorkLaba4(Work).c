#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <float.h>
#include <string.h>
#include <errno.h>

// ==================== КОНСТАНТЫ И НАСТРОЙКИ ====================
#define MAX_POLYNOMIAL_DEGREE 100     // Максимальная допустимая степень
#define MAX_ITERATIONS 1000           // Максимальное количество итераций для сходимости
#define EPSILON 1e-12                 // Точность сравнения чисел с плавающей точкой

// ==================== СТРУКТУРЫ ДАННЫХ ====================
typedef struct {
    int is_valid;
    double value;
    char error_message[256];
} CalculationResult;

// ==================== ПРОТОТИПЫ ФУНКЦИЙ ====================
double polynomial_value(double x, size_t degree, ...);
CalculationResult safe_polynomial_value(double x, size_t degree, ...);
int validate_polynomial_parameters(double x, size_t degree, ...);
int is_finite_number(double value);
int is_reasonable_number(double value);
void print_polynomial(size_t degree, const double* coefficients);
void demo_polynomial_calculation(void);
void interactive_mode(void);
void run_tests(void);

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

/**
 * Проверяет, является ли число конечным (не NaN и не бесконечность)
 */
int is_finite_number(double value) {
    return !isnan(value) && !isinf(value);
}

/**
 * Проверяет, находится ли число в разумных пределах для вычислений
 */
int is_reasonable_number(double value) {
    return fabs(value) <= 1e100; // Ограничение на очень большие числа
}

/**
 * Валидация параметров многочлена
 */
int validate_polynomial_parameters(double x, size_t degree, ...) {
    // Проверка степени многочлена
    if (degree > MAX_POLYNOMIAL_DEGREE) {
        fprintf(stderr, "Ошибка: степень многочлена (%zu) превышает максимально допустимую (%d)\n",
            degree, MAX_POLYNOMIAL_DEGREE);
        return 0;
    }

    // Проверка значения x
    if (!is_finite_number(x)) {
        fprintf(stderr, "Ошибка: x = %f не является конечным числом\n", x);
        return 0;
    }

    if (!is_reasonable_number(x)) {
        fprintf(stderr, "Предупреждение: x = %e выходит за разумные пределы\n", x);
    }

    // Проверка коэффициентов
    va_list args;
    va_start(args, degree);

    int all_zero = 1;
    for (size_t i = 0; i <= degree; i++) {
        double coeff = va_arg(args, double);

        if (!is_finite_number(coeff)) {
            fprintf(stderr, "Ошибка: коэффициент a[%zu] = %f не является конечным числом\n",
                i, coeff);
            va_end(args);
            return 0;
        }

        if (!is_reasonable_number(coeff)) {
            fprintf(stderr, "Предупреждение: коэффициент a[%zu] = %e выходит за разумные пределы\n",
                i, coeff);
        }

        if (fabs(coeff) > EPSILON) {
            all_zero = 0;
        }
    }

    va_end(args);

    if (all_zero) {
        fprintf(stderr, "Предупреждение: все коэффициенты многочлена равны нулю\n");
    }

    return 1;
}

/**
 * Вычисление значения многочлена с использованием схемы Горнера
 */
double polynomial_value_horner(double x, size_t degree, const double* coefficients) {
    double result = coefficients[degree];

    for (int i = (int)degree - 1; i >= 0; i--) {
        result = result * x + coefficients[i];

        // Проверка на переполнение
        if (isinf(result)) {
            fprintf(stderr, "Переполнение при вычислении значения многочлена\n");
            return result; // Возвращаем бесконечность
        }

        if (isnan(result)) {
            fprintf(stderr, "Результат вычислений стал NaN\n");
            return NAN;
        }
    }

    return result;
}

// ==================== ОСНОВНАЯ ФУНКЦИЯ ====================

/**
 * Основная функция вычисления значения многочлена
 * Аргументы: x, degree, коэффициенты (от старшей степени к свободному члену)
 */
double polynomial_value(double x, size_t degree, ...) {
    // Проверка частных случаев
    if (degree == 0) {
        double a0;
        va_list args;
        va_start(args, degree);
        a0 = va_arg(args, double);
        va_end(args);
        return a0; // Многочлен нулевой степени - константа
    }

    if (fabs(x) < EPSILON) {
        double a0;
        va_list args;
        va_start(args, degree);

        // Пропускаем старшие коэффициенты
        for (size_t i = 0; i < degree; i++) {
            va_arg(args, double);
        }
        a0 = va_arg(args, double); // Свободный член

        va_end(args);
        return a0; // При x=0 значение равно свободному члену
    }

    // Получаем коэффициенты
    va_list args;
    va_start(args, degree);

    double* coefficients = (double*)malloc((degree + 1) * sizeof(double));
    if (!coefficients) {
        fprintf(stderr, "Ошибка выделения памяти для коэффициентов\n");
        va_end(args);
        return NAN;
    }

    // Сохраняем коэффициенты в правильном порядке
    // Вход: от старшей степени к свободному члену
    // Для схемы Горнера: от младшей к старшей
    for (size_t i = 0; i <= degree; i++) {
        coefficients[degree - i] = va_arg(args, double);
    }

    va_end(args);

    // Вычисляем значение
    double result = polynomial_value_horner(x, degree, coefficients);

    free(coefficients);
    return result;
}

/**
 * Безопасная версия с полной проверкой и возвратом структуры результата
 */
CalculationResult safe_polynomial_value(double x, size_t degree, ...) {
    CalculationResult result = { 0 };

    // Валидация параметров
    va_list args_validation;
    va_start(args_validation, degree);

    int is_valid = 1;
    if (degree > MAX_POLYNOMIAL_DEGREE) {
        snprintf(result.error_message, sizeof(result.error_message),
            "Степень многочлена (%zu) превышает максимальную (%d)",
            degree, MAX_POLYNOMIAL_DEGREE);
        is_valid = 0;
    }

    if (!is_finite_number(x)) {
        snprintf(result.error_message, sizeof(result.error_message),
            "x = %f не является конечным числом", x);
        is_valid = 0;
    }

    // Проверка коэффициентов
    for (size_t i = 0; i <= degree && is_valid; i++) {
        double coeff = va_arg(args_validation, double);
        if (!is_finite_number(coeff)) {
            snprintf(result.error_message, sizeof(result.error_message),
                "Коэффициент a[%zu] = %f не является конечным числом", i, coeff);
            is_valid = 0;
        }
    }

    va_end(args_validation);

    if (!is_valid) {
        result.is_valid = 0;
        result.value = NAN;
        return result;
    }

    // Вычисление значения
    va_list args_calc;
    va_start(args_calc, degree);

    double* coefficients = (double*)malloc((degree + 1) * sizeof(double));
    if (!coefficients) {
        snprintf(result.error_message, sizeof(result.error_message),
            "Ошибка выделения памяти");
        result.is_valid = 0;
        result.value = NAN;
        va_end(args_calc);
        return result;
    }

    // Сохраняем коэффициенты
    for (size_t i = 0; i <= degree; i++) {
        coefficients[degree - i] = va_arg(args_calc, double);
    }

    va_end(args_calc);

    // Вычисляем
    double calc_result = polynomial_value_horner(x, degree, coefficients);

    // Проверка результата
    if (!is_finite_number(calc_result)) {
        if (isinf(calc_result)) {
            snprintf(result.error_message, sizeof(result.error_message),
                "Результат вычисления - бесконечность");
        }
        else {
            snprintf(result.error_message, sizeof(result.error_message),
                "Результат вычисления - NaN");
        }
        result.is_valid = 0;
    }
    else {
        result.is_valid = 1;
        strcpy(result.error_message, "Успешно");
    }

    result.value = calc_result;
    free(coefficients);

    return result;
}

/**
 * Красивый вывод многочлена
 */
void print_polynomial(size_t degree, const double* coefficients) {
    int first_term = 1;

    for (int i = (int)degree; i >= 0; i--) {
        double coeff = coefficients[i];

        // Пропускаем нулевые коэффициенты
        if (fabs(coeff) < EPSILON) {
            continue;
        }

        // Вывод знака
        if (!first_term) {
            printf(" %c ", coeff > 0 ? '+' : '-');
        }
        else if (coeff < 0) {
            printf("-");
        }

        // Вывод коэффициента
        double abs_coeff = fabs(coeff);

        if (i == 0) {
            // Свободный член
            printf("%.4f", abs_coeff);
        }
        else if (fabs(abs_coeff - 1.0) < EPSILON) {
            // Коэффициент 1 не выводим
            if (i == 1) {
                printf("x");
            }
            else {
                printf("x^%d", i);
            }
        }
        else {
            // Обычный коэффициент
            if (i == 1) {
                printf("%.4fx", abs_coeff);
            }
            else {
                printf("%.4fx^%d", abs_coeff, i);
            }
        }

        first_term = 0;
    }

    if (first_term) {
        printf("0");
    }
}

// ==================== ДЕМОНСТРАЦИЯ И ТЕСТИРОВАНИЕ ====================

void demo_polynomial_calculation(void) {
    printf("\n=== ДЕМОНСТРАЦИЯ ВЫЧИСЛЕНИЯ ЗНАЧЕНИЯ МНОГОЧЛЕНА ===\n\n");

    // Пример 1: Простой квадратный многочлен
    printf("Пример 1: 2x² + 3x + 1\n");
    printf("Коэффициенты (от старшей степени): 2, 3, 1\n");
    printf("Вычисление при x = 2.0:\n");

    double result = polynomial_value(2.0, 2, 2.0, 3.0, 1.0);
    printf("P(2.0) = %.4f\n\n", result);

    // Пример 2: Кубический многочлен
    printf("Пример 2: x³ - 2x + 5\n");
    printf("Коэффициенты (от старшей степени): 1, 0, -2, 5\n");
    printf("Вычисление при x = 1.0:\n");

    result = polynomial_value(1.0, 3, 1.0, 0.0, -2.0, 5.0);
    printf("P(1.0) = %.4f\n\n", result);

    // Пример 3: Многочлен высокой степени
    printf("Пример 3: 3x⁴ - 4x³ + 2x² - x + 7\n");
    printf("Коэффициенты (от старшей степени): 3, -4, 2, -1, 7\n");
    printf("Вычисление при x = -1.5:\n");

    result = polynomial_value(-1.5, 4, 3.0, -4.0, 2.0, -1.0, 7.0);
    printf("P(-1.5) = %.4f\n\n", result);

    // Пример 4: Нулевая степень (константа)
    printf("Пример 4: Константа 3.14\n");
    printf("Коэффициенты: 3.14\n");
    printf("Вычисление при x = 100.0:\n");

    result = polynomial_value(100.0, 0, 3.14);
    printf("P(100.0) = %.4f\n\n", result);
}

void run_tests(void) {
    printf("\n=== ТЕСТИРОВАНИЕ ФУНКЦИИ ===\n\n");

    // Тест 1: Корректные данные
    printf("Тест 1: Корректные данные\n");
    CalculationResult res = safe_polynomial_value(2.0, 2, 1.0, 2.0, 1.0);
    if (res.is_valid) {
        printf("  Успешно: P(2) = %.4f\n", res.value);
        printf("  Ожидалось: 1*4 + 2*2 + 1 = 9.0\n");
    }
    else {
        printf("  Ошибка: %s\n", res.error_message);
    }

    // Тест 2: x = 0
    printf("\nТест 2: x = 0\n");
    res = safe_polynomial_value(0.0, 3, 2.0, 3.0, 4.0, 5.0);
    if (res.is_valid) {
        printf("  Успешно: P(0) = %.4f\n", res.value);
        printf("  Ожидалось: свободный член = 5.0\n");
    }
    else {
        printf("  Ошибка: %s\n", res.error_message);
    }

    // Тест 3: Очень большой x
    printf("\nТест 3: Очень большой x\n");
    res = safe_polynomial_value(1e10, 1, 1.0, 1.0);
    if (res.is_valid) {
        printf("  Успешно: P(1e10) = %e\n", res.value);
    }
    else {
        printf("  Ошибка: %s\n", res.error_message);
    }

    // Тест 4: Слишком высокая степень (исправлено - не используем va_start в неправильном контексте)
    printf("\nТест 4: Слишком высокая степень\n");
    // Создаем тест напрямую вызовом функции
    printf("  Попытка вычисления многочлена степени 101 (превышает лимит)...\n");

    // Создаем массив коэффициентов для степени 101
    double coeffs_high[102];
    for (int i = 0; i < 102; i++) coeffs_high[i] = 1.0;

    // Вызываем через безопасную функцию (степень 101)
    res = safe_polynomial_value(1.0, 101,
        coeffs_high[101], coeffs_high[100], coeffs_high[99], coeffs_high[98], coeffs_high[97],
        coeffs_high[96], coeffs_high[95], coeffs_high[94], coeffs_high[93], coeffs_high[92],
        coeffs_high[91], coeffs_high[90], coeffs_high[89], coeffs_high[88], coeffs_high[87],
        coeffs_high[86], coeffs_high[85], coeffs_high[84], coeffs_high[83], coeffs_high[82],
        coeffs_high[81], coeffs_high[80], coeffs_high[79], coeffs_high[78], coeffs_high[77],
        coeffs_high[76], coeffs_high[75], coeffs_high[74], coeffs_high[73], coeffs_high[72],
        coeffs_high[71], coeffs_high[70], coeffs_high[69], coeffs_high[68], coeffs_high[67],
        coeffs_high[66], coeffs_high[65], coeffs_high[64], coeffs_high[63], coeffs_high[62],
        coeffs_high[61], coeffs_high[60], coeffs_high[59], coeffs_high[58], coeffs_high[57],
        coeffs_high[56], coeffs_high[55], coeffs_high[54], coeffs_high[53], coeffs_high[52],
        coeffs_high[51], coeffs_high[50], coeffs_high[49], coeffs_high[48], coeffs_high[47],
        coeffs_high[46], coeffs_high[45], coeffs_high[44], coeffs_high[43], coeffs_high[42],
        coeffs_high[41], coeffs_high[40], coeffs_high[39], coeffs_high[38], coeffs_high[37],
        coeffs_high[36], coeffs_high[35], coeffs_high[34], coeffs_high[33], coeffs_high[32],
        coeffs_high[31], coeffs_high[30], coeffs_high[29], coeffs_high[28], coeffs_high[27],
        coeffs_high[26], coeffs_high[25], coeffs_high[24], coeffs_high[23], coeffs_high[22],
        coeffs_high[21], coeffs_high[20], coeffs_high[19], coeffs_high[18], coeffs_high[17],
        coeffs_high[16], coeffs_high[15], coeffs_high[14], coeffs_high[13], coeffs_high[12],
        coeffs_high[11], coeffs_high[10], coeffs_high[9], coeffs_high[8], coeffs_high[7],
        coeffs_high[6], coeffs_high[5], coeffs_high[4], coeffs_high[3], coeffs_high[2],
        coeffs_high[1], coeffs_high[0]);

    if (!res.is_valid) {
        printf("  Ожидаемая ошибка: %s\n", res.error_message);
    }
    else {
        printf("  Неожиданный успех (должна была быть ошибка)\n");
    }

    // Тест 5: NaN в коэффициентах
    printf("\nТест 5: NaN в коэффициентах\n");
    res = safe_polynomial_value(1.0, 2, 1.0, NAN, 1.0);
    if (!res.is_valid) {
        printf("  Ожидаемая ошибка: %s\n", res.error_message);
    }
    else {
        printf("  Неожиданный успех\n");
    }

    // Тест 6: Бесконечность в x
    printf("\nТест 6: Бесконечность в x\n");
    res = safe_polynomial_value(INFINITY, 2, 1.0, 0.0, 1.0);
    if (!res.is_valid) {
        printf("  Ожидаемая ошибка: %s\n", res.error_message);
    }
    else {
        printf("  Результат: %e\n", res.value);
    }

    // Тест 7: Отрицательная степень (невозможно, так как degree имеет тип size_t)
    printf("\nТест 7: Проверка на нулевые коэффициенты\n");
    res = safe_polynomial_value(5.0, 3, 0.0, 0.0, 0.0, 0.0);
    if (res.is_valid) {
        printf("  Успешно: P(5) = %.4f (должен быть 0)\n", res.value);
    }
    else {
        printf("  Ошибка: %s\n", res.error_message);
    }
}

void interactive_mode(void) {
    printf("\n=== ИНТЕРАКТИВНЫЙ РЕЖИМ ===\n\n");

    double x;
    size_t degree;
    char buffer[256];

    // Ввод значения x
    while (1) {
        printf("Введите значение x: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка чтения ввода\n");
            return;
        }

        char* endptr;
        x = strtod(buffer, &endptr);

        if (endptr == buffer || *endptr != '\n') {
            printf("Ошибка: введите корректное число\n");
        }
        else if (!is_finite_number(x)) {
            printf("Ошибка: x не является конечным числом\n");
        }
        else {
            break;
        }
    }

    // Ввод степени многочлена
    while (1) {
        printf("Введите степень многочлена (0-%d): ", MAX_POLYNOMIAL_DEGREE);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка чтения ввода\n");
            return;
        }

        char* endptr;
        long temp = strtol(buffer, &endptr, 10);

        if (endptr == buffer || *endptr != '\n') {
            printf("Ошибка: введите целое число\n");
        }
        else if (temp < 0 || temp > MAX_POLYNOMIAL_DEGREE) {
            printf("Ошибка: степень должна быть от 0 до %d\n", MAX_POLYNOMIAL_DEGREE);
        }
        else {
            degree = (size_t)temp;
            break;
        }
    }

    // Ввод коэффициентов
    printf("\nВведите коэффициенты (от старшей степени к свободному члену):\n");
    double* coefficients = (double*)malloc((degree + 1) * sizeof(double));
    if (!coefficients) {
        printf("Ошибка выделения памяти\n");
        return;
    }

    for (size_t i = 0; i <= degree; i++) {
        while (1) {
            printf("  Коэффициент при x^%zu: ", degree - i);
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                printf("Ошибка чтения ввода\n");
                free(coefficients);
                return;
            }

            char* endptr;
            coefficients[i] = strtod(buffer, &endptr);

            if (endptr == buffer || *endptr != '\n') {
                printf("Ошибка: введите число\n");
            }
            else if (!is_finite_number(coefficients[i])) {
                printf("Ошибка: коэффициент не является конечным числом\n");
            }
            else {
                break;
            }
        }
    }

    // Вывод многочлена
    printf("\nМногочлен: P(x) = ");
    print_polynomial(degree, coefficients);
    printf("\n");

    // Вычисление значения
    printf("\nВычисление значения при x = %.4f...\n", x);

    // Создаем правильный вызов функции в зависимости от степени
    CalculationResult res;

    if (degree == 0) {
        res = safe_polynomial_value(x, degree, coefficients[0]);
    }
    else if (degree == 1) {
        res = safe_polynomial_value(x, degree, coefficients[1], coefficients[0]);
    }
    else if (degree == 2) {
        res = safe_polynomial_value(x, degree, coefficients[2], coefficients[1], coefficients[0]);
    }
    else if (degree == 3) {
        res = safe_polynomial_value(x, degree, coefficients[3], coefficients[2], coefficients[1], coefficients[0]);
    }
    else {
        // Для высоких степеней используем ручной расчет через схему Горнера
        printf("  Примечание: используется прямое вычисление для высокой степени\n");
        double result = polynomial_value_horner(x, degree, coefficients);

        res.is_valid = is_finite_number(result);
        res.value = result;
        if (res.is_valid) {
            strcpy(res.error_message, "Успешно (прямое вычисление)");
        }
        else {
            snprintf(res.error_message, sizeof(res.error_message),
                "Ошибка при прямом вычислении");
        }
    }

    printf("Результат: ");

    if (res.is_valid) {
        printf("P(%.4f) = ", x);
        if (isinf(res.value)) {
            printf("бесконечность\n");
        }
        else if (isnan(res.value)) {
            printf("не число (NaN)\n");
        }
        else {
            // Вывод с разной точностью в зависимости от величины
            if (fabs(res.value) > 1e6 || (fabs(res.value) < 1e-6 && res.value != 0)) {
                printf("%e\n", res.value);
            }
            else {
                printf("%.10f\n", res.value);
            }
        }
    }
    else {
        printf("Ошибка: %s\n", res.error_message);
    }

    free(coefficients);
}

// ==================== ГЛАВНАЯ ФУНКЦИЯ ====================

int main(void) {
    setlocale(LC_ALL, "Rus");

    printf("========================================================================\n");
    printf("         ВЫЧИСЛЕНИЕ ЗНАЧЕНИЯ МНОГОЧЛЕНА С ПЕРЕМЕННЫМ ЧИСЛОМ АРГУМЕНТОВ\n");
    printf("========================================================================\n\n");

    int choice;
    char buffer[100];

    do {
        printf("\nМЕНЮ:\n");
        printf("1. Демонстрация работы функции\n");
        printf("2. Тестирование сценариев\n");
        printf("3. Интерактивный режим\n");
        printf("4. Выход\n");
        printf("\nВыберите действие (1-4): ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Ошибка чтения ввода\n");
            break;
        }

        choice = atoi(buffer);

        switch (choice) {
        case 1:
            demo_polynomial_calculation();
            break;

        case 2:
            run_tests();
            break;

        case 3:
            interactive_mode();
            break;

        case 4:
            printf("\nВыход из программы...\n");
            break;

        default:
            printf("Неверный выбор. Попробуйте снова.\n");
            break;
        }

    } while (choice != 4);

    printf("\nПрограмма завершена.\n");
    return 0;
}
