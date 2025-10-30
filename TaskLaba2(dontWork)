#include <stdlib.h>
#include <ctype.h>
#include<locale.h>

// Вспомогательные функции для работы со строками
size_t my_strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* my_strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return original_dest;
}

// Функции для флагов
char* process_flag_l(const char* str) {
    size_t len = my_strlen(str);
    char* result = malloc(20); // Достаточно для числа
    if (result != NULL) {
        snprintf(result, 20, "%zu", len);
    }
    return result;
}

char* process_flag_r(const char* str) {
    size_t len = my_strlen(str);
    char* result = malloc(len + 1);
    if (result != NULL) {
        for (size_t i = 0; i < len; i++) {
            result[i] = str[len - 1 - i];
        }
        result[len] = '\0';
    }
    return result;
}

char* process_flag_u(const char* str) {
    size_t len = my_strlen(str);
    char* result = malloc(len + 1);
    if (result != NULL) {
        for (size_t i = 0; i < len; i++) {
            if (i % 2 == 1) {
                result[i] = toupper(str[i]);
            }
            else {
                result[i] = str[i];
            }
        }
        result[len] = '\0';
    }
    return result;
}

char* process_flag_n(const char* str) {
    size_t len = my_strlen(str);
    char* result = malloc(len + 1);
    if (result == NULL) return NULL;

    size_t idx = 0;

    // Цифры
    for (size_t i = 0; i < len; i++) {
        if (isdigit(str[i])) {
            result[idx++] = str[i];
        }
    }

    // Буквы
    for (size_t i = 0; i < len; i++) {
        if (isalpha(str[i])) {
            result[idx++] = str[i];
        }
    }

    // Остальные символы
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(str[i]) && !isalpha(str[i])) {
            result[idx++] = str[i];
        }
    }

    result[idx] = '\0';
    return result;
}

char* process_flag_c(unsigned int seed, int count, char** strings) {
    if (count < 2) return NULL;

    // Подсчет общей длины
    size_t total_len = 0;
    for (int i = 1; i < count; i++) {
        total_len += my_strlen(strings[i]);
    }

    char* result = malloc(total_len + 1);
    if (result == NULL) return NULL;

    // Создание массива индексов и перемешивание
    int* indices = malloc((count - 1) * sizeof(int));
    if (indices == NULL) {
        free(result);
        return NULL;
    }

    for (int i = 0; i < count - 1; i++) {
        indices[i] = i + 1;
    }

    srand(seed);
    // Простой алгоритм Фишера-Йетса
    for (int i = count - 2; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    // Конкатенация в случайном порядке
    size_t pos = 0;
    for (int i = 0; i < count - 1; i++) {
        char* current = strings[indices[i]];
        size_t current_len = my_strlen(current);
        my_strcpy(result + pos, current);
        pos += current_len;
    }

    free(indices);
    return result;
}

void main() {
    setlocale(LC_ALL, "Rus");
    printf("=== Задание 2: Обработка строк ===\n");

    // Демонстрация без аргументов командной строки
    const char* test_str = "Hello123 World!";

    char* result;

    // Флаг -l
    result = process_flag_l(test_str);
    printf("Флаг -l (длина): %s\n", result);
    free(result);

    // Флаг -r
    result = process_flag_r(test_str);
    printf("Флаг -r (реверс): %s\n", result);
    free(result);

    // Флаг -u
    result = process_flag_u(test_str);
    printf("Флаг -u (верхний регистр): %s\n", result);
    free(result);

    // Флаг -n
    result = process_flag_n(test_str);
    printf("Флаг -n (сортировка): %s\n", result);
    free(result);

    // Флаг -c
    char* test_strings[] = { "prog", "Hello", "World", "Test", "String" };
    result = process_flag_c(123, 5, test_strings);
    printf("Флаг -c (конкатенация): %s\n", result);
    free(result);

    printf("\n");
}
