#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>

// Функция для получения числового значения цифры
int char_to_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1; // Некорректный символ
}

// Функция для преобразования цифры в символ
char digit_to_char(int digit) {
    if (digit >= 0 && digit <= 9) return '0' + digit;
    if (digit >= 10 && digit <= 35) return 'a' + digit - 10;
    return '?'; // Некорректная цифра
}

// Функция сложения двух чисел в заданной системе счисления (сложение в столбик)
char* add_numbers_in_base(const char* num1, const char* num2, int base) {
    if (base < 2 || base > 36 || num1 == NULL || num2 == NULL) {
        return NULL;
    }

    // Определяем длины чисел
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int max_len = (len1 > len2) ? len1 : len2;

    // Выделяем память для результата (максимальная длина + возможный перенос + завершающий ноль)
    char* result = (char*)malloc(max_len + 2);
    if (result == NULL) return NULL;

    int carry = 0; // Перенос
    int index = 0;

    // Складываем цифры справа налево
    for (int i = 0; i < max_len || carry > 0; i++) {
        int digit1 = 0, digit2 = 0;

        // Получаем цифры из первого числа (если они есть)
        if (i < len1) {
            char c1 = num1[len1 - 1 - i];
            digit1 = char_to_digit(c1);
            if (digit1 == -1 || digit1 >= base) {
                free(result);
                return NULL; // Некорректная цифра
            }
        }

        // Получаем цифры из второго числа (если они есть)
        if (i < len2) {
            char c2 = num2[len2 - 1 - i];
            digit2 = char_to_digit(c2);
            if (digit2 == -1 || digit2 >= base) {
                free(result);
                return NULL; // Некорректная цифра
            }
        }

        // Суммируем цифры и перенос
        int sum = digit1 + digit2 + carry;
        carry = sum / base;
        int digit = sum % base;

        // Записываем цифру в результат
        result[index++] = digit_to_char(digit);
    }

    result[index] = '\0';

    // Переворачиваем результат (так как складывали справа налево)
    for (int i = 0; i < index / 2; i++) {
        char temp = result[i];
        result[i] = result[index - 1 - i];
        result[index - 1 - i] = temp;
    }

    return result;
}

// Функция для удаления ведущих нулей
char* remove_leading_zeros(const char* num) {
    if (num == NULL) return NULL;

    // Пропускаем ведущие нули
    const char* start = num;
    while (*start == '0' && *(start + 1) != '\0') {
        start++;
    }

    // Если все цифры были нулями, возвращаем "0"
    if (*start == '\0') {
        char* result = (char*)malloc(2);
        result[0] = '0';
        result[1] = '\0';
        return result;
    }

    // Копируем оставшуюся часть
    return strdup(start);
}

// Основная функция суммирования с переменным числом аргументов
char* sum_in_base(int base, int count, ...) {
    if (base < 2 || base > 36 || count <= 0) {
        return NULL;
    }

    va_list args;
    va_start(args, count);

    // Начинаем с нуля
    char* total = (char*)malloc(2);
    if (total == NULL) {
        va_end(args);
        return NULL;
    }
    total[0] = '0';
    total[1] = '\0';

    for (int i = 0; i < count; i++) {
        const char* num_str = va_arg(args, const char*);

        // Пропускаем NULL или пустые строки
        if (num_str == NULL || strlen(num_str) == 0) {
            continue;
        }

        // Проверяем корректность числа
        for (int j = 0; num_str[j] != '\0'; j++) {
            int digit = char_to_digit(num_str[j]);
            if (digit == -1 || digit >= base) {
                free(total);
                va_end(args);
                return NULL; // Некорректное число
            }
        }

        // Складываем текущее total с новым числом
        char* new_total = add_numbers_in_base(total, num_str, base);
        free(total);

        if (new_total == NULL) {
            va_end(args);
            return NULL;
        }

        total = new_total;
    }

    va_end(args);

    // Убираем ведущие нули из результата
    char* cleaned_result = remove_leading_zeros(total);
    free(total);

    return cleaned_result;
}

// Функция для демонстрации работы
void demo_sum_in_base() {
    printf("=== Демонстрация суммирования в произвольной системе счисления ===\n\n");

    // Тест 1: Суммирование в десятичной системе
    printf("1. Суммирование в 10-чной системе:\n");
    char* result1 = sum_in_base(10, 4, "123", "456", "789", "1000");
    if (result1) {
        printf("   123 + 456 + 789 + 1000 = %s\n", result1);
        free(result1);
    }

    // Тест 2: Суммирование в двоичной системе
    printf("\n2. Суммирование в 2-чной системе:\n");
    char* result2 = sum_in_base(2, 3, "1010", "1101", "1111");
    if (result2) {
        printf("   1010 + 1101 + 1111 = %s\n", result2);
        free(result2);
    }

    // Тест 3: Суммирование в шестнадцатеричной системе
    printf("\n3. Суммирование в 16-чной системе:\n");
    char* result3 = sum_in_base(16, 3, "A", "B", "C");
    if (result3) {
        printf("   A + B + C = %s\n", result3);
        free(result3);
    }

    // Тест 4: Большие числа (которые не помещаются в стандартные типы)
    printf("\n4. Суммирование больших чисел:\n");
    char* result4 = sum_in_base(10, 2, "99999999999999999999", "1");
    if (result4) {
        printf("   99999999999999999999 + 1 = %s\n", result4);
        free(result4);
    }

    // Тест 5: Суммирование в 36-ричной системе
    printf("\n5. Суммирование в 36-чной системе:\n");
    char* result5 = sum_in_base(36, 3, "z", "1", "a");
    if (result5) {
        printf("   z + 1 + a = %s\n", result5);
        free(result5);
    }

    // Тест 6: С одним числом
    printf("\n6. С одним числом:\n");
    char* result6 = sum_in_base(10, 1, "42");
    if (result6) {
        printf("   42 = %s\n", result6);
        free(result6);
    }

    // Тест 7: С нулями
    printf("\n7. С нулями:\n");
    char* result7 = sum_in_base(10, 3, "0", "000", "0");
    if (result7) {
        printf("   0 + 000 + 0 = %s\n", result7);
        free(result7);
    }
}

// Демонстрация работы функции сложения в столбик
void demo_addition() {
    printf("\n=== Демонстрация сложения в столбик ===\n\n");

    struct TestCase {
        const char* num1;
        const char* num2;
        int base;
        const char* expected;
    } tests[] = {
        {"123", "456", 10, "579"},
        {"1010", "1101", 2, "10111"},
        {"A", "B", 16, "15"},
        {"999", "1", 10, "1000"},
        {"zz", "1", 36, "100"},
        {"0", "123", 10, "123"},
        {"", "456", 10, "456"} // Пустая строка должна игнорироваться
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("Тест %d: %s + %s (основание %d)\n", i + 1,
            tests[i].num1, tests[i].num2, tests[i].base);

        char* result = add_numbers_in_base(tests[i].num1, tests[i].num2, tests[i].base);
        if (result) {
            printf("   Результат: %s\n", result);
            printf("   Ожидалось: %s\n", tests[i].expected);
            printf("   %s\n\n", strcmp(result, tests[i].expected) == 0 ? "✓ ПРОЙДЕН" : "✗ ОШИБКА");
            free(result);
        }
        else {
            printf("   ОШИБКА: не удалось вычислить сумму\n\n");
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    printf("СУММИРОВАНИЕ ЧИСЕЛ В ПРОИЗВОЛЬНОЙ СИСТЕМЕ СЧИСЛЕНИЯ\n");
    printf("===================================================\n\n");

    demo_addition();
    demo_sum_in_base();

    printf("\nАсимптотическая сложность:\n");
    printf("- Сложение в столбик: O(max(m, n)), где m и n - длины чисел\n");
    printf("- Суммирование k чисел: O(k * max_length)\n");

    return 0;
}
