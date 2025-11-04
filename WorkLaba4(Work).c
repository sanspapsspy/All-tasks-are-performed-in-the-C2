#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include<locale.h>

// ==================== ЗАДАНИЕ 4 ====================
double polynomial_value(double x, size_t degree, ...) {
    va_list args;
    va_start(args, degree);

    double result = 0.0;
    double x_power = 1.0;

    for (size_t i = 0; i <= degree; i++) {
        double coefficient = va_arg(args, double);
        result += coefficient * x_power;
        x_power *= x;
    }

    va_end(args);
    return result;
}

void demo_task() {
    printf("=== Задание 4: Вычисление значения многочлена ===\n");

    double value = polynomial_value(2.0, 2, 2.0, 3.0, 1.0);
    printf("2x^2 + 3x + 1 при x=2: %.2f\n", value);

    value = polynomial_value(1.0, 3, 1.0, 0.0, -2.0, 5.0);
    printf("x^3 - 2x + 5 при x=1: %.2f\n", value);

    printf("\n");
}
int main() {
    setlocale(LC_ALL, "Rus");
    printf("Демонстрация выполненных заданий\n\n");

    
    demo_task();
    
    printf("Все демонстрации завершены.\n");
    return 0;
}
