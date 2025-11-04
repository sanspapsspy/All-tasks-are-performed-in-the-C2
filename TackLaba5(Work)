#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include<locale.h>

#define EPS 1e-12

// Функция для проверки, является ли число числом Капрекара в заданной системе счисления
int is_kaprekar(const char* num_str, int base, double epsilon) {
    if (base < 2 || base > 36) return 0;
    if (num_str == NULL || strlen(num_str) == 0) return 0;

    // Преобразование строки в число
    long long num = 0;
    for (int i = 0; num_str[i] != '\0'; i++) {
        char c = num_str[i];
        int digit;
        if (isdigit(c)) {
            digit = c - '0';
        }
        else if (isalpha(c)) {
            digit = tolower(c) - 'a' + 10;
        }
        else {
            return 0; // Некорректный символ
        }

        if (digit >= base) return 0; // Некорректная цифра для данной системы счисления

        num = num * base + digit;
    }

    if (num == 0) return 0; // 0 не считается числом Капрекара

    long long square = num * num;

    // Преобразование квадрата в строку в заданной системе счисления
    char square_str[100] = { 0 };
    long long temp = square;
    int len = 0;

    while (temp > 0) {
        int digit = temp % base;
        square_str[len++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        temp /= base;
    }

    // Переворачиваем строку
    for (int i = 0; i < len / 2; i++) {
        char temp_c = square_str[i];
        square_str[i] = square_str[len - 1 - i];
        square_str[len - 1 - i] = temp_c;
    }
    square_str[len] = '\0';

    // Проверяем все возможные разбиения
    for (int split = 1; split < len; split++) {
        // Левая часть
        long long left = 0;
        for (int i = 0; i < split; i++) {
            char c = square_str[i];
            int digit = (isdigit(c)) ? (c - '0') : (tolower(c) - 'a' + 10);
            left = left * base + digit;
        }

        // Правая часть
        long long right = 0;
        for (int i = split; i < len; i++) {
            char c = square_str[i];
            int digit = (isdigit(c)) ? (c - '0') : (tolower(c) - 'a' + 10);
            right = right * base + digit;
        }

        // Проверяем условие Капрекара
        if (right > 0 && left + right == num) {
            return 1;
        }
    }

    return 0;
}

// Функция с переменным числом аргументов для поиска чисел Капрекара
const char* find_kaprekar_numbers(int base, double epsilon, int count, ...) {
    if (base < 2 || base > 36 || count <= 0) {
        return NULL;
    }

    va_list args;
    va_start(args, count);

    const char* result = NULL;

    for (int i = 0; i < count; i++) {
        const char* num_str = va_arg(args, const char*);
        if (is_kaprekar(num_str, base, epsilon)) {
            result = num_str;
            break;
        }
    }

    va_end(args);
    return result;
}

void main() {
    setlocale(LC_ALL, "Rus");
    printf("=== Демонстрация поиска чисел Капрекара ===\n");

    // В десятичной системе: 9, 45, 55, 99 - числа Капрекара
    const char* result1 = find_kaprekar_numbers(10, EPS, 4, "9", "10", "45", "99");
    if (result1) {
        printf("Найдено число Капрекара: %s\n", result1);
    }

    const char* result2 = find_kaprekar_numbers(10, EPS, 3, "10", "20", "30");
    if (!result2) {
        printf("Числа Капрекара не найдены среди {10, 20, 30}\n");
    }
}
