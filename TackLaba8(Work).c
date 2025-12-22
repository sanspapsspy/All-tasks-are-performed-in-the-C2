
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

// Вспомогательные функции для валидации
int is_valid_function_pointer(double (*func)(double), const char* func_name) {
    if (func == NULL) {
        fprintf(stderr, "Ошибка в %s: NULL указатель на функцию\n", func_name);
        return 0;
    }
    return 1;
}

int is_valid_epsilon(double epsilon) {
    if (epsilon <= 0) {
        fprintf(stderr, "Ошибка: epsilon должен быть положительным\n");
        return 0;
    }
    
    if (epsilon < DBL_MIN * 10) {
        fprintf(stderr, "Ошибка: epsilon слишком мал\n");
        return 0;
    }
    
    if (epsilon > 1.0) {
        fprintf(stderr, "Ошибка: epsilon слишком велик\n");
        return 0;
    }
    
    return 1;
}

int is_valid_interval(double a, double b, const char* func_name) {
    if (!isfinite(a) || !isfinite(b)) {
        fprintf(stderr, "Ошибка в %s: границы интервала должны быть конечными числами\n", func_name);
        return 0;
    }
    
    // Проверка на слишком большой интервал
    if (fabs(b - a) > 1e9) {
        fprintf(stderr, "Ошибка в %s: интервал слишком велик\n", func_name);
        return 0;
    }
    
    return 1;
}

int is_valid_function_value(double value, const char* func_name) {
    if (!isfinite(value)) {
        fprintf(stderr, "Ошибка в %s: значение функции не конечно\n", func_name);
        return 0;
    }
    
    // Проверка на слишком большие значения
    if (fabs(value) > 1e100) {
        fprintf(stderr, "Ошибка в %s: значение функции слишком велико\n", func_name);
        return 0;
    }
    
    return 1;
}

// Функция метода дихотомии с валидацией
double dichotomy(double a, double b, double epsilon, double (*func)(double)) {
    // не забыл
    if (!is_valid_function_pointer(func, "dichotomy") || 
        !is_valid_epsilon(epsilon) || 
        !is_valid_interval(a, b, "dichotomy")) {
        return NAN;
    }
    
    // Проверяем, что функция возвращает корректные значения на границах
    errno = 0;
    double fa = func(a);
    if (errno != 0 || !is_valid_function_value(fa, "dichotomy (f(a))")) {
        fprintf(stderr, "Ошибка: функция не может быть вычислена в точке a = %.6f\n", a);
        return NAN;
    }
    
    errno = 0;
    double fb = func(b);
    if (errno != 0 || !is_valid_function_value(fb, "dichotomy (f(b))")) {
        fprintf(stderr, "Ошибка: функция не может быть вычислена в точке b = %.6f\n", b);
        return NAN;
    }
    
    // Нормализуем интервал: a <= b
    if (a > b) {
        double temp = a;
        a = b;
        b = temp;
        
        double temp_f = fa;
        fa = fb;
        fb = temp_f;
    }
    
    // Проверка наличия корня на интервале
    if (fa * fb > 0) {
        printf("Ошибка: На интервале [%.6f, %.6f] нет корня или их несколько\n", a, b);
        printf("f(a) = %.6f, f(b) = %.6f\n", fa, fb);
        return NAN;
    }
    
    // Проверка, что функция определена на всем интервале
    // (добавим несколько тестовых точек)
    int num_test_points = 5;
    for (int i = 1; i < num_test_points; i++) {
        double test_point = a + (b - a) * i / num_test_points;
        errno = 0;
        double ftest = func(test_point);
        if (errno != 0 || !isfinite(ftest)) {
            fprintf(stderr, "Ошибка: функция не определена в точке %.6f\n", test_point);
            return NAN;
        }
    }
    
    int iteration = 0;
    double c;
    
    printf("Итерации метода дихотомии:\n");
    printf("┌──────┬────────────┬────────────┬────────────┬────────────┬────────────┬────────────┐\n");
    printf("│ Итер │     a      │     b      │     c      │    f(a)    │    f(b)    │    f(c)    │\n");
    printf("├──────┼────────────┼────────────┼────────────┼────────────┼────────────┼────────────┤\n");
    
    // Контроль точности и переполнения
    double last_c = 0;
    int precision_stagnation = 0;
    
    while (fabs(b - a) > epsilon) {
        iteration++;
        
        // Проверка на слишком большое количество итераций
        if (iteration > 1000) {
            printf("│ %4d │ %10s │ %10s │ %10s │ %10s │ %10s │ %10s │\n",
                iteration, "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT", "PREKRAT");
            printf("└──────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘\n");
            printf("Превышено максимальное количество итераций (1000)\n");
            return NAN;
        }
        
        c = (a + b) / 2.0;
        
        // Проверка на застой точности
        if (iteration > 1 && fabs(c - last_c) < epsilon / 1000) {
            precision_stagnation++;
            if (precision_stagnation > 10) {
                printf("Застой точности: алгоритм не улучшает точность\n");
                break;
            }
        }
        last_c = c;
        
        errno = 0;
        double fc = func(c);
        if (errno != 0 || !is_valid_function_value(fc, "dichotomy (f(c))")) {
            fprintf(stderr, "Ошибка при вычислении функции в точке c = %.6f\n", c);
            return NAN;
        }
        
        printf("│ %4d │ %10.6f │ %10.6f │ %10.6f │ %10.6f │ %10.6f │ %10.6f │\n",
            iteration, a, b, c, fa, fb, fc);
        
        // Проверка на достижение достаточной точности
        if (fabs(fc) < epsilon) {
            break;
        }
        
        // Проверка на численную нестабильность
        if (fabs(fa - fb) < epsilon * epsilon) {
            printf("Численная нестабильность: значения функции слишком близки\n");
            break;
        }
        
        // Обновление интервала
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        }
        else {
            a = c;
            fa = fc;
        }
        
        // Проверка на вырождение интервала
        if (fabs(b - a) < DBL_MIN * 10) {
            printf("Интервал выродился в точку\n");
            break;
        }
    }
    
    printf("└──────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┘\n");
    
    double final_root = (a + b) / 2.0;
    
    // Проверка окончательного результата
    errno = 0;
    double f_final = func(final_root);
    if (errno != 0 || !is_valid_function_value(f_final, "dichotomy (final)")) {
        fprintf(stderr, "Ошибка при проверке окончательного результата\n");
        return NAN;
    }
    
    printf("Итоговый корень: %.8f\n", final_root);
    printf("Значение функции в корне: %.2e\n", f_final);
    printf("Достигнутая точность: %.2e\n", fabs(b - a));
    printf("Количество итераций: %d\n\n", iteration);
    
    return final_root;
}

// Тестовые функции с защитой от ошибок вычислений

// Квадратное уравнение: x^2 - 4 = 0, корни: -2, 2
double quadratic(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    double result = x * x - 4;
    
    // Проверка на переполнение
    if (!isfinite(result)) {
        errno = ERANGE;
        return copysign(HUGE_VAL, result);
    }
    
    return result;
}

// Тригонометрическое уравнение: sin(x) = 0, корни: k*π
double trigonometric(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    double result = sin(x);
    
    if (!isfinite(result)) {
        errno = EDOM;
        return NAN;
    }
    
    return result;
}

// Экспоненциальное уравнение: e^x - 2 = 0, корень: ln(2) ≈ 0.693147
double exponential(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    // Проверка на слишком большой аргумент
    if (x > 100) {
        errno = ERANGE;
        return HUGE_VAL;
    }
    
    double exp_x = exp(x);
    if (!isfinite(exp_x)) {
        errno = ERANGE;
        return exp_x;
    }
    
    return exp_x - 2;
}

// Логарифмическое уравнение: ln(x) - 1 = 0, корень: e ≈ 2.71828
double logarithmic(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    if (x <= 0) {
        errno = EDOM;
        return NAN;
    }
    
    double result = log(x) - 1;
    
    if (!isfinite(result)) {
        errno = EDOM;
        return NAN;
    }
    
    return result;
}

// Кубическое уравнение: x^3 - 2x - 5 = 0, корень около 2.094
double cubic(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    // Проверка на переполнение при возведении в куб
    if (fabs(x) > pow(DBL_MAX, 1.0/3.0)) {
        errno = ERANGE;
        return copysign(HUGE_VAL, x*x*x);
    }
    
    double x3 = x * x * x;
    double result = x3 - 2 * x - 5;
    
    if (!isfinite(result)) {
        errno = ERANGE;
        return result;
    }
    
    return result;
}

// Сложная функция: x^3 * sin(x) - 2x + 1
double complex_function(double x) {
    // не забыл
    if (!isfinite(x)) {
        errno = EDOM;
        return NAN;
    }
    
    // Проверка на переполнение при возведении в куб
    if (fabs(x) > pow(DBL_MAX, 1.0/3.0)) {
        errno = ERANGE;
        return copysign(HUGE_VAL, x*x*x);
    }
    
    double x3 = x * x * x;
    double sin_x = sin(x);
    
    if (!isfinite(x3) || !isfinite(sin_x)) {
        errno = ERANGE;
        return NAN;
    }
    
    double result = x3 * sin_x - 2 * x + 1;
    
    if (!isfinite(result)) {
        errno = ERANGE;
        return result;
    }
    
    return result;
}

// Функция для безопасного ввода чисел
double safe_input_double(const char* prompt, double min_val, double max_val) {
    double value;
    char buffer[100];
    
    while (1) {
        printf("%s", prompt);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Ошибка ввода\n");
            exit(EXIT_FAILURE);
        }
        
        char* endptr;
        errno = 0;
        value = strtod(buffer, &endptr);
        
        if (errno == ERANGE) {
            printf("Число вне допустимого диапазона\n");
            continue;
        }
        
        if (endptr == buffer) {
            printf("Неверный формат числа\n");
            continue;
        }
        
        if (!isfinite(value)) {
            printf("Число должно быть конечным\n");
            continue;
        }
        
        if (value < min_val || value > max_val) {
            printf("Число должно быть в диапазоне [%.2f, %.2f]\n", min_val, max_val);
            continue;
        }
        
        break;
    }
    
    return value;
}

// Функция для тестирования метода на разных интервалах и точности
void test_dichotomy(double (*func)(double), const char* func_name,
    double a, double b, double epsilon, const char* expected_info) {
    // не забыл
    if (!is_valid_function_pointer(func, "test_dichotomy") ||
        !is_valid_interval(a, b, "test_dichotomy") ||
        !is_valid_epsilon(epsilon)) {
        printf("Ошибка: некорректные параметры для теста\n");
        return;
    }
    
    printf("Функция: %s\n", func_name);
    printf("Интервал: [%.2f, %.2f]\n", a, b);
    printf("Точность: %.0e\n", epsilon);
    printf("Ожидаемый результат: %s\n", expected_info);
    
    double root = dichotomy(a, b, epsilon, func);
    
    if (!isnan(root)) {
        printf("Успешно найден корень: %.8f\n", root);
    }
    else {
        printf("Не удалось найти корень на заданном интервале\n");
    }
    printf("\n");
}

void demo_dichotomy() {
    printf("Демонстрация метода дихотомии\n\n");
    
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
    
    // Тест 9: Проверка обработки ошибок
    printf("Тест обработки ошибок:\n");
    test_dichotomy(logarithmic, "ln(x) - 1 = 0", -1.0, 0.5, 1e-6, "должна быть ошибка - логарифм от отрицательного");
}

// Дополнительная демонстрация: сравнение скорости сходимости для разных точностей
void convergence_comparison() {
    printf("Сравнение скорости сходимости для разных точностей\n\n");
    
    double (*func)(double) = quadratic;
    double a = 0.0, b = 3.0;
    double epsilons[] = { 1e-1, 1e-2, 1e-3, 1e-6, 1e-9, 1e-12 };
    int num_epsilons = sizeof(epsilons) / sizeof(epsilons[0]);
    
    if (!is_valid_interval(a, b, "convergence_comparison")) {
        return;
    }
    
    printf("Функция: x² - 4 = 0, интервал: [%.1f, %.1f]\n", a, b);
    printf("┌──────────────┬──────────────┬──────────────────┐\n");
    printf("│   Точность   │ Итерации │     Корень      │\n");
    printf("├──────────────┼──────────────┼──────────────────┤\n");
    
    for (int i = 0; i < num_epsilons; i++) {
        double epsilon = epsilons[i];
        
        if (!is_valid_epsilon(epsilon)) {
            continue;
        }
        
        // Воспроизводим метод дихотомии для подсчета итераций
        double left = a, right = b;
        
        // Проверяем функцию на границах
        errno = 0;
        double f_left = func(left);
        double f_right = func(right);
        
        if (errno != 0 || !is_valid_function_value(f_left, "convergence (left)") ||
            !is_valid_function_value(f_right, "convergence (right)")) {
            printf("│ %12.0e │ %10s │ %14s │\n", epsilon, "ERROR", "ERROR");
            continue;
        }
        
        // Проверка наличия корня
        if (f_left * f_right > 0) {
            printf("│ %12.0e │ %10s │ %14s │\n", epsilon, "NO_ROOT", "NO_ROOT");
            continue;
        }
        
        int iterations = 0;
        
        while (fabs(right - left) > epsilon && iterations < 1000) {
            double mid = (left + right) / 2.0;
            
            errno = 0;
            double f_mid = func(mid);
            if (errno != 0 || !is_valid_function_value(f_mid, "convergence (mid)")) {
                break;
            }
            
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
        if (is_valid_epsilon(epsilon)) {
            double theoretical_iter = ceil(log2((b - a) / epsilon));
            printf("Для ε = %.0e: теоретически ≥ %.0f итераций\n", epsilon, theoretical_iter);
        }
    }
}

// Функция для интерактивного тестирования
void interactive_test() {
    printf("Интерактивное тестирование\n");
    
    int choice;
    printf("Выберите функцию для тестирования:\n");
    printf("1. x² - 4 = 0\n");
    printf("2. sin(x) = 0\n");
    printf("3. eˣ - 2 = 0\n");
    printf("4. ln(x) - 1 = 0\n");
    printf("5. x³ - 2x - 5 = 0\n");
    printf("Ваш выбор: ");
    
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Ошибка при чтении выбора\n");
        while (getchar() != '\n'); // Очистка буфера
        return;
    }
    
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
    
    // Очистка буфера после scanf
    while (getchar() != '\n');
    
    double a = safe_input_double("Введите левую границу интервала: ", -1e9, 1e9);
    double b = safe_input_double("Введите правую границу интервала: ", -1e9, 1e9);
    double epsilon = safe_input_double("Введите точность (например, 1e-6): ", DBL_MIN, 1.0);
    
    printf("Результаты тестирования:\n");
    test_dichotomy(selected_func, func_name, a, b, epsilon, "пользовательский тест");
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    printf("Метод дихотомии для решения уравнений\n\n");
    
    int choice;
    printf("Выберите режим:\n");
    printf("1. Автоматическая демонстрация\n");
    printf("2. Сравнение сходимости\n");
    printf("3. Интерактивное тестирование\n");
    printf("Ваш выбор: ");
    
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Ошибка при чтении выбора\n");
        return EXIT_FAILURE;
    }
    
    // Очистка буфера
    while (getchar() != '\n');
    
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
    
    printf("Асимптотическая сложность метода дихотомии: O(log((b-a)/ε))\n");
    printf("Метод гарантированно сходится за n ≥ log₂((b-a)/ε) итераций\n");
    
    return 0;
}
