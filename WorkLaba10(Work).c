#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>
#include <float.h>
#include <errno.h>

#define EPS 1e-12
#define MAX_POLY_DEGREE 100

// Вспомогательные функции для валидации

// Проверка указателя
int is_valid_pointer(const void* ptr, const char* func_name) {
    // не забыл
    if (ptr == NULL) {
        fprintf(stderr, "Ошибка в %s: NULL указатель\n", func_name);
        return 0;
    }
    return 1;
}

// Проверка точности epsilon
int is_valid_epsilon(double epsilon, const char* func_name) {
    // не забыл
    if (epsilon <= 0) {
        fprintf(stderr, "Ошибка в %s: epsilon должен быть положительным\n", func_name);
        return 0;
    }
    
    if (epsilon < DBL_MIN * 10) {
        fprintf(stderr, "Ошибка в %s: epsilon слишком мал\n", func_name);
        return 0;
    }
    
    if (epsilon > 1.0) {
        fprintf(stderr, "Ошибка в %s: epsilon слишком велик\n", func_name);
        return 0;
    }
    
    return 1;
}

// Проверка степени многочлена
int is_valid_degree(size_t degree, const char* func_name) {
    // не забыл
    if (degree > MAX_POLY_DEGREE) {
        fprintf(stderr, "Ошибка в %s: степень многочлена слишком высока (%zu > %d)\n", 
                func_name, degree, MAX_POLY_DEGREE);
        return 0;
    }
    
    return 1;
}

// Проверка коэффициента многочлена
int is_valid_coefficient(double coeff, const char* func_name) {
    // не забыл
    if (!isfinite(coeff)) {
        fprintf(stderr, "Ошибка в %s: коэффициент не является конечным числом\n", func_name);
        return 0;
    }
    
    // Проверка на слишком большие значения
    if (fabs(coeff) > 1e100) {
        fprintf(stderr, "Ошибка в %s: коэффициент слишком велик\n", func_name);
        return 0;
    }
    
    return 1;
}

// Проверка точки a
int is_valid_point(double a, const char* func_name) {
    // не забыл
    if (!isfinite(a)) {
        fprintf(stderr, "Ошибка в %s: точка a не является конечным числом\n", func_name);
        return 0;
    }
    
    // Проверка на слишком большие значения
    if (fabs(a) > 1e100) {
        fprintf(stderr, "Ошибка в %s: точка a слишком велика\n", func_name);
        return 0;
    }
    
    return 1;
}

// Функция переразложения многочлена с валидацией
int polynomial_recomposition(double epsilon, double a, double** result_coeffs,
    size_t degree, ...) {
    // не забыл
    if (!is_valid_epsilon(epsilon, "polynomial_recomposition") ||
        !is_valid_point(a, "polynomial_recomposition") ||
        !is_valid_pointer(result_coeffs, "polynomial_recomposition") ||
        !is_valid_degree(degree, "polynomial_recomposition")) {
        return 0;
    }
    
    // Инициализируем результат в NULL на случай ошибки
    *result_coeffs = NULL;
    
    // Получаем исходные коэффициенты из переменных аргументов
    va_list args;
    va_start(args, degree);
    
    // Выделяем память для исходных коэффициентов
    double* f_coeffs = (double*)malloc((degree + 1) * sizeof(double));
    if (f_coeffs == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память для коэффициентов\n");
        va_end(args);
        return 0;
    }
    
    // Читаем коэффициенты и проверяем их корректность
    for (size_t i = 0; i <= degree; i++) {
        double coeff = va_arg(args, double);
        if (!is_valid_coefficient(coeff, "polynomial_recomposition (input coeff)")) {
            free(f_coeffs);
            va_end(args);
            return 0;
        }
        f_coeffs[i] = coeff;
    }
    va_end(args);
    
    // Выделяем память для коэффициентов результата
    *result_coeffs = (double*)calloc(degree + 1, sizeof(double));
    if (*result_coeffs == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память для результата\n");
        free(f_coeffs);
        return 0;
    }
    
    // Используем формулу Тейлора для вычисления коэффициентов g_k = f^(k)(a) / k!
    for (size_t k = 0; k <= degree; k++) {
        // Вычисляем k-ю производную в точке a
        double derivative = 0.0;
        
        for (size_t n = k; n <= degree; n++) {
            // Для монома f_n * x^n: k-я производная равна f_n * n! / (n-k)! * x^(n-k)
            double coeff = f_coeffs[n];
            
            // Проверка на слишком маленький коэффициент
            if (fabs(coeff) < DBL_MIN * 100) {
                continue; // Пропускаем очень маленькие коэффициенты
            }
            
            // Вычисляем n! / (n-k)! = n * (n-1) * ... * (n-k+1)
            double factorial_ratio = 1.0;
            for (size_t i = n - k + 1; i <= n; i++) {
                // Проверка на переполнение
                if (factorial_ratio > DBL_MAX / i) {
                    fprintf(stderr, "Ошибка: переполнение при вычислении факториала\n");
                    free(f_coeffs);
                    free(*result_coeffs);
                    *result_coeffs = NULL;
                    return 0;
                }
                factorial_ratio *= i;
            }
            
            // Вычисляем a^(n-k) с проверкой переполнения
            double a_power;
            if (n - k == 0) {
                a_power = 1.0;
            } else {
                a_power = pow(a, n - k);
                if (!isfinite(a_power)) {
                    fprintf(stderr, "Ошибка: переполнение при возведении a в степень\n");
                    free(f_coeffs);
                    free(*result_coeffs);
                    *result_coeffs = NULL;
                    return 0;
                }
            }
            
            // Проверка на переполнение при умножении
            double term = coeff * factorial_ratio;
            if (!isfinite(term)) {
                fprintf(stderr, "Ошибка: переполнение при умножении коэффициентов\n");
                free(f_coeffs);
                free(*result_coeffs);
                *result_coeffs = NULL;
                return 0;
            }
            
            term *= a_power;
            if (!isfinite(term)) {
                fprintf(stderr, "Ошибка: переполнение при вычислении члена\n");
                free(f_coeffs);
                free(*result_coeffs);
                *result_coeffs = NULL;
                return 0;
            }
            
            derivative += term;
            
            // Проверка на переполнение суммы
            if (!isfinite(derivative)) {
                fprintf(stderr, "Ошибка: переполнение при суммировании производной\n");
                free(f_coeffs);
                free(*result_coeffs);
                *result_coeffs = NULL;
                return 0;
            }
        }
        
        // Вычисляем k!
        double k_factorial = 1.0;
        for (size_t i = 2; i <= k; i++) {
            // Проверка на переполнение факториала
            if (k_factorial > DBL_MAX / i) {
                fprintf(stderr, "Предупреждение: факториал слишком велик, использование приближения\n");
                k_factorial = INFINITY;
                break;
            }
            k_factorial *= i;
        }
        
        // Вычисляем коэффициент g_k = derivative / k!
        double g_k;
        if (k_factorial == 0) {
            g_k = derivative; // Для k=0, k_factorial=1
        } else if (isinf(k_factorial)) {
            g_k = 0.0; // Если факториал бесконечен, коэффициент стремится к 0
        } else {
            g_k = derivative / k_factorial;
        }
        
        // Проверка на корректность результата
        if (!isfinite(g_k)) {
            fprintf(stderr, "Ошибка: некорректное значение коэффициента g_%zu\n", k);
            free(f_coeffs);
            free(*result_coeffs);
            *result_coeffs = NULL;
            return 0;
        }
        
        // Применяем точность: если коэффициент близок к нулю, устанавливаем в 0
        if (fabs(g_k) < epsilon) {
            g_k = 0.0;
        }
        
        (*result_coeffs)[k] = g_k;
    }
    
    free(f_coeffs);
    return 1;
}

