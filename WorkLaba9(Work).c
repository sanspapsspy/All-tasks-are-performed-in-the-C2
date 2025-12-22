 #include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>
#include <limits.h>
#include <errno.h>

// Максимальные размеры для безопасности
#define MAX_NUMBER_LENGTH 1000
#define MIN_BASE 2
#define MAX_BASE 36

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

// Проверка системы счисления
int is_valid_base(int base, const char* func_name) {
    // не забыл
    if (base < MIN_BASE || base > MAX_BASE) {
        fprintf(stderr, "Ошибка в %s: недопустимая система счисления %d (допустимо %d-%d)\n", 
                func_name, base, MIN_BASE, MAX_BASE);
        return 0;
    }
    return 1;
}

// Проверка корректности строки с числом
int is_valid_number_string(const char* str, int base, const char* func_name) {
    // не забыл
    if (!is_valid_pointer(str, func_name)) {
        return 0;
    }
    
    size_t len = strlen(str);
    if (len == 0) {
        fprintf(stderr, "Ошибка в %s: пустая строка\n", func_name);
        return 0;
    }
    
    if (len > MAX_NUMBER_LENGTH) {
        fprintf(stderr, "Ошибка в %s: число слишком длинное (макс. %d цифр)\n", 
                func_name, MAX_NUMBER_LENGTH);
        return 0;
    }
    
    // Проверка на недопустимые символы
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        int digit;
        
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit = c - 'A' + 10;
        } else {
            fprintf(stderr, "Ошибка в %s: недопустимый символ '%c'\n", func_name, c);
            return 0;
        }
        
        if (digit >= base) {
            fprintf(stderr, "Ошибка в %s: цифра %d недопустима в системе счисления %d\n", 
                    func_name, digit, base);
            return 0;
        }
    }
    
    return 1;
}

// Функция для получения числового значения цифры с валидацией
int char_to_digit(char c, const char* func_name) {
    // не забыл
    int digit;
    
    if (c >= '0' && c <= '9') {
        digit = c - '0';
    } else if (c >= 'a' && c <= 'z') {
        digit = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'Z') {
        digit = c - 'A' + 10;
    } else {
        fprintf(stderr, "Ошибка в %s: недопустимый символ '%c'\n", func_name, c);
        return -1;
    }
    
    return digit;
}

// Функция для преобразования цифры в символ с валидацией
char digit_to_char(int digit, const char* func_name) {
    // не забыл
    if (digit < 0) {
        fprintf(stderr, "Ошибка в %s: отрицательная цифра %d\n", func_name, digit);
        return '?';
    }
    
    if (digit <= 9) {
        return '0' + digit;
    } else if (digit <= 35) {
        return 'a' + digit - 10;
    } else {
        fprintf(stderr, "Ошибка в %s: цифра %d вне диапазона 0-35\n", func_name, digit);
        return '?';
    }
}

// Функция сложения двух чисел в заданной системе счисления (сложение в столбик)
char* add_numbers_in_base(const char* num1, const char* num2, int base) {
    // не забыл
    if (!is_valid_number_string(num1, base, "add_numbers_in_base (num1)") ||
        !is_valid_number_string(num2, base, "add_numbers_in_base (num2)") ||
        !is_valid_base(base, "add_numbers_in_base")) {
        return NULL;
    }
    
    int len1 = (int)strlen(num1);
    int len2 = (int)strlen(num2);
    int max_len = (len1 > len2) ? len1 : len2;
    
    // Проверка на переполнение при выделении памяти
    if (max_len > INT_MAX - 2) {
        fprintf(stderr, "Ошибка: слишком большое число для сложения\n");
        return NULL;
    }
    
    // Выделяем память для результата с запасом для переноса
    char* result = (char*)malloc(max_len + 3); // +2 для возможного переноса, +1 для '\0'
    if (result == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память для результата\n");
        return NULL;
    }
    
    int carry = 0;
    int index = 0;
    int i;
    
    // Складываем цифры справа налево
    for (i = 0; i < max_len || carry > 0; i++) {
        int digit1 = 0, digit2 = 0;
        
        // Получаем цифры из первого числа
        if (i < len1) {
            char c1 = num1[len1 - 1 - i];
            digit1 = char_to_digit(c1, "add_numbers_in_base (digit1)");
            if (digit1 == -1) {
                free(result);
                return NULL;
            }
        }
        
        // Получаем цифры из второго числа
        if (i < len2) {
            char c2 = num2[len2 - 1 - i];
            digit2 = char_to_digit(c2, "add_numbers_in_base (digit2)");
            if (digit2 == -1) {
                free(result);
                return NULL;
            }
        }
        
        // Проверка на переполнение при сложении
        if (digit1 > INT_MAX - digit2 - carry) {
            fprintf(stderr, "Ошибка: переполнение при сложении цифр\n");
            free(result);
            return NULL;
        }
        
        int sum = digit1 + digit2 + carry;
        
        // Проверка на корректность суммы
        if (sum < 0) {
            fprintf(stderr, "Ошибка: отрицательная сумма цифр\n");
            free(result);
            return NULL;
        }
        
        carry = sum / base;
        int digit = sum % base;
        
        // Проверка индекса для предотвращения переполнения буфера
        if (index >= max_len + 2) {
            fprintf(stderr, "Ошибка: переполнение буфера результата\n");
            free(result);
            return NULL;
        }
        
        result[index++] = digit_to_char(digit, "add_numbers_in_base (result digit)");
        if (result[index-1] == '?') {
            free(result);
            return NULL;
        }
    }
    
    result[index] = '\0';
    
    // Проверка, что результат не пустой
    if (index == 0) {
        result[0] = '0';
        result[1] = '\0';
        index = 1;
    }
    
    // Переворачиваем результат
    for (int j = 0; j < index / 2; j++) {
        char temp = result[j];
        result[j] = result[index - 1 - j];
        result[index - 1 - j] = temp;
    }
    
    // Проверка корректности результата
    if (!is_valid_number_string(result, base, "add_numbers_in_base (result)")) {
        free(result);
        return NULL;
    }
    
    return result;
}

// Функция для удаления ведущих нулей с валидацией
char* remove_leading_zeros(const char* num, const char* func_name) {
    // не забыл
    if (!is_valid_pointer(num, func_name)) {
        return NULL;
    }
    
    // Пропускаем ведущие нули
    const char* start = num;
    while (*start == '0' && *(start + 1) != '\0') {
        start++;
    }
    
    // Если все цифры были нулями, возвращаем "0"
    if (*start == '\0' || (*start == '0' && *(start + 1) == '\0')) {
        char* result = (char*)malloc(2);
        if (result == NULL) {
            fprintf(stderr, "Ошибка: не удалось выделить память для '0'\n");
            return NULL;
        }
        result[0] = '0';
        result[1] = '\0';
        return result;
    }
    
    // Копируем оставшуюся часть
    char* result = strdup(start);
    if (result == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память для копии строки\n");
    }
    
    return result;
}

// Основная функция суммирования с переменным числом аргументов
char* sum_in_base(int base, int count, ...) {
    // не забыл
    if (!is_valid_base(base, "sum_in_base") || count <= 0) {
        fprintf(stderr, "Ошибка: некорректные параметры для sum_in_base\n");
        return NULL;
    }
    
    if (count > 1000) {
        fprintf(stderr, "Ошибка: слишком много чисел для суммирования (макс. 1000)\n");
        return NULL;
    }
    
    va_list args;
    va_start(args, count);
    
    // Начинаем с нуля
    char* total = (char*)malloc(2);
    if (total == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память для начального значения\n");
        va_end(args);
        return NULL;
    }
    total[0] = '0';
    total[1] = '\0';
    
    int numbers_processed = 0;
    
    for (int i = 0; i < count; i++) {
        const char* num_str = va_arg(args, const char*);
        numbers_processed++;
        
        // Пропускаем NULL указатели
        if (num_str == NULL) {
            fprintf(stderr, "Предупреждение: пропущен NULL указатель на число №%d\n", numbers_processed);
            continue;
        }
        
        // Проверяем корректность числа
        if (!is_valid_number_string(num_str, base, "sum_in_base (arg)")) {
            fprintf(stderr, "Ошибка: некорректное число №%d: '%s'\n", numbers_processed, num_str);
            free(total);
            va_end(args);
            return NULL;
        }
        
        // Проверяем, не слишком ли большое число
        if (strlen(num_str) > MAX_NUMBER_LENGTH) {
            fprintf(stderr, "Ошибка: число №%d слишком длинное\n", numbers_processed);
            free(total);
            va_end(args);
            return NULL;
        }
        
        // Складываем текущее total с новым числом
        char* new_total = add_numbers_in_base(total, num_str, base);
        if (new_total == NULL) {
            fprintf(stderr, "Ошибка при сложении числа №%d\n", numbers_processed);
            free(total);
            va_end(args);
            return NULL;
        }
        
        free(total);
        total = new_total;
        
        // Проверка на слишком большой промежуточный результат
        if (strlen(total) > MAX_NUMBER_LENGTH * 2) {
            fprintf(stderr, "Ошибка: промежуточный результат стал слишком большим\n");
            free(total);
            va_end(args);
            return NULL;
        }
    }
    
    va_end(args);
    
    // Проверка, что мы обработали хотя бы одно корректное число
    if (strcmp(total, "0") == 0 && numbers_processed > 0) {
        // Это нормально, если все числа были нулями или пропущены
    }
    
    // Убираем ведущие нули из результата
    char* cleaned_result = remove_leading_zeros(total, "sum_in_base (cleanup)");
    if (cleaned_result == NULL) {
        fprintf(stderr, "Ошибка при удалении ведущих нулей\n");
        free(total);
        return NULL;
    }
    
    free(total);
    
    return cleaned_result;
}

// Функция для демонстрации работы
void demo_sum_in_base() {
    printf("Демонстрация суммирования в произвольной системе счисления\n\n");
    
    // Тест 1: Суммирование в десятичной системе
    printf("1. Суммирование в 10-чной системе:\n");
    char* result1 = sum_in_base(10, 4, "123", "456", "789", "1000");
    if (result1) {
        printf("   123 + 456 + 789 + 1000 = %s\n", result1);
        free(result1);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 2: Суммирование в двоичной системе
    printf("\n2. Суммирование в 2-чной системе:\n");
    char* result2 = sum_in_base(2, 3, "1010", "1101", "1111");
    if (result2) {
        printf("   1010 + 1101 + 1111 = %s\n", result2);
        free(result2);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 3: Суммирование в шестнадцатеричной системе
    printf("\n3. Суммирование в 16-чной системе:\n");
    char* result3 = sum_in_base(16, 3, "A", "B", "C");
    if (result3) {
        printf("   A + B + C = %s\n", result3);
        free(result3);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 4: Большие числа
    printf("\n4. Суммирование больших чисел:\n");
    char* result4 = sum_in_base(10, 2, "99999999999999999999", "1");
    if (result4) {
        printf("   99999999999999999999 + 1 = %s\n", result4);
        free(result4);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 5: Суммирование в 36-ричной системе
    printf("\n5. Суммирование в 36-чной системе:\n");
    char* result5 = sum_in_base(36, 3, "z", "1", "a");
    if (result5) {
        printf("   z + 1 + a = %s\n", result5);
        free(result5);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 6: С одним числом
    printf("\n6. С одним числом:\n");
    char* result6 = sum_in_base(10, 1, "42");
    if (result6) {
        printf("   42 = %s\n", result6);
        free(result6);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 7: С нулями
    printf("\n7. С нулями:\n");
    char* result7 = sum_in_base(10, 3, "0", "000", "0");
    if (result7) {
        printf("   0 + 000 + 0 = %s\n", result7);
        free(result7);
    } else {
        printf("   Ошибка при вычислении\n");
    }
    
    // Тест 8: Обработка ошибок
    printf("\n8. Тест обработки ошибок:\n");
    char* result8 = sum_in_base(10, 2, "123", "45a"); // Некорректный символ
    if (result8) {
        printf("   Результат: %s\n", result8);
        free(result8);
    } else {
        printf("   Корректно обнаружена ошибка: некорректный символ в числе\n");
    }
    
    // Тест 9: Некорректная система счисления
    printf("\n9. Тест с некорректной системой счисления:\n");
    char* result9 = sum_in_base(1, 2, "123", "456");
    if (result9) {
        printf("   Результат: %s\n", result9);
        free(result9);
    } else {
        printf("   Корректно обнаружена ошибка: некорректная система счисления\n");
    }
}

// Демонстрация работы функции сложения в столбик
void demo_addition() {
    printf("Демонстрация сложения в столбик\n\n");
    
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
        {"", "456", 10, "ERROR"} // Пустая строка должна вызывать ошибку
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("Тест %d: %s + %s (основание %d)\n", i + 1,
            tests[i].num1, tests[i].num2, tests[i].base);
        
        char* result = add_numbers_in_base(tests[i].num1, tests[i].num2, tests[i].base);
        if (result) {
            printf("   Результат: %s\n", result);
            printf("   Ожидалось: %s\n", tests[i].expected);
            printf("   %s\n\n", strcmp(result, tests[i].expected) == 0 ? "ПРОЙДЕН" : "ОШИБКА");
            free(result);
        } else {
            if (strlen(tests[i].num1) == 0 || strlen(tests[i].num2) == 0) {
                printf("   Корректно обнаружена ошибка: пустая строка\n\n");
            } else {
                printf("   ОШИБКА: не удалось вычислить сумму\n\n");
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    
    printf("Суммирование чисел в произвольной системе счисления\n\n");
    
    demo_addition();
    demo_sum_in_base();
    
    printf("\nАсимптотическая сложность:\n");
    printf("- Сложение в столбик: O(max(m, n)), где m и n - длины чисел\n");
    printf("- Суммирование k чисел: O(k * max_length)\n");
    printf("\nОграничения:\n");
    printf("- Максимальная длина числа: %d цифр\n", MAX_NUMBER_LENGTH);
    printf("- Допустимые системы счисления: %d-%d\n", MIN_BASE, MAX_BASE);
    printf("- Максимальное количество чисел: 1000\n");
    
    return 0;
}