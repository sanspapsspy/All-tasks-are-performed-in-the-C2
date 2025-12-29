
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
#define MAX_STRING_LENGTH 1024
#define MAX_ROMAN_NUMERAL 3999
#define MAX_FIB_COUNT 50
#define MIN_BASE 2
#define MAX_BASE 36

// Вспомогательные функции для валидации
int is_valid_pointer(const void* ptr, const char* func_name) {
    if (ptr == NULL) {
        fprintf(stderr, "Ошибка в %s: NULL указатель\n", func_name);
        return 0;
    }
    return 1;
}

int is_valid_string(const char* str, const char* func_name) {
    if (!is_valid_pointer(str, func_name)) {
        return 0;
    }
    
    size_t len = strlen(str);
    if (len == 0) {
        fprintf(stderr, "Ошибка в %s: пустая строка\n", func_name);
        return 0;
    }
    
    if (len >= MAX_STRING_LENGTH) {
        fprintf(stderr, "Ошибка в %s: строка слишком длинная\n", func_name);
        return 0;
    }
    
    return 1;
}

int is_valid_base(int base) {
    if (base < MIN_BASE || base > MAX_BASE) {
        fprintf(stderr, "Ошибка: система счисления %d вне диапазона %d-%d\n", 
                base, MIN_BASE, MAX_BASE);
        return 0;
    }
    return 1;
}

int is_valid_file(FILE* stream, const char* func_name) {
    if (stream == NULL) {
        fprintf(stderr, "Ошибка в %s: NULL файловый поток\n", func_name);
        return 0;
    }
    
    if (ferror(stream)) {
        fprintf(stderr, "Ошибка в %s: ошибка файлового потока\n", func_name);
        return 0;
    }
    
    return 1;
}

// Парсинг римских цифр с валидацией
int parse_roman(const char* str) {
    // не забыл
    if (!is_valid_string(str, "parse_roman")) {
        return 0;
    }
    
    const char* valid_numerals = "IVXLCDM";
    const int values[] = {1, 5, 10, 50, 100, 500, 1000};
    
    // Проверка допустимых символов
    for (size_t i = 0; i < strlen(str); i++) {
        if (strchr(valid_numerals, str[i]) == NULL) {
            fprintf(stderr, "Ошибка в parse_roman: недопустимый символ '%c'\n", str[i]);
            return 0;
        }
    }
    
    int result = 0;
    int prev_value = 0;
    
    // Ограничение длины для предотвращения переполнения
    size_t len = strlen(str);
    if (len > 15) {
        fprintf(stderr, "Предупреждение: слишком длинная римская цифра\n");
        len = 15;
    }
    
    for (int i = (int)len - 1; i >= 0; i--) {
        char* pos = strchr(valid_numerals, str[i]);
        if (pos == NULL) {
            return 0;
        }
        
        int value = values[pos - valid_numerals];
        
        // Проверка правил вычитания
        if (value < prev_value) {
            // Проверка допустимых вычитаемых комбинаций
            if (!(prev_value == 5 && value == 1) &&   // IV
                !(prev_value == 10 && value == 1) &&  // IX
                !(prev_value == 50 && value == 10) && // XL
                !(prev_value == 100 && value == 10) &&// XC
                !(prev_value == 500 && value == 100) &&// CD
                !(prev_value == 1000 && value == 100)) {// CM
                fprintf(stderr, "Ошибка в parse_roman: недопустимая комбинация\n");
                return 0;
            }
            result -= value;
        } else {
            result += value;
        }
        
        // Проверка на переполнение
        if (result > MAX_ROMAN_NUMERAL || result < 0) {
            fprintf(stderr, "Ошибка в parse_roman: число вне диапазона\n");
            return 0;
        }
        
        prev_value = value;
    }
    
    // Дополнительная проверка правильности римского числа
    if (result <= 0) {
        fprintf(stderr, "Ошибка в parse_roman: некорректное римское число\n");
        return 0;
    }
    
    return result;
}