// Функция для вычисления значения многочлена в точке x с валидацией
double evaluate_polynomial(const double* coeffs, size_t degree, double x) {
    // не забыл
    if (!is_valid_pointer(coeffs, "evaluate_polynomial") ||
        !is_valid_degree(degree, "evaluate_polynomial") ||
        !is_valid_point(x, "evaluate_polynomial")) {
        return NAN;
    }
    
    double result = 0.0;
    double x_power = 1.0;
    
    // Проверяем коэффициенты
    for (size_t i = 0; i <= degree; i++) {
        if (!is_valid_coefficient(coeffs[i], "evaluate_polynomial")) {
            return NAN;
        }
    }
    
    for (size_t i = 0; i <= degree; i++) {
        // Проверка на переполнение при умножении
        double term = coeffs[i] * x_power;
        if (!isfinite(term)) {
            fprintf(stderr, "Ошибка: переполнение при вычислении значения многочлена\n");
            return NAN;
        }
        
        result += term;
        
        // Проверка на переполнение суммы
        if (!isfinite(result)) {
            fprintf(stderr, "Ошибка: переполнение при суммировании значений многочлена\n");
            return NAN;
        }
        
        // Вычисляем следующую степень x
        if (i < degree) {
            x_power *= x;
            if (!isfinite(x_power)) {
                fprintf(stderr, "Ошибка: переполнение при возведении x в степень\n");
                return NAN;
            }
        }
    }
    
    return result;
}

// Функция для вычисления значения многочлена в форме (x-a) в точке x с валидацией
double evaluate_shifted_polynomial(const double* coeffs, size_t degree, double x, double a) {
    // не забыл
    if (!is_valid_pointer(coeffs, "evaluate_shifted_polynomial") ||
        !is_valid_degree(degree, "evaluate_shifted_polynomial") ||
        !is_valid_point(x, "evaluate_shifted_polynomial") ||
        !is_valid_point(a, "evaluate_shifted_polynomial")) {
        return NAN;
    }
    
    // Проверяем коэффициенты
    for (size_t i = 0; i <= degree; i++) {
        if (!is_valid_coefficient(coeffs[i], "evaluate_shifted_polynomial")) {
            return NAN;
        }
    }
    
    double result = 0.0;
    double x_minus_a_power = 1.0;
    double x_minus_a = x - a;
    
    // Проверка на переполнение при вычитании
    if (!isfinite(x_minus_a)) {
        fprintf(stderr, "Ошибка: некорректное значение (x - a)\n");
        return NAN;
    }
    
    for (size_t i = 0; i <= degree; i++) {
        // Проверка на переполнение при умножении
        double term = coeffs[i] * x_minus_a_power;
        if (!isfinite(term)) {
            fprintf(stderr, "Ошибка: переполнение при вычислении значения смещенного многочлена\n");
            return NAN;
        }
        
        result += term;
        
        // Проверка на переполнение суммы
        if (!isfinite(result)) {
            fprintf(stderr, "Ошибка: переполнение при суммировании значений смещенного многочлена\n");
            return NAN;
        }
        
        // Вычисляем следующую степень (x-a)
        if (i < degree) {
            x_minus_a_power *= x_minus_a;
            if (!isfinite(x_minus_a_power)) {
                fprintf(stderr, "Ошибка: переполнение при возведении (x-a) в степень\n");
                return NAN;
            }
        }
    }
    
    return result;
}

// Функция для красивого вывода многочлена с валидацией
void print_polynomial(const double* coeffs, size_t degree, const char* variable) {
    // не забыл
    if (!is_valid_pointer(coeffs, "print_polynomial") ||
        !is_valid_degree(degree, "print_polynomial") ||
        !is_valid_pointer(variable, "print_polynomial")) {
        printf("ERROR");
        return;
    }
    
    // Проверяем коэффициенты
    for (size_t i = 0; i <= degree; i++) {
        if (!is_valid_coefficient(coeffs[i], "print_polynomial")) {
            printf("ERROR");
            return;
        }
    }
    
    int first = 1;
    
    for (size_t i = degree; i != (size_t)-1; i--) {
        if (fabs(coeffs[i]) > 1e-10) {
            if (!first) {
                printf(" + ");
            }
            
            if (i == 0) {
                printf("%.2f", coeffs[i]);
            }
            else if (i == 1) {
                if (fabs(coeffs[i] - 1.0) < 1e-10) {
                    printf("%s", variable);
                }
                else {
                    printf("%.2f*%s", coeffs[i], variable);
                }
            }
            else {
                if (fabs(coeffs[i] - 1.0) < 1e-10) {
                    printf("%s^%zu", variable, i);
                }
                else {
                    printf("%.2f*%s^%zu", coeffs[i], variable, i);
                }
            }
            
            first = 0;
        }
    }
    
    if (first) {
        printf("0");
    }
}

