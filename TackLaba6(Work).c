#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>

// Вспомогательные функции для специальных форматов

// Преобразование в римские цифры
void int_to_roman(int num, char* buffer) {
    if (num <= 0 || num > 3999) {
        strcpy(buffer, "ERROR");
        return;
    }

    const char* roman_numerals[] = { "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
    const int values[] = { 1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1 };

    buffer[0] = '\0';
    int temp = num;

    for (int i = 0; i < 13; i++) {
        while (temp >= values[i]) {
            strcat(buffer, roman_numerals[i]);
            temp -= values[i];
        }
    }
}

// Цекендорфово представление (числа Фибоначчи)
void uint_to_zeckendorf(unsigned int num, char* buffer) {
    if (num == 0) {
        strcpy(buffer, "01");
        return;
    }

    // Находим числа Фибоначчи до num
    unsigned int fib[50];
    int fib_count = 0;
    fib[0] = 1;
    fib[1] = 2;
    fib_count = 2;

    while (fib[fib_count - 1] <= num && fib_count < 50) {
        fib[fib_count] = fib[fib_count - 1] + fib[fib_count - 2];
        fib_count++;
    }

    // Строим представление
    char temp[100] = { 0 };
    unsigned int remaining = num;
    int started = 0;

    for (int i = fib_count - 1; i >= 0; i--) {
        if (fib[i] <= remaining) {
            strcat(temp, "1");
            remaining -= fib[i];
            started = 1;
        }
        else if (started) {
            strcat(temp, "0");
        }
    }

    strcpy(buffer, temp);
    strcat(buffer, "1");
}

// Преобразование числа в систему счисления с основанием base
void int_to_base(int num, int base, char* buffer, int uppercase) {
    if (base < 2 || base > 36) base = 10;

    if (num == 0) {
        strcpy(buffer, "0");
        return;
    }

    char temp[100];
    int index = 0;
    int is_negative = num < 0;
    unsigned int n = is_negative ? -num : num;

    while (n > 0) {
        int digit = n % base;
        if (digit < 10) {
            temp[index++] = '0' + digit;
        }
        else {
            if (uppercase) {
                temp[index++] = 'A' + digit - 10;
            }
            else {
                temp[index++] = 'a' + digit - 10;
            }
        }
        n /= base;
    }

    if (is_negative) {
        temp[index++] = '-';
    }

    // Переворачиваем строку
    for (int i = 0; i < index / 2; i++) {
        char t = temp[i];
        temp[i] = temp[index - 1 - i];
        temp[index - 1 - i] = t;
    }
    temp[index] = '\0';

    strcpy(buffer, temp);
}

// Преобразование из произвольной системы счисления в десятичную (ДОБАВЛЕНА ЭТА ФУНКЦИЯ)
int base_to_int(const char* str, int base, int uppercase) {
    if (base < 2 || base > 36) base = 10;
    if (str == NULL || strlen(str) == 0) return 0;

    int result = 0;
    int is_negative = 0;
    const char* ptr = str;

    if (*ptr == '-') {
        is_negative = 1;
        ptr++;
    }
    else if (*ptr == '+') {
        ptr++;
    }

    while (*ptr != '\0') {
        char c = *ptr;
        int digit;

        if (isdigit(c)) {
            digit = c - '0';
        }
        else if (isalpha(c)) {
            if (uppercase) {
                digit = toupper(c) - 'A' + 10;
            }
            else {
                digit = tolower(c) - 'a' + 10;
            }
        }
        else {
            break;
        }

        if (digit >= base) break;

        result = result * base + digit;
        ptr++;
    }

    return is_negative ? -result : result;
}

// Дамп памяти для 4-байтного числа
void memory_dump_int(int num, char* buffer) {
    unsigned char* bytes = (unsigned char*)&num;
    buffer[0] = '\0';
    char temp[10];

    for (int i = 0; i < 4; i++) {
        for (int j = 7; j >= 0; j--) {
            temp[7 - j] = (bytes[i] & (1 << j)) ? '1' : '0';
        }
        temp[8] = '\0';

        strcat(buffer, temp);
        if (i < 3) strcat(buffer, " ");
    }
}

void memory_dump_uint(unsigned int num, char* buffer) {
    memory_dump_int((int)num, buffer);
}

// Дамп памяти для double
void memory_dump_double(double num, char* buffer) {
    unsigned char* bytes = (unsigned char*)&num;
    buffer[0] = '\0';
    char temp[10];

    for (int i = 0; i < 8; i++) {
        for (int j = 7; j >= 0; j--) {
            temp[7 - j] = (bytes[i] & (1 << j)) ? '1' : '0';
        }
        temp[8] = '\0';

        strcat(buffer, temp);
        if (i < 7) strcat(buffer, " ");
    }
}

// Дамп памяти для float
void memory_dump_float(float num, char* buffer) {
    unsigned char* bytes = (unsigned char*)&num;
    buffer[0] = '\0';
    char temp[10];

    for (int i = 0; i < 4; i++) {
        for (int j = 7; j >= 0; j--) {
            temp[7 - j] = (bytes[i] & (1 << j)) ? '1' : '0';
        }
        temp[8] = '\0';

        strcat(buffer, temp);
        if (i < 3) strcat(buffer, " ");
    }
}

// Основная функция форматирования
int overfprintf(FILE* stream, const char* format, ...) {
    if (stream == NULL || format == NULL) return -1;

    va_list args;
    va_start(args, format);

    int written = 0;
    const char* ptr = format;

    while (*ptr != '\0') {
        if (*ptr != '%') {
            fputc(*ptr, stream);
            written++;
            ptr++;
            continue;
        }

        ptr++;

        if (*ptr == 'R' && *(ptr + 1) == 'o') {
            int num = va_arg(args, int);
            char buffer[50];
            int_to_roman(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'Z' && *(ptr + 1) == 'r') {
            unsigned int num = va_arg(args, unsigned int);
            char buffer[100];
            uint_to_zeckendorf(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'v') {
            int num = va_arg(args, int);
            int base = va_arg(args, int);
            char buffer[100];
            int_to_base(num, base, buffer, 0);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'V') {
            int num = va_arg(args, int);
            int base = va_arg(args, int);
            char buffer[100];
            int_to_base(num, base, buffer, 1);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 't' && *(ptr + 1) == 'o') {
            const char* str = va_arg(args, const char*);
            int base = va_arg(args, int);
            int result = base_to_int(str, base, 0); // ИСПРАВЛЕНО: добавлена функция base_to_int
            written += fprintf(stream, "%d", result);
            ptr += 2;
        }
        else if (*ptr == 'T' && *(ptr + 1) == 'O') {
            const char* str = va_arg(args, const char*);
            int base = va_arg(args, int);
            int result = base_to_int(str, base, 1); // ИСПРАВЛЕНО: добавлена функция base_to_int
            written += fprintf(stream, "%d", result);
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'i') {
            int num = va_arg(args, int);
            char buffer[100];
            memory_dump_int(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'u') {
            unsigned int num = va_arg(args, unsigned int);
            char buffer[100];
            memory_dump_uint(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'd') {
            double num = va_arg(args, double);
            char buffer[200];
            memory_dump_double(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'f') {
            double num_double = va_arg(args, double);
            float num = (float)num_double;
            char buffer[100];
            memory_dump_float(num, buffer);
            int len = strlen(buffer);
            fwrite(buffer, 1, len, stream);
            written += len;
            ptr += 2;
        }
        else {
            fputc('%', stream);
            written++;
            if (*ptr != '\0') {
                fputc(*ptr, stream);
                written++;
                ptr++;
            }
        }
    }

    va_end(args);
    return written;
}

// Упрощенная реализация oversprintf
int oversprintf(char* str, const char* format, ...) {
    if (str == NULL || format == NULL) return -1;

    va_list args;
    va_start(args, format);

    const char* ptr = format;
    char* dest = str;
    int total_written = 0;

    while (*ptr != '\0') {
        if (*ptr != '%') {
            *dest++ = *ptr++;
            total_written++;
            continue;
        }

        ptr++;

        if (*ptr == 'R' && *(ptr + 1) == 'o') {
            int num = va_arg(args, int);
            char buffer[50];
            int_to_roman(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'Z' && *(ptr + 1) == 'r') {
            unsigned int num = va_arg(args, unsigned int);
            char buffer[100];
            uint_to_zeckendorf(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'v') {
            int num = va_arg(args, int);
            int base = va_arg(args, int);
            char buffer[100];
            int_to_base(num, base, buffer, 0);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'V') {
            int num = va_arg(args, int);
            int base = va_arg(args, int);
            char buffer[100];
            int_to_base(num, base, buffer, 1);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 't' && *(ptr + 1) == 'o') {
            const char* num_str = va_arg(args, const char*);
            int base = va_arg(args, int);
            int num = base_to_int(num_str, base, 0); // ИСПРАВЛЕНО: добавлена функция base_to_int
            int len = sprintf(dest, "%d", num);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'T' && *(ptr + 1) == 'O') {
            const char* num_str = va_arg(args, const char*);
            int base = va_arg(args, int);
            int num = base_to_int(num_str, base, 1); // ИСПРАВЛЕНО: добавлена функция base_to_int
            int len = sprintf(dest, "%d", num);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'i') {
            int num = va_arg(args, int);
            char buffer[100];
            memory_dump_int(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'u') {
            unsigned int num = va_arg(args, unsigned int);
            char buffer[100];
            memory_dump_uint(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'd') {
            double num = va_arg(args, double);
            char buffer[200];
            memory_dump_double(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else if (*ptr == 'm' && *(ptr + 1) == 'f') {
            double num_double = va_arg(args, double);
            float num = (float)num_double;
            char buffer[100];
            memory_dump_float(num, buffer);
            int len = strlen(buffer);
            strcpy(dest, buffer);
            dest += len;
            total_written += len;
            ptr += 2;
        }
        else {
            *dest++ = '%';
            total_written++;
            if (*ptr != '\0') {
                *dest++ = *ptr;
                total_written++;
                ptr++;
            }
        }
    }

    *dest = '\0';
    va_end(args);
    return total_written;
}

void demo_overprintf() {
    printf("=== Демонстрация overfprintf/oversprintf ===\n");

    printf("1. Римские цифры: ");
    overfprintf(stdout, "2023 в римских: %Ro\n", 2023);

    printf("2. Цекендорфово представление: ");
    overfprintf(stdout, "20 в цекендорфовом: %Zr\n", 20U);

    printf("3. Системы счисления: ");
    overfprintf(stdout, "255 в hex: %Cv, в двоичной: %Cv\n", 255, 16, 255, 2);

    printf("4. Преобразование систем: ");
    overfprintf(stdout, "FF из hex: %to, 1010 из двоичной: %to\n", "FF", 16, "1010", 2);

    printf("5. Дамп памяти int: ");
    int test_int = 305419896;
    overfprintf(stdout, "%mi\n", test_int);

    printf("6. Дамп памяти double: ");
    double test_double = 3.14;
    overfprintf(stdout, "%md\n", test_double);

    char buffer[500];
    oversprintf(buffer, "Комбинированный вывод: %Ro, %Zr, %Cv", 42, 15U, 255, 16);
    printf("7. oversprintf: %s\n", buffer);
}

int main() {
    setlocale(LC_ALL, "Russian");
    printf("Демонстрация функций overfprintf и oversprintf\n");
    printf("==============================================\n");

    demo_overprintf();

    return 0;
}