// Парсинг цекендорфова представления с валидацией
unsigned int parse_zeckendorf(const char* str) {
    // не забыл
    if (!is_valid_string(str, "parse_zeckendorf")) {
        return 0;
    }
    
    // Проверка формата (должно оканчиваться на '1')
    size_t len = strlen(str);
    if (str[len - 1] != '1') {
        fprintf(stderr, "Ошибка в parse_zeckendorf: строка не оканчивается на '1'\n");
        return 0;
    }
    
    // Проверка допустимых символов (только '0' и '1')
    for (size_t i = 0; i < len; i++) {
        if (str[i] != '0' && str[i] != '1') {
            fprintf(stderr, "Ошибка в parse_zeckendorf: недопустимый символ '%c'\n", str[i]);
            return 0;
        }
    }
    
    // Проверка на наличие двух последовательных '1'
    for (size_t i = 0; i < len - 1; i++) {
        if (str[i] == '1' && str[i + 1] == '1') {
            fprintf(stderr, "Ошибка в parse_zeckendorf: два последовательных '1'\n");
            return 0;
        }
    }
    
    // Генерация чисел Фибоначчи с проверкой переполнения
    unsigned int fib[MAX_FIB_COUNT];
    fib[0] = 1;
    fib[1] = 2;
    int fib_count = 2;
    
    while (fib_count < MAX_FIB_COUNT) {
        // Проверка переполнения
        if (fib[fib_count - 1] > UINT_MAX - fib[fib_count - 2]) {
            fprintf(stderr, "Предупреждение: переполнение чисел Фибоначчи\n");
            break;
        }
        
        unsigned int next = fib[fib_count - 1] + fib[fib_count - 2];
        if (next > UINT_MAX / 2) {
            break; // Прекращаем, если следующее число слишком большое
        }
        
        fib[fib_count] = next;
        fib_count++;
    }
    
    unsigned int result = 0;
    int str_len = (int)len - 1; // Игнорируем последнюю '1'
    int fib_index = str_len - 1;
    
    // Проверка длины строки
    if (str_len > fib_count) {
        fprintf(stderr, "Ошибка в parse_zeckendorf: строка слишком длинная\n");
        return 0;
    }
    
    for (int i = 0; i < str_len; i++) {
        if (str[i] == '1') {
            if (fib_index >= 0 && fib_index < fib_count) {
                // Проверка переполнения
                if (result > UINT_MAX - fib[fib_index]) {
                    fprintf(stderr, "Ошибка в parse_zeckendorf: переполнение результата\n");
                    return 0;
                }
                result += fib[fib_index];
            } else {
                fprintf(stderr, "Ошибка в parse_zeckendorf: индекс Фибоначчи вне диапазона\n");
                return 0;
            }
        }
        fib_index--;
    }
    
    return result;
}

// Парсинг числа из произвольной системы счисления с валидацией
int parse_base(const char* str, int base, int uppercase) {
    // не забыл
    if (!is_valid_string(str, "parse_base")) {
        return 0;
    }
    
    if (!is_valid_base(base)) {
        return 0;
    }
    
    long long result = 0; // Используем long long для проверки переполнения
    int is_negative = 0;
    const char* ptr = str;
    
    // Пропускаем начальные пробелы
    while (*ptr == ' ') ptr++;
    
    if (*ptr == '-') {
        is_negative = 1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }
    
    // Пропускаем ведущие нули
    while (*ptr == '0') ptr++;
    
    // Проверка, что после знака есть цифры
    if (*ptr == '\0') {
        return 0; // Только знак без цифр
    }
    
    int digit_count = 0;
    
    while (*ptr != '\0') {
        char c = *ptr;
        int digit;
        
        if (isdigit((unsigned char)c)) {
            digit = c - '0';
        } else if (isalpha((unsigned char)c)) {
            if (uppercase) {
                digit = toupper((unsigned char)c) - 'A' + 10;
            } else {
                digit = tolower((unsigned char)c) - 'a' + 10;
            }
        } else {
            fprintf(stderr, "Ошибка в parse_base: недопустимый символ '%c'\n", c);
            break;
        }
        
        if (digit >= base) {
            fprintf(stderr, "Ошибка в parse_base: цифра %d недопустима в системе %d\n", digit, base);
            break;
        }
        
        // Проверка переполнения перед умножением
        if (result > (LLONG_MAX - digit) / base) {
            fprintf(stderr, "Ошибка в parse_base: переполнение при преобразовании\n");
            return is_negative ? INT_MIN : INT_MAX;
        }
        
        result = result * base + digit;
        ptr++;
        digit_count++;
        
        // Ограничение количества цифр для безопасности
        if (digit_count > 10) { // Эвристическое ограничение
            fprintf(stderr, "Предупреждение: слишком много цифр в числе\n");
            break;
        }
    }
    
    // Проверка на выход за пределы int
    if (result > INT_MAX) {
        fprintf(stderr, "Предупреждение: число слишком велико для int\n");
        return is_negative ? INT_MIN : INT_MAX;
    }
    
    if (is_negative && result > (long long)INT_MAX + 1) {
        fprintf(stderr, "Предупреждение: число слишком мало для int\n");
        return INT_MIN;
    }
    
    return is_negative ? -(int)result : (int)result;
}

