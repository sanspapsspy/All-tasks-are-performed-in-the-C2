#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>

#define EPS 1e-12

// Функция метода дихотомии
double dichotomy(double a, double b, double epsilon, double (*func)(double)) {
    if (func == NULL || epsilon <= 0) {
        return NAN;
    }

    if (a > b) {
        double temp = a;
        a = b;
        b = temp;
    }

    double fa = func(a);
    double fb = func(b);

    // Проверка наличия корня на интервале
    if (fa * fb > 0) {
        printf("Ошибка: На интервале [%.6f, %.6f] нет корня или их несколько\n", a, b);
        printf("f(a) = %.6f, f(b) = %.6f\n", fa, fb);
        return NAN;
    }

    int iteration = 0;
    double c;

    printf("\nИтерации метода дихотомии:\n");
    printf("┌──────┬────────────┬────────────┬────────────┬────────────┬────────────┬────────────┐\n");
    printf("│ Итер │     a      │     b      │     c      │    f(a)    │    f(b)    │    f(c)    │\n");
    printf("├──────┼────────────┼────────────┼────────────┼────────────┼────────────┼────────────┤\n");

    while (fabs(b - a) > epsilon) {
        iteration++;
        c = (a + b) / 2.0;
        double fc = func(c);

        printf("│ %4d │ %10.6f │ %10.6f │ %10.6f │ %10.6f │ %10.6f │ %10.6f │\n",
            iteration, a, b, c, fa, fb, fc);

        if (fabs(fc) < epsilon) {
            break; // Найден достаточно точный корень
        }

        if (fa * fc < 0) {
            b = c;
            fb = fc;
        }
        else {
            a = c;
            fa = fc;
        }

        // Защита от бесконечного цикла
        if (iteration > 1000) {
            printf("│ %4d │ %10s │ %10s │ %10s │ %10s │ %10s │ %10s │\n",
                iteration, "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT");
            printf("└──────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘\n");
            printf("Превышено максимальное количество итераций (1000)\n");
            break;
        }
    }

    printf("└──────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘\n");

    double final_root = (a + b) / 2.0;
    printf("Итоговый корень: %.8f\n", final_root);
    printf("Значение функции в корне: %.2e\n", func(final_root));
    printf("Достигнутая точность: %.2e\n", fabs(b - a));
    printf("Количество итераций: %d\n\n", iteration);

    return final_root;
}

// Тестовые функции

// Квадратное уравнение: x^2 - 4 = 0, корни: -2, 2
double quadratic(double x) {
    return x * x - 4;
}

// Тригонометрическое уравнение: sin(x) = 0, корни: k*π
double trigonometric(double x) {
    return sin(x);
}

// Экспоненциальное уравнение: e^x - 2 = 0, корень: ln(2) ≈ 0.693147
double exponential(double x) {
    return exp(x) - 2;
}

// Логарифмическое уравнение: ln(x) - 1 = 0, корень: e ≈ 2.71828
double logarithmic(double x) {
    return log(x) - 1;
}

// Кубическое уравнение: x^3 - 2x - 5 = 0, корень около 2.094
double cubic(double x) {
    return x * x * x - 2 * x - 5;
}

// Сложная функция: x^3 * sin(x) - 2x + 1
double complex_function(double x) {
    return x * x * x * sin(x) - 2 * x + 1;
}

// Функция для тестирования метода на разных интервалах и точности
void test_dichotomy(double (*func)(double), const char* func_name,
    double a, double b, double epsilon, const char* expected_info) {
    printf("================================================================================\n");
    printf("Функция: %s\n", func_name);
    printf("Интервал: [%.2f, %.2f]\n", a, b);
    printf("Точность: %.0e\n", epsilon);
    printf("Ожидаемый результат: %s\n", expected_info);
    printf("--------------------------------------------------------------------------------\n");

    double root = dichotomy(a, b, epsilon, func);

    if (!isnan(root)) {
        printf("✓ Успешно найден корень: %.8f\n", root);
    }
    else {
        printf("✗ Не удалось найти корень на заданном интервале\n");
    }
    printf("================================================================================\n\n");
}

void demo_dichotomy() {
    printf("ДЕМОНСТРАЦИЯ МЕТОДА ДИХОТОМИИ\n");
    printf("==============================\n\n");

    // Тест 1: Квадратное уравнение с разной точностью
    test_dichotomy(quadratic, "x² - 4 = 0", 0.0, 3.0, 1e-3, "корень ≈ 2.0");
    test_dichotomy(quadratic, "x² - 4 = 0", 0.0, 3.0, 1e-6, "корень ≈ 2.0");
    test_dichotomy(quadratic, "x² - 4 = 0", 0.0, 3.0, 1e-9, "корень ≈ 2.0");

    // Тест 2: Тригонометрическое уравнение
    test_dichotomy(trigonometric, "sin(x) = 0", 2.0, 4.0, 1e-6, "корень ≈ π ≈ 3.141593");

    // Тест 3: Экспоненциальное уравнение
    test_dichotomy(exponential, "eˣ - 2 = 0", 0.0, 1.0, 1e-6, "корень ≈ ln(2) ≈ 0.693147");

    // Тест 4: Логарифмическое уравнение
    test_dichotomy(logarithmic, "ln(x) - 1 = 0", 2.0, 3.0, 1e-6, "корень ≈ e ≈ 2.718282");

    // Тест 5: Кубическое уравнение
    test_dichotomy(cubic, "x³ - 2x - 5 = 0", 2.0, 3.0, 1e-6, "корень ≈ 2.094551");

    // Тест 6: Сложная функция
    test_dichotomy(complex_function, "x³·sin(x) - 2x + 1 = 0", 0.5, 1.5, 1e-6, "корень ≈ 0.8-1.0");

    // Тест 7: Ошибочный интервал (нет корня)
    test_dichotomy(quadratic, "x² - 4 = 0", 3.0, 5.0, 1e-6, "нет корня на интервале");

    // Тест 8: Очень высокая точность
    test_dichotomy(trigonometric, "sin(x) = 0", 3.1, 3.2, 1e-12, "корень ≈ π с высокой точностью");
}

// Дополнительная демонстрация: сравнение скорости сходимости для разных точностей
void convergence_comparison() {
    printf("СРАВНЕНИЕ СКОРОСТИ СХОДИМОСТИ ДЛЯ РАЗНЫХ ТОЧНОСТЕЙ\n");
    printf("==================================================\n\n");

    double (*func)(double) = quadratic;
    double a = 0.0, b = 3.0;
    double epsilons[] = { 1e-1, 1e-2, 1e-3, 1e-6, 1e-9, 1e-12 };
    int num_epsilons = sizeof(epsilons) / sizeof(epsilons[0]);

    printf("Функция: x² - 4 = 0, интервал: [%.1f, %.1f]\n", a, b);
    printf("┌──────────────┬──────────────┬──────────────────┐\n");
    printf("│   Точность   │ Итерации │     Корень      │\n");
    printf("├──────────────┼──────────────┼──────────────────┤\n");

    for (int i = 0; i < num_epsilons; i++) {
        double epsilon = epsilons[i];

        // Воспроизводим метод дихотомии для подсчета итераций
        double left = a, right = b;
        double f_left = func(left);
        double f_right = func(right);
        int iterations = 0;

        while (fabs(right - left) > epsilon && iterations < 1000) {
            double mid = (left + right) / 2.0;
            double f_mid = func(mid);

            if (fabs(f_mid) < epsilon) break;

            if (f_left * f_mid < 0) {
                right = mid;
                f_right = f_mid;
            }
            else {
                left = mid;
                f_left = f_mid;
            }
            iterations++;
        }

        double root = (left + right) / 2.0;
        printf("│ %12.0e │ %10d │ %14.8f │\n", epsilon, iterations, root);
    }

    printf("└──────────────┴──────────────┴──────────────────┘\n");
    printf("\nТеоретическая оценка итераций: n ≥ log₂((b-a)/ε)\n");

    for (int i = 0; i < num_epsilons; i++) {
        double epsilon = epsilons[i];
        double theoretical_iter = ceil(log2((b - a) / epsilon));
        printf("Для ε = %.0e: теоретически ≥ %.0f итераций\n", epsilon, theoretical_iter);
    }
}

// Функция для интерактивного тестирования
void interactive_test() {
    printf("\nИНТЕРАКТИВНОЕ ТЕСТИРОВАНИЕ\n");
    printf("===========================\n");

    int choice;
    printf("Выберите функцию для тестирования:\n");
    printf("1. x² - 4 = 0\n");
    printf("2. sin(x) = 0\n");
    printf("3. eˣ - 2 = 0\n");
    printf("4. ln(x) - 1 = 0\n");
    printf("5. x³ - 2x - 5 = 0\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);

    double (*selected_func)(double) = NULL;
    char func_name[100];

    switch (choice) {
    case 1:
        selected_func = quadratic;
        strcpy(func_name, "x² - 4 = 0");
        break;
    case 2:
        selected_func = trigonometric;
        strcpy(func_name, "sin(x) = 0");
        break;
    case 3:
        selected_func = exponential;
        strcpy(func_name, "eˣ - 2 = 0");
        break;
    case 4:
        selected_func = logarithmic;
        strcpy(func_name, "ln(x) - 1 = 0");
        break;
    case 5:
        selected_func = cubic;
        strcpy(func_name, "x³ - 2x - 5 = 0");
        break;
    default:
        printf("Неверный выбор!\n");
        return;
    }

    double a, b, epsilon;
    printf("Введите левую границу интервала: ");
    scanf("%lf", &a);
    printf("Введите правую границу интервала: ");
    scanf("%lf", &b);
    printf("Введите точность (например, 1e-6): ");
    scanf("%lf", &epsilon);

    printf("\nРЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ:\n");
    test_dichotomy(selected_func, func_name, a, b, epsilon, "пользовательский тест");
}

int main() {
    setlocale(LC_ALL, "Russian");

    printf("МЕТОД ДИХОТОМИИ ДЛЯ РЕШЕНИЯ УРАВНЕНИЙ\n");
    printf("=====================================\n\n");

    int choice;
    printf("Выберите режим:\n");
    printf("1. Автоматическая демонстрация\n");
    printf("2. Сравнение сходимости\n");
    printf("3. Интерактивное тестирование\n");
    printf("Ваш выбор: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        demo_dichotomy();
        break;
    case 2:
        convergence_comparison();
        break;
    case 3:
        interactive_test();
        break;
    default:
        printf("Неверный выбор! Запускается автоматическая демонстрация.\n");
        demo_dichotomy();
        break;
    }

    printf("\nАсимптотическая сложность метода дихотомии: O(log((b-a)/ε))\n");
    printf("Метод гарантированно сходится за n ≥ log₂((b-a)/ε) итераций\n");

    return 0;
}
