#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <locale.h>

// Вспомогательные функции для парсинга специальных форматов

// Парсинг римских цифр
int parse_roman(const char* str) {
    if (str == NULL || strlen(str) == 0) return 0;
    
    const char* numerals = "IVXLCDM";
    const int values[] = {1, 5, 10, 50, 100, 500, 1000};
    
    int result = 0;
    int prev_value = 0;
    
    for (int i = strlen(str) - 1; i >= 0; i--) {
        char* pos = strchr(numerals, str[i]);
        if (pos == NULL) return 0;
        
        int value = values[pos - numerals];
        
        if (value < prev_value) {
            result -= value;
        } else {
            result += value;
        }
        prev_value = value;
    }
    
    return result;
}

// Парсинг цекендорфова представления
unsigned int parse_zeckendorf(const char* str) {
    if (str == NULL || strlen(str) == 0) return 0;
    
    if (str[strlen(str) - 1] != '1') return 0;
    
    unsigned int fib[50];
    fib[0] = 1;
    fib[1] = 2;
    int fib_count = 2;
    
    while (fib_count < 50) {
        fib[fib_count] = fib[fib_count - 1] + fib[fib_count - 2];
        fib_count++;
    }
    
    unsigned int result = 0;
    int str_len = strlen(str) - 1;
    int fib_index = str_len - 1;
    
    for (int i = 0; i < str_len; i++) {
        if (str[i] == '1') {
            if (fib_index >= 0 && fib_index < fib_count) {
                result += fib[fib_index];
            }
        } else if (str[i] != '0') {
            return 0;
        }
        fib_index--;
    }
    
    return result;
}

// Парсинг числа из произвольной системы счисления
int parse_base(const char* str, int base, int uppercase) {
    if (base < 2 || base > 36) base = 10;
    if (str == NULL || strlen(str) == 0) return 0;
    
    int result = 0;
    int is_negative = 0;
    const char* ptr = str;
    
    if (*ptr == '-') {
        is_negative = 1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }
    
    while (*ptr != '\0') {
        char c = *ptr;
        int digit;
        
        if (isdigit(c)) {
            digit = c - '0';
        } else if (isalpha(c)) {
            if (uppercase) {
                digit = toupper(c) - 'A' + 10;
            } else {
                digit = tolower(c) - 'a' + 10;
            }
        } else {
            break;
        }
        
        if (digit >= base) break;
        
        result = result * base + digit;
        ptr++;
    }
    
    return is_negative ? -result : result;
}

// Основная функция сканирования
int overfscanf(FILE* stream, const char* format, ...) {
    if (stream == NULL || format == NULL) return -1;
    
    va_list args;
    va_start(args, format);
    
    int read_count = 0;
    const char* ptr = format;
    
    while (*ptr != '\0') {
        if (*ptr != '%') {
            int c = fgetc(stream);
            if (c != *ptr) {
                ungetc(c, stream);
                break;
            }
            ptr++;
            continue;
        }
        
        ptr++;
        
        if (*ptr == 'R' && *(ptr + 1) == 'o') {
            int* num_ptr = va_arg(args, int*);
            char buffer[100];
            if (fscanf(stream, "%99s", buffer) == 1) {
                *num_ptr = parse_roman(buffer);
                read_count++;
            } else {
                break;
            }
            ptr += 2;
        }
        else if (*ptr == 'Z' && *(ptr + 1) == 'r') {
            unsigned int* num_ptr = va_arg(args, unsigned int*);
            char buffer[100];
            if (fscanf(stream, "%99s", buffer) == 1) {
                *num_ptr = parse_zeckendorf(buffer);
                read_count++;
            } else {
                break;
            }
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'v') {
            int* num_ptr = va_arg(args, int*);
            int base = va_arg(args, int);
            char buffer[100];
            if (fscanf(stream, "%99s", buffer) == 1) {
                *num_ptr = parse_base(buffer, base, 0);
                read_count++;
            } else {
                break;
            }
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'V') {
            int* num_ptr = va_arg(args, int*);
            int base = va_arg(args, int);
            char buffer[100];
            if (fscanf(stream, "%99s", buffer) == 1) {
                *num_ptr = parse_base(buffer, base, 1);
                read_count++;
            } else {
                break;
            }
            ptr += 2;
        }
        else {
            char format_spec[3] = { '%', *ptr, '\0' };
            
            switch (*ptr) {
                case 'd': {
                    int* num_ptr = va_arg(args, int*);
                    if (fscanf(stream, format_spec, num_ptr) == 1) {
                        read_count++;
                    } else {
                        break;
                    }
                    break;
                }
                case 'u': {
                    unsigned int* num_ptr = va_arg(args, unsigned int*);
                    if (fscanf(stream, format_spec, num_ptr) == 1) {
                        read_count++;
                    } else {
                        break;
                    }
                    break;
                }
                case 's': {
                    char* str_ptr = va_arg(args, char*);
                    if (fscanf(stream, format_spec, str_ptr) == 1) {
                        read_count++;
                    } else {
                        break;
                    }
                    break;
                }
                default:
                    break;
            }
            ptr++;
        }
    }
    
    va_end(args);
    return read_count;
}

int oversscanf(const char* str, const char* format, ...) {
    if (str == NULL || format == NULL) return -1;
    
    FILE* temp = tmpfile();
    if (temp == NULL) return -1;
    
    fputs(str, temp);
    rewind(temp);
    
    va_list args;
    va_start(args, format);
    
    int result = overfscanf(temp, format, args);
    
    fclose(temp);
    va_end(args);
    return result;
}

void demo_overfscanf() {
    printf("=== Демонстрация overfscanf/oversscanf ===\n");
    
    FILE* test_file = fopen("test_input.txt", "w+");
    if (test_file) {
        fprintf(test_file, "MMXXIII 1001011 FF 1010\n");
        rewind(test_file);
        
        int roman_num, hex_num, bin_num;
        unsigned int zeck_num;
        
        int count = overfscanf(test_file, "%Ro %Zr %Cv %Cv", 
                              &roman_num, &zeck_num, &hex_num, 16, &bin_num, 2);
        
        printf("Прочитано %d значений:\n", count);
        printf("Римские: %d\n", roman_num);
        printf("Цекендорфово: %u\n", zeck_num);
        printf("Hex: %d\n", hex_num);
        printf("Binary: %d\n", bin_num);
        
        fclose(test_file);
        remove("test_input.txt");
    }
    
    const char* test_str = "XLV 1010011 1A 1101";
    int r1, r2, r3;
    unsigned int z1;
    
    int count2 = oversscanf(test_str, "%Ro %Zr %Cv %Cv", 
                           &r1, &z1, &r2, 16, &r3, 2);
    
    printf("oversscanf прочитал %d значений:\n", count2);
    printf("Римские: %d\n", r1);
    printf("Цекендорфово: %u\n", z1);
    printf("Hex: %d\n", r2);
    printf("Binary: %d\n", r3);
}

int main() {
    setlocale(LC_ALL, "Russian");
    printf("Демонстрация функций overfscanf и oversscanf\n");
    printf("============================================\n");
    
    demo_overfscanf();
    
    return 0;
}