// Основная функция сканирования с валидацией
int overfscanf(FILE* stream, const char* format, ...) {
    // не забыл
    if (!is_valid_file(stream, "overfscanf") || !is_valid_string(format, "overfscanf")) {
        return -1;
    }
    
    va_list args;
    va_start(args, format);
    
    int read_count = 0;
    const char* ptr = format;
    int last_read_success = 1;
    
    while (*ptr != '\0' && last_read_success) {
        if (*ptr != '%') {
            int c = fgetc(stream);
            if (c == EOF) {
                last_read_success = 0;
                if (ferror(stream)) {
                    perror("Ошибка чтения из потока");
                }
                break;
            }
            
            if (c != *ptr) {
                ungetc(c, stream);
                last_read_success = 0;
                break;
            }
            ptr++;
            continue;
        }
        
        ptr++;
        
        // Обработка специальных спецификаторов
        if (*ptr == 'R' && *(ptr + 1) == 'o') {
            int* num_ptr = va_arg(args, int*);
            if (!is_valid_pointer(num_ptr, "overfscanf (Ro)")) {
                va_end(args);
                return -1;
            }
            
            char buffer[MAX_STRING_LENGTH];
            if (fscanf(stream, "%1023s", buffer) == 1) {
                *num_ptr = parse_roman(buffer);
                read_count++;
            } else {
                last_read_success = 0;
                if (feof(stream)) {
                    break;
                }
            }
            ptr += 2;
        }
        else if (*ptr == 'Z' && *(ptr + 1) == 'r') {
            unsigned int* num_ptr = va_arg(args, unsigned int*);
            if (!is_valid_pointer(num_ptr, "overfscanf (Zr)")) {
                va_end(args);
                return -1;
            }
            
            char buffer[MAX_STRING_LENGTH];
            if (fscanf(stream, "%1023s", buffer) == 1) {
                *num_ptr = parse_zeckendorf(buffer);
                read_count++;
            } else {
                last_read_success = 0;
                if (feof(stream)) {
                    break;
                }
            }
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'v') {
            int* num_ptr = va_arg(args, int*);
            if (!is_valid_pointer(num_ptr, "overfscanf (Cv)")) {
                va_end(args);
                return -1;
            }
            
            int base = va_arg(args, int);
            if (!is_valid_base(base)) {
                base = 10;
            }
            
            char buffer[MAX_STRING_LENGTH];
            if (fscanf(stream, "%1023s", buffer) == 1) {
                *num_ptr = parse_base(buffer, base, 0);
                read_count++;
            } else {
                last_read_success = 0;
                if (feof(stream)) {
                    break;
                }
            }
            ptr += 2;
        }
        else if (*ptr == 'C' && *(ptr + 1) == 'V') {
            int* num_ptr = va_arg(args, int*);
            if (!is_valid_pointer(num_ptr, "overfscanf (CV)")) {
                va_end(args);
                return -1;
            }
            
            int base = va_arg(args, int);
            if (!is_valid_base(base)) {
                base = 10;
            }
            
            char buffer[MAX_STRING_LENGTH];
            if (fscanf(stream, "%1023s", buffer) == 1) {
                *num_ptr = parse_base(buffer, base, 1);
                read_count++;
            } else {
                last_read_success = 0;
                if (feof(stream)) {
                    break;
                }
            }
            ptr += 2;
        }
        else {
            // Стандартные спецификаторы
            char format_spec[3] = { '%', *ptr, '\0' };
            
            switch (*ptr) {
                case 'd': {
                    int* num_ptr = va_arg(args, int*);
                    if (!is_valid_pointer(num_ptr, "overfscanf (d)")) {
                        va_end(args);
                        return -1;
                    }
                    
                    int result = fscanf(stream, format_spec, num_ptr);
                    if (result == 1) {
                        read_count++;
                    } else if (result == EOF) {
                        last_read_success = 0;
                        if (ferror(stream)) {
                            perror("Ошибка чтения int");
                        }
                    } else {
                        last_read_success = 0;
                    }
                    break;
                }
                case 'u': {
                    unsigned int* num_ptr = va_arg(args, unsigned int*);
                    if (!is_valid_pointer(num_ptr, "overfscanf (u)")) {
                        va_end(args);
                        return -1;
                    }
                    
                    int result = fscanf(stream, format_spec, num_ptr);
                    if (result == 1) {
                        read_count++;
                    } else if (result == EOF) {
                        last_read_success = 0;
                        if (ferror(stream)) {
                            perror("Ошибка чтения unsigned int");
                        }
                    } else {
                        last_read_success = 0;
                    }
                    break;
                }
                case 's': {
                    char* str_ptr = va_arg(args, char*);
                    if (!is_valid_pointer(str_ptr, "overfscanf (s)")) {
                        va_end(args);
                        return -1;
                    }
                    
                    int result = fscanf(stream, format_spec, str_ptr);
                    if (result == 1) {
                        read_count++;
                    } else if (result == EOF) {
                        last_read_success = 0;
                        if (ferror(stream)) {
                            perror("Ошибка чтения строки");
                        }
                    } else {
                        last_read_success = 0;
                    }
                    break;
                }
                case 'f': {
                    float* num_ptr = va_arg(args, float*);
                    if (!is_valid_pointer(num_ptr, "overfscanf (f)")) {
                        va_end(args);
                        return -1;
                    }
                    
                    int result = fscanf(stream, format_spec, num_ptr);
                    if (result == 1) {
                        read_count++;
                    } else if (result == EOF) {
                        last_read_success = 0;
                        if (ferror(stream)) {
                            perror("Ошибка чтения float");
                        }
                    } else {
                        last_read_success = 0;
                    }
                    break;
                }
                case 'l':
                    // Обработка %lf, %ld и т.д.
                    if (*(ptr + 1) == 'f') {
                        double* num_ptr = va_arg(args, double*);
                        if (!is_valid_pointer(num_ptr, "overfscanf (lf)")) {
                            va_end(args);
                            return -1;
                        }
                        
                        char long_format[4] = { '%', 'l', 'f', '\0' };
                        int result = fscanf(stream, long_format, num_ptr);
                        if (result == 1) {
                            read_count++;
                        } else if (result == EOF) {
                            last_read_success = 0;
                            if (ferror(stream)) {
                                perror("Ошибка чтения double");
                            }
                        } else {
                            last_read_success = 0;
                        }
                        ptr++; // Пропускаем 'l'
                    } else if (*(ptr + 1) == 'd') {
                        long* num_ptr = va_arg(args, long*);
                        if (!is_valid_pointer(num_ptr, "overfscanf (ld)")) {
                            va_end(args);
                            return -1;
                        }
                        
                        char long_format[4] = { '%', 'l', 'd', '\0' };
                        int result = fscanf(stream, long_format, num_ptr);
                        if (result == 1) {
                            read_count++;
                        } else if (result == EOF) {
                            last_read_success = 0;
                            if (ferror(stream)) {
                                perror("Ошибка чтения long");
                            }
                        } else {
                            last_read_success = 0;
                        }
                        ptr++; // Пропускаем 'l'
                    }
                    break;
                case '%': {
                    // Литеральный процент
                    int c = fgetc(stream);
                    if (c == '%') {
                        // OK
                    } else if (c == EOF) {
                        last_read_success = 0;
                        if (ferror(stream)) {
                            perror("Ошибка чтения символа процента");
                        }
                    } else {
                        ungetc(c, stream);
                        last_read_success = 0;
                    }
                    break;
                }
                default:
                    fprintf(stderr, "Ошибка в overfscanf: неизвестный спецификатор '%%%c'\n", *ptr);
                    va_end(args);
                    return -1;
            }
            ptr++;
        }
    }
    
    va_end(args);
    return read_count;
}

int oversscanf(const char* str, const char* format, ...) {
    // не забыл
    if (!is_valid_string(str, "oversscanf") || !is_valid_string(format, "oversscanf")) {
        return -1;
    }
    
    // Создаем временный файл в памяти
    FILE* temp = tmpfile();
    if (temp == NULL) {
        perror("Ошибка создания временного файла");
        return -1;
    }
    
    // Записываем строку в файл
    if (fputs(str, temp) == EOF) {
        perror("Ошибка записи во временный файл");
        fclose(temp);
        return -1;
    }
    
    rewind(temp);
    
    va_list args;
    va_start(args, format);
    
    int result = overfscanf(temp, format, args);
    
    fclose(temp);
    va_end(args);
    return result;
}

void demo_overfscanf() {
    printf("Демонстрация функций overfscanf и oversscanf\n\n");
    
    FILE* test_file = fopen("test_input.txt", "w");
    if (test_file) {
        fprintf(test_file, "MMXXIII 1001011 FF 1010\n");
        rewind(test_file);
        
        int roman_num, hex_num, bin_num;
        unsigned int zeck_num;
        
        int count = overfscanf(test_file, "%Ro %Zr %Cv %Cv", 
                              &roman_num, &zeck_num, &hex_num, 16, &bin_num, 2);
        
        printf("Прочитано %d значений из файла:\n", count);
        printf("Римские: %d\n", roman_num);
        printf("Цекендорфово: %u\n", zeck_num);
        printf("Hex: %d\n", hex_num);
        printf("Binary: %d\n", bin_num);
        
        fclose(test_file);
        remove("test_input.txt");
    } else {
        printf("Не удалось создать тестовый файл\n");
    }
    
    printf("\n");
    
    const char* test_str = "XLV 1010011 1A 1101";
    int r1, r2, r3;
    unsigned int z1;
    
    int count2 = oversscanf(test_str, "%Ro %Zr %Cv %Cv", 
                           &r1, &z1, &r2, 16, &r3, 2);
    
    printf("oversscanf прочитал %d значений из строки:\n", count2);
    printf("Римские: %d\n", r1);
    printf("Цекендорфово: %u\n", z1);
    printf("Hex: %d\n", r2);
    printf("Binary: %d\n", r3);
    
    printf("\n");
    
    // Демонстрация обработки ошибок
    printf("Тест обработки ошибок:\n");
    
    const char* error_str = "MMMCMXCIX 1A2B3C 99999";
    int err1, err2, err3;
    
    int error_count = oversscanf(error_str, "%Ro %Cv %Cv", 
                                &err1, &err2, 10, &err3, 2);
    
    printf("Чтение с ошибками: прочитано %d значений\n", error_count);
    printf("Римские: %d\n", err1);
    printf("Десятичное: %d\n", err2);
    printf("Двоичное: %d\n", err3);
}

int main() {
    setlocale(LC_ALL, "Russian");
    printf("Демонстрация функций форматированного ввода с валидацией\n");
    printf("\n");
    
    demo_overfscanf();
    
    printf("\n");
    printf("Программа завершена\n");
    
    return 0;
}