#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>

#define EPS 1e-12

// Функция переразложения многочлена
int polynomial_recomposition(double epsilon, double a, double** result_coeffs,
    size_t degree, ...) {
    if (result_coeffs == NULL) {
        return 0;
    }

    // Выделяем память для коэффициентов результата
    *result_coeffs = (double*)calloc(degree + 1, sizeof(double));
    if (*result_coeffs == NULL) {
        return 0;
    }

    // Получаем исходные коэффициенты
    va_list args;
    va_start(args, degree);

    // Выделяем память для исходных коэффициентов
    double* f_coeffs = (double*)malloc((degree + 1) * sizeof(double));
    if (f_coeffs == NULL) {
        va_end(args);
        free(*result_coeffs);
        *result_coeffs = NULL;
        return 0;
    }

    // Читаем коэффициенты из переменных аргументов
    for (size_t i = 0; i <= degree; i++) {
        f_coeffs[i] = va_arg(args, double);
    }
    va_end(args);

    // Используем формулу Тейлора для вычисления коэффициентов g_k = f^(k)(a) / k!
    for (size_t k = 0; k <= degree; k++) {
        // Вычисляем k-ю производную в точке a
        double derivative = 0.0;

        for (size_t n = k; n <= degree; n++) {
            // Для монома f_n * x^n: k-я производная равна f_n * n! / (n-k)! * x^(n-k)
            if (n >= k) {
                double coeff = f_coeffs[n];

                // Вычисляем n! / (n-k)! = n * (n-1) * ... * (n-k+1)
                double factorial_ratio = 1.0;
                for (size_t i = n - k + 1; i <= n; i++) {
                    factorial_ratio *= i;
                }

                derivative += coeff * factorial_ratio * pow(a, n - k);
            }
        }

        // Делим на k! чтобы получить коэффициент g_k
        double k_factorial = 1.0;
        for (size_t i = 2; i <= k; i++) {
            k_factorial *= i;
        }

        (*result_coeffs)[k] = derivative / k_factorial;

        // Применяем точность: если коэффициент близок к нулю, устанавливаем в 0
        if (fabs((*result_coeffs)[k]) < epsilon) {
            (*result_coeffs)[k] = 0.0;
        }
    }

    free(f_coeffs);
    return 1;
}

// Функция для вычисления значения многочлена в точке x
double evaluate_polynomial(const double* coeffs, size_t degree, double x) {
    double result = 0.0;
    double x_power = 1.0;

    for (size_t i = 0; i <= degree; i++) {
        result += coeffs[i] * x_power;
        x_power *= x;
    }

    return result;
}

// Функция для вычисления значения многочлена в форме (x-a) в точке x
double evaluate_shifted_polynomial(const double* coeffs, size_t degree, double x, double a) {
    double result = 0.0;
    double x_minus_a_power = 1.0;

    for (size_t i = 0; i <= degree; i++) {
        result += coeffs[i] * x_minus_a_power;
        x_minus_a_power *= (x - a);
    }

    return result;
}

// Функция для красивого вывода многочлена
void print_polynomial(const double* coeffs, size_t degree, const char* variable) {
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
    printf("=== Демонстрация переразложения многочлена ===\n\n");

    double* coeffs = NULL;
    double epsilon = 1e-10;

    // Тест 1: f(x) = x^2, a = 1
    // f(x) = x^2 = (x-1+1)^2 = (x-1)^2 + 2(x-1) + 1
    printf("1. f(x) = x², a = 1:\n");
    printf("   Исходный многочлен: ");
    print_polynomial((double[]) { 0, 0, 1 }, 2, "x");
    printf("\n");

    if (polynomial_recomposition(epsilon, 1.0, &coeffs, 2, 0.0, 0.0, 1.0)) {
        printf("   Переразложение: ");
        print_shifted_polynomial(coeffs, 2, 1.0, "x");
        printf("\n");

        // Проверка: вычисляем оба представления в нескольких точках
        printf("   Проверка (значения должны совпадать):\n");
        for (double x = 0.0; x <= 2.0; x += 0.5) {
            double orig_value = evaluate_polynomial((double[]) { 0, 0, 1 }, 2, x);
            double shifted_value = evaluate_shifted_polynomial(coeffs, 2, x, 1.0);
            printf("   x = %.1f: %.2f = %.2f %s\n",
                x, orig_value, shifted_value,
                fabs(orig_value - shifted_value) < epsilon ? "✓" : "✗");
        }
        free(coeffs);
    }
    printf("\n");

    // Тест 2: f(x) = x^3 - 3x^2 + 3x - 1 = (x-1)^3, a = 1
    // Должны получить: g0 = 0, g1 = 0, g2 = 0, g3 = 1
    printf("2. f(x) = x³ - 3x² + 3x - 1, a = 1:\n");
    printf("   Исходный многочлен: ");
    print_polynomial((double[]) { -1, 3, -3, 1 }, 3, "x");
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
    }
    printf("\n");

    // Тест 3: f(x) = 2x^2 + 3x + 1, a = 2
    // f(x) = 2(x-2+2)^2 + 3(x-2+2) + 1 = 2[(x-2)^2 + 4(x-2) + 4] + 3(x-2) + 6 + 1
    // = 2(x-2)^2 + 8(x-2) + 8 + 3(x-2) + 7 = 2(x-2)^2 + 11(x-2) + 15
    printf("3. f(x) = 2x² + 3x + 1, a = 2:\n");
    printf("   Исходный многочлен: ");
    print_polynomial((double[]) { 1, 3, 2 }, 2, "x");
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
            double orig_value = evaluate_polynomial((double[]) { 1, 3, 2 }, 2, x);
            double shifted_value = evaluate_shifted_polynomial(coeffs, 2, x, 2.0);
            printf("   x = %.1f: %.2f = %.2f %s\n",
                x, orig_value, shifted_value,
                fabs(orig_value - shifted_value) < epsilon ? "✓" : "✗");
        }
        free(coeffs);
    }
    printf("\n");

    // Тест 4: f(x) = x^4 - 4x^3 + 6x^2 - 4x + 1 = (x-1)^4, a = 1
    printf("4. f(x) = x⁴ - 4x³ + 6x² - 4x + 1, a = 1:\n");
    printf("   Исходный многочлен: ");
    print_polynomial((double[]) { 1, -4, 6, -4, 1 }, 4, "x");
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
    }
    printf("\n");

    // Тест 5: Линейная функция f(x) = 2x + 3, a = 5
    printf("5. f(x) = 2x + 3, a = 5:\n");
    printf("   Исходный многочлен: ");
    print_polynomial((double[]) { 3, 2 }, 1, "x");
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
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    printf("ПЕРЕРАЗЛОЖЕНИЕ МНОГОЧЛЕНА ПО СТЕПЕНЯМ (x - a)\n");
    printf("=============================================\n\n");

    demo_polynomial_recomposition();

    printf("\nАсимптотическая сложность: O(n²), где n - степень многочлена\n");
    printf("Алгоритм использует формулу Тейлора для вычисления коэффициентов\n");

    return 0;
}
