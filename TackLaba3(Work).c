#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include<locale.h>

typedef struct {
    double x, y;
} Point;

double cross_product(const Point* a, const Point* b, const Point* c) {
    return (b->x - a->x) * (c->y - a->y) - (b->y - a->y) * (c->x - a->x);
}

int is_convex_polygon(double epsilon, size_t vertex_count, ...) {
    if (vertex_count < 3) return 0;

    Point* vertices = malloc(vertex_count * sizeof(Point));
    if (vertices == NULL) return 0;

    va_list args;
    va_start(args, vertex_count);

    for (size_t i = 0; i < vertex_count; i++) {
        vertices[i].x = va_arg(args, double);
        vertices[i].y = va_arg(args, double);
    }
    va_end(args);

    int sign = 0;

    for (size_t i = 0; i < vertex_count; i++) {
        const Point* a = &vertices[i];
        const Point* b = &vertices[(i + 1) % vertex_count];
        const Point* c = &vertices[(i + 2) % vertex_count];

        double cross = cross_product(a, b, c);

        if (fabs(cross) > epsilon) {
            int current_sign = (cross > 0) ? 1 : -1;

            if (sign == 0) {
                sign = current_sign;
            }
            else if (sign != current_sign) {
                free(vertices);
                return 0;
            }
        }
    }

    free(vertices);
    return 1;
}

void demo_task() {
    printf("=== Задание 3: Проверка выпуклости многоугольника ===\n");

    double epsilon = 1e-12;

    int convex = is_convex_polygon(epsilon, 4,
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0);
    printf("Квадрат (выпуклый): %s\n", convex ? "Да" : "Нет");

    convex = is_convex_polygon(epsilon, 4,
        0.0, 0.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 1.0);
    printf("Невыпуклый многоугольник: %s\n", convex ? "Да" : "Нет");

    convex = is_convex_polygon(epsilon, 3,
        0.0, 0.0,
        1.0, 0.0,
        0.5, 1.0);
    printf("Треугольник (выпуклый): %s\n", convex ? "Да" : "Нет");

    printf("\n");
}
int main() {
    setlocale(LC_ALL, "Rus");
    printf("Демонстрация выполненной задачи\n\n");

    demo_task();

    printf("Все демонстрации завершены.\n");
    return 0;
}