void print_shifted_polynomial(const double* coeffs, size_t degree, double a, const char* variable) {
    // не забыл
    if (!is_valid_pointer(coeffs, "print_shifted_polynomial") ||
        !is_valid_degree(degree, "print_shifted_polynomial") ||
        !is_valid_point(a, "print_shifted_polynomial") ||
        !is_valid_pointer(variable, "print_shifted_polynomial")) {
        printf("ERROR");
        return;
    }
    
    // Проверяем коэффициенты
    for (size_t i = 0; i <= degree; i++) {
        if (!is_valid_coefficient(coeffs[i], "print_shifted_polynomial")) {
            printf("ERROR");
            return;
        }
    }
    
    int first = 1;
    
    for (size_t i = degree; i != (size_t)-1; i--) {
        if (fabs(coeffs[i]) > 1e-10) {
            if (!first) {
                printf(" + ");
            }
            
            if (i == 0) {
                printf("%.2f", coeffs[i]);
            }
            else if (i == 1) {
                if (fabs(coeffs[i] - 1.0) < 1e-10) {
                    printf("(%s - %.2f)", variable, a);
                }
                else {
                    printf("%.2f*(%s - %.2f)", coeffs[i], variable, a);
                }
            }
            else {
                if (fabs(coeffs[i] - 1.0) < 1e-10) {
                    printf("(%s - %.2f)^%zu", variable, a, i);
                }
                else {
                    printf("%.2f*(%s - %.2f)^%zu", coeffs[i], variable, a, i);
                }
            }
            
            first = 0;
        }
    }
    
    if (first) {
        printf("0");
    }
}

// Демонстрация работы функции
void demo_polynomial_recomposition() {
    printf("Демонстрация переразложения многочлена\n\n");
    
    double* coeffs = NULL;
    double epsilon = 1e-10;
    
    // Тест 1: f(x) = x^2, a = 1
    printf("1. f(x) = x², a = 1:\n");
    printf("   Исходный многочлен: ");
    double test1_coeffs[] = {0, 0, 1};
    print_polynomial(test1_coeffs, 2, "x");
    printf("\n");
    
    if (polynomial_recomposition(epsilon, 1.0, &coeffs, 2, 0.0, 0.0, 1.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 2, 1.0, "x");
        printf("\n");
        
        // Проверка
        printf("   Проверка (значения должны совпадать):\n");
        for (double x = 0.0; x <= 2.0; x += 0.5) {
            double orig_value = evaluate_polynomial(test1_coeffs, 2, x);
            double shifted_value = evaluate_shifted_polynomial(coeffs, 2, x, 1.0);
            
            if (isnan(orig_value) || isnan(shifted_value)) {
                printf("   x = %.1f: Ошибка вычисления\n", x);
            } else {
                printf("   x = %.1f: %.2f = %.2f %s\n",
                    x, orig_value, shifted_value,
                    fabs(orig_value - shifted_value) < epsilon ? "✓" : "✗");
            }
        }
        free(coeffs);
    } else {
        printf("   Ошибка при переразложении\n");
    }
    printf("\n");
    
    // Тест 2: f(x) = x^3 - 3x^2 + 3x - 1 = (x-1)^3, a = 1
    printf("2. f(x) = x³ - 3x² + 3x - 1, a = 1:\n");
    double test2_coeffs[] = {-1, 3, -3, 1};
    printf("   Исходный многочлен: ");
    print_polynomial(test2_coeffs, 3, "x");
    printf("\n");
    
    if (polynomial_recomposition(epsilon, 1.0, &coeffs, 3, -1.0, 3.0, -3.0, 1.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 3, 1.0, "x");
        printf("\n");
        
        printf("   Коэффициенты: ");
        for (size_t i = 0; i <= 3; i++) {
            printf("g%zu = %.2f ", i, coeffs[i]);
        }
        printf("\n");
        free(coeffs);
    } else {
        printf("   Ошибка при переразложении\n");
    }
    printf("\n");
    
    // Тест 3: f(x) = 2x^2 + 3x + 1, a = 2
    printf("3. f(x) = 2x² + 3x + 1, a = 2:\n");
    double test3_coeffs[] = {1, 3, 2};
    printf("   Исходный многочлен: ");
    print_polynomial(test3_coeffs, 2, "x");
    printf("\n");
    
    if (polynomial_recomposition(epsilon, 2.0, &coeffs, 2, 1.0, 3.0, 2.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 2, 2.0, "x");
        printf("\n");
        
        printf("   Коэффициенты: ");
        for (size_t i = 0; i <= 2; i++) {
            printf("g%zu = %.2f ", i, coeffs[i]);
        }
        printf("\n");
        
        // Проверка
        printf("   Проверка:\n");
        for (double x = 1.0; x <= 3.0; x += 0.5) {
            double orig_value = evaluate_polynomial(test3_coeffs, 2, x);
            double shifted_value = evaluate_shifted_polynomial(coeffs, 2, x, 2.0);
            
            if (isnan(orig_value) || isnan(shifted_value)) {
                printf("   x = %.1f: Ошибка вычисления\n", x);
            } else {
                printf("   x = %.1f: %.2f = %.2f %s\n",
                    x, orig_value, shifted_value,
                    fabs(orig_value - shifted_value) < epsilon ? "✓" : "✗");
            }
        }
        free(coeffs);
    } else {
        printf("   Ошибка при переразложении\n");
    }
    printf("\n");
    
    // Тест 4: f(x) = x^4 - 4x^3 + 6x^2 - 4x + 1 = (x-1)^4, a = 1
    printf("4. f(x) = x⁴ - 4x³ + 6x² - 4x + 1, a = 1:\n");
    double test4_coeffs[] = {1, -4, 6, -4, 1};
    printf("   Исходный многочлен: ");
    print_polynomial(test4_coeffs, 4, "x");
    printf("\n");
    
    if (polynomial_recomposition(epsilon, 1.0, &coeffs, 4, 1.0, -4.0, 6.0, -4.0, 1.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 4, 1.0, "x");
        printf("\n");
        
        printf("   Коэффициенты: ");
        for (size_t i = 0; i <= 4; i++) {
            printf("g%zu = %.2f ", i, coeffs[i]);
        }
        printf("\n");
        free(coeffs);
    } else {
        printf("   Ошибка при переразложении\n");
    }
    printf("\n");
    
    // Тест 5: Линейная функция f(x) = 2x + 3, a = 5
    printf("5. f(x) = 2x + 3, a = 5:\n");
    double test5_coeffs[] = {3, 2};
    printf("   Исходный многочлен: ");
    print_polynomial(test5_coeffs, 1, "x");
    printf("\n");
    
    if (polynomial_recomposition(epsilon, 5.0, &coeffs, 1, 3.0, 2.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 1, 5.0, "x");
        printf("\n");
        
        printf("   Коэффициенты: ");
        for (size_t i = 0; i <= 1; i++) {
            printf("g%zu = %.2f ", i, coeffs[i]);
        }
        printf("\n");
        free(coeffs);
    } else {
        printf("   Ошибка при переразложении\n");
    }
    
    // Тест 6: Проверка обработки ошибок
    printf("\n6. Тест обработки ошибок:\n");
    
    // Некорректный epsilon
    printf("   а) Некорректный epsilon: ");
    if (!polynomial_recomposition(-1.0, 1.0, &coeffs, 2, 0.0, 0.0, 1.0)) {
        printf("Корректно обнаружена ошибка\n");
    }
    
    // Некорректная степень
    printf("   б) Слишком высокая степень: ");
    if (!polynomial_recomposition(epsilon, 1.0, &coeffs, 200, 0.0, 0.0, 1.0)) {
        printf("Корректно обнаружена ошибка\n");
    }
    
    // Некорректный коэффициент
    printf("   в) Некорректный коэффициент (inf): ");
    if (!polynomial_recomposition(epsilon, 1.0, &coeffs, 2, INFINITY, 0.0, 1.0)) {
        printf("Корректно обнаружена ошибка\n");
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    printf("Переразложение многочлена по степеням (x - a)\n\n");
    
    demo_polynomial_recomposition();
    
    printf("\nАсимптотическая сложность: O(n²), где n - степень многочлена\n");
    printf("Алгоритм использует формулу Тейлора для вычисления коэффициентов\n");
    printf("Ограничения:\n");
    printf("- Максимальная степень многочлена: %d\n", MAX_POLY_DEGREE);
    printf("- Точность epsilon: положительное число в диапазоне (0, 1]\n");
    
    return 0;
}