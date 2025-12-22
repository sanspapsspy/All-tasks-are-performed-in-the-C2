#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>
#include <limits.h>
#include <errno.h>

#define EPS 1e-12
#define MAX_DIGITS 50
#define MAX_INPUT_LENGTH 100

// ВАЛИДАЦИЯ И БЕЗОПАСНОЕ ВЫДЕЛЕНИЕ ПАМЯТИ

// Безопасное выделение памяти с проверкой
void* safe_malloc(size_t size, const char* func_name) {
    if (size == 0) {
        fprintf(stderr, "Ошибка в %s: попытка выделить 0 байт\n", func_name);
        return NULL;
    }
    
    if (size > SIZE_MAX / 2) {
        fprintf(stderr, "Ошибка в %s: слишком большой размер запроса (%zu байт)\n", func_name, size);
        return NULL;
    }
    
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Ошибка в %s: не удалось выделить %zu байт памяти\n", func_name, size);
        perror("malloc");
    }
    
    return ptr;
}

void* safe_calloc(size_t num, size_t size, const char* func_name) {
    if (num == 0 || size == 0) {
        fprintf(stderr, "Ошибка в %s: неверные параметры calloc (num=%zu, size=%zu)\n", func_name, num, size);
        return NULL;
    }
    
    // Проверка на переполнение
    if (num > SIZE_MAX / size) {
        fprintf(stderr, "Ошибка в %s: переполнение при вычислении размера (%zu * %zu)\n", func_name, num, size);
        return NULL;
    }
    
    void* ptr = calloc(num, size);
    if (ptr == NULL) {
        fprintf(stderr, "Ошибка в %s: не удалось выделить %zu * %zu байт памяти\n", func_name, num, size);
        perror("calloc");
    }
    
    return ptr;
}

// Безопасное освобождение памяти
void safe_free(void** ptr) {
    if (ptr != NULL && *ptr != NULL) {
        free(*ptr);
        *ptr = NULL;
    }
}

// Проверка корректности строки
int is_valid_string(const char* str, int max_length) {
    if (str == NULL) {
        fprintf(stderr, "Ошибка: получен NULL указатель\n");
        return 0;
    }
    
    if (strlen(str) > max_length) {
        fprintf(stderr, "Ошибка: строка слишком длинная (макс. %d символов)\n", max_length);
        return 0;
    }
    
    if (strlen(str) == 0) {
        fprintf(stderr, "Ошибка: пустая строка\n");
        return 0;
    }
    
    return 1;
}

// Проверка корректности системы счисления
int is_valid_base(int base) {
    if (base < 2 || base > 36) {
        fprintf(stderr, "Ошибка: недопустимая система счисления %d (допустимо 2-36)\n", base);
        return 0;
    }
    return 1;
}

// ФУНКЦИЯ ДЛЯ ПРОВЕРКИ ЧИСЕЛ КАПРЕКАРА

// Функция для проверки, является ли число числом Капрекара в заданной системе счисления
int is_kaprekar(const char* num_str, int base, double epsilon) {
    // Валидация входных параметров
    if (!is_valid_string(num_str, MAX_INPUT_LENGTH)) {
        return 0;
    }
    
    if (!is_valid_base(base)) {
        return 0;
    }
    
    if (epsilon < 0 || epsilon > 1.0) {
        fprintf(stderr, "Ошибка: недопустимое значение epsilon (должно быть 0-1)\n");
        return 0;
    }
    
    // Преобразование строки в число с проверкой переполнения
    long long num = 0;
    int length = strlen(num_str);
    
    for (int i = 0; i < length; i++) {
        char c = num_str[i];
        int digit;
        
        if (isdigit((unsigned char)c)) {
            digit = c - '0';
        }
        else if (isalpha((unsigned char)c)) {
            digit = tolower((unsigned char)c) - 'a' + 10;
        }
        else {
            fprintf(stderr, "Ошибка: некорректный символ '%c' в числе\n", c);
            return 0;
        }

        if (digit >= base) {
            fprintf(stderr, "Ошибка: цифра %d недопустима в системе счисления %d\n", digit, base);
            return 0;
        }

        // Проверка на переполнение перед умножением
        if (num > LLONG_MAX / base) {
            fprintf(stderr, "Ошибка: переполнение при преобразовании строки в число\n");
            return 0;
        }
        
        num = num * base + digit;
        
        // Проверка на отрицательное число (переполнение в отрицательную сторону)
        if (num < 0) {
            fprintf(stderr, "Ошибка: число слишком большое для long long\n");
            return 0;
        }
    }

    if (num == 0) {
        return 0;
    }

    // Проверка на переполнение при возведении в квадрат
    if (num > sqrt(LLONG_MAX)) {
        fprintf(stderr, "Ошибка: квадрат числа %lld вызовет переполнение\n", num);
        return 0;
    }
    
    long long square = num * num;
    
    // Выделение памяти для строки квадрата числа
    char* square_str = (char*)safe_calloc(MAX_DIGITS + 2, sizeof(char), "is_kaprekar");
    if (square_str == NULL) {
        return 0;
    }
    
    long long temp = square;
    int len = 0;

    // Преобразование квадрата в строку в заданной системе счисления
    while (temp > 0 && len < MAX_DIGITS) {
        int digit = temp % base;
        square_str[len++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        temp /= base;
        
        if (len >= MAX_DIGITS) {
            fprintf(stderr, "Предупреждение: достигнут предел длины числа\n");
            break;
        }
    }
    
    if (len == 0) {
        square_str[len++] = '0';
    }
    square_str[len] = '\0';

    // Переворачиваем строку
    for (int i = 0; i < len / 2; i++) {
        char temp_c = square_str[i];
        square_str[i] = square_str[len - 1 - i];
        square_str[len - 1 - i] = temp_c;
    }

    int is_kaprekar_result = 0;
    
    // Проверяем все возможные разбиения
    for (int split = 1; split < len && !is_kaprekar_result; split++) {
        // Левая часть
        long long left = 0;
        for (int i = 0; i < split; i++) {
            char c = square_str[i];
            int digit = (isdigit((unsigned char)c)) ? (c - '0') : (tolower((unsigned char)c) - 'a' + 10);
            
            if (left > LLONG_MAX / base) {
                break;
            }
            
            left = left * base + digit;
        }

        // Правая часть
        long long right = 0;
        for (int i = split; i < len; i++) {
            char c = square_str[i];
            int digit = (isdigit((unsigned char)c)) ? (c - '0') : (tolower((unsigned char)c) - 'a' + 10);
            
            if (right > LLONG_MAX / base) {
                break;
            }
            
            right = right * base + digit;
        }

        // Проверяем условие Капрекара
        if (right > 0 && left + right == num) {
            is_kaprekar_result = 1;
        }
    }

    safe_free((void**)&square_str);
    
    return is_kaprekar_result;
}

// ФУНКЦИЯ С ПЕРЕМЕННЫМ ЧИСЛОМ АРГУМЕНТОВ

// Функция с переменным числом аргументов для поиска чисел Капрекара
const char* find_kaprekar_numbers(int base, double epsilon, int count, ...) {
    // Валидация параметров
    if (!is_valid_base(base)) {
        return NULL;
    }
    
    if (count <= 0) {
        fprintf(stderr, "Ошибка: количество чисел должно быть положительным\n");
        return NULL;
    }
    
    if (count > 100) {
        fprintf(stderr, "Ошибка: слишком много чисел для проверки (%d > 100)\n", count);
        return NULL;
    }
    
    if (epsilon < 0 || epsilon > 1.0) {
        fprintf(stderr, "Ошибка: некорректное значение epsilon\n");
        return NULL;
    }
    
    va_list args;
    va_start(args, count);

    const char* result = NULL;

    for (int i = 0; i < count; i++) {
        const char* num_str = va_arg(args, const char*);
        
        if (!is_valid_string(num_str, MAX_INPUT_LENGTH)) {
            fprintf(stderr, "Ошибка: некорректная строка №%d\n", i + 1);
            continue;
        }
        
        printf("Проверка числа '%s' в системе счисления %d...\n", num_str, base);
        
        if (is_kaprekar(num_str, base, epsilon)) {
            result = num_str;
            break;
        }
    }

    va_end(args);
    return result;
}

// Проверка ввода строки
char* safe_input_string(const char* prompt) {
    printf("%s", prompt);
    
    char* buffer = (char*)safe_malloc(MAX_INPUT_LENGTH + 2, "safe_input_string");
    if (buffer == NULL) {
        return NULL;
    }
    
    if (fgets(buffer, MAX_INPUT_LENGTH + 1, stdin) == NULL) {
        fprintf(stderr, "Ошибка при чтении ввода\n");
        safe_free((void**)&buffer);
        return NULL;
    }
    
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return buffer;
}

// Демонстрация работы с различными системами счисления
void demonstrate_kaprekar() {
    printf("\nДемонстрация работы с числами Капрекара\n\n");
    
    // Тест 1: десятичная система
    printf("Тест 1: Десятичная система (основание 10)\n");
    const char* decimal_test[] = {"9", "45", "55", "99", "297", "703"};
    printf("Проверяемые числа: ");
    for (int i = 0; i < 6; i++) printf("%s ", decimal_test[i]);
    printf("\n");
    
    const char* result = find_kaprekar_numbers(10, EPS, 6, 
                                               decimal_test[0], decimal_test[1], 
                                               decimal_test[2], decimal_test[3],
                                               decimal_test[4], decimal_test[5]);
    if (result) {
        printf("Найдено число Капрекара: %s\n", result);
    } else {
        printf("Числа Капрекара не найдены\n");
    }
    
    // Тест 2: проверка некорректных данных
    printf("\nТест 2: Проверка некорректных данных\n");
    result = find_kaprekar_numbers(10, EPS, 3, "10", "20", "30");
    if (!result) {
        printf("Числа Капрекара не найдены среди {10, 20, 30} - корректно\n");
    }
    
    // Тест 3: двоичная система
    printf("\nТест 3: Двоичная система (основание 2)\n");
    result = find_kaprekar_numbers(2, EPS, 3, "1", "10", "11");
    if (result) {
        printf("Найдено число Капрекара в двоичной системе: %s\n", result);
    }
    
    // Тест 4: проверка на ошибки
    printf("\nТест 4: Проверка обработки ошибок\n");
    result = find_kaprekar_numbers(37, EPS, 1, "123");
    if (!result) {
        printf("Обработка ошибки сработала корректно\n");
    }
}

// ГЛАВНАЯ ФУНКЦИЯ

int main() {
    setlocale(LC_ALL, "Rus");
    printf("Программа поиска чисел Капрекара\n");
    printf("С валидацией данных и безопасным выделением памяти\n\n");
    
    demonstrate_kaprekar();
    
    // Интерактивный режим
    printf("\nИнтерактивный режим\n");
    printf("Хотите проверить свои числа? (y/n): ");
    
    char choice;
    if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
        getchar();
        
        char* base_str = safe_input_string("Введите систему счисления (2-36): ");
        if (base_str != NULL) {
            int base = atoi(base_str);
            safe_free((void**)&base_str);
            
            if (is_valid_base(base)) {
                printf("Введите числа для проверки (через пробел, макс. 10 чисел):\n");
                printf("Для завершения введите 'end':\n");
                
                const char* numbers[10];
                int count = 0;
                
                while (count < 10) {
                    char* num = safe_input_string("> ");
                    if (num == NULL) break;
                    
                    if (strcmp(num, "end") == 0) {
                        safe_free((void**)&num);
                        break;
                    }
                    
                    numbers[count++] = num;
                }
                
                if (count > 0) {
                    const char* result;
                    if (count == 1) {
                        result = find_kaprekar_numbers(base, EPS, 1, numbers[0]);
                    } else if (count == 2) {
                        result = find_kaprekar_numbers(base, EPS, 2, numbers[0], numbers[1]);
                    } else if (count == 3) {
                        result = find_kaprekar_numbers(base, EPS, 3, numbers[0], numbers[1], numbers[2]);
                    } else {
                        printf("Для простоты демонстрации проверяем только первые 3 числа\n");
                        result = find_kaprekar_numbers(base, EPS, 3, numbers[0], numbers[1], numbers[2]);
                    }
                    
                    if (result) {
                        printf("\nНайдено число Капрекара: %s\n", result);
                    } else {
                        printf("\nЧисла Капрекара не найдены\n");
                    }
                }
            }
        }
    }
    
    printf("\nПрограмма завершена.\n");
    
    return 0;
}