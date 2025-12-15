#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>

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

char* process_flag_l(const char* str) {
    size_t len = my_strlen(str);
    char* result = (char*)malloc(20);
    if (result != NULL) {
        snprintf(result, 20, "%zu", len);
    }
    return result;
}

char* process_flag_r(const char* str) {
    size_t len = my_strlen(str);
    char* result = (char*)malloc(len + 1);
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
    char* result = (char*)malloc(len + 1);
    if (result != NULL) {
        for (size_t i = 0; i < len; i++) {
            if (i % 2 == 1) {
                result[i] = toupper((unsigned char)str[i]);
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
    char* result = (char*)malloc(len + 1);
    if (result == NULL) return NULL;

    size_t idx = 0;

    for (size_t i = 0; i < len; i++) {
        if (isdigit((unsigned char)str[i])) {
            result[idx++] = str[i];
        }
    }

    for (size_t i = 0; i < len; i++) {
        if (isalpha((unsigned char)str[i])) {
            result[idx++] = str[i];
        }
    }

    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)str[i]) && !isalpha((unsigned char)str[i])) {
            result[idx++] = str[i];
        }
    }

    result[idx] = '\0';
    return result;
}

char* process_flag_c(unsigned int seed, int count, char** strings) {
    if (count < 2) return NULL;

    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        if (strings[i] != NULL) {
            total_len += my_strlen(strings[i]);
        }
    }

    char* result = (char*)malloc(total_len + 1);
    if (result == NULL) return NULL;

    int* indices = (int*)malloc(count * sizeof(int));
    if (indices == NULL) {
        free(result);
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        indices[i] = i;
    }

    srand(seed);
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    size_t pos = 0;
    for (int i = 0; i < count; i++) {
        char* current = strings[indices[i]];
        if (current != NULL) {
            size_t current_len = my_strlen(current);
            my_strcpy(result + pos, current);
            pos += current_len;
        }
    }
    result[pos] = '\0';

    free(indices);
    return result;
}

int main() {
    setlocale(LC_ALL, "Russian");
    printf("=== Задание 2: Обработка строк ===\n");

    const char* test_str = "Hello123 World!";
    char* result;

    printf("\n1. Тестирование флага -l (длина строки):\n");
    result = process_flag_l(test_str);
    if (result != NULL) {
        printf("Успех: Длина строки \"%s\" = %s\n", test_str, result);
        free(result);
    } else {
        printf("Ошибка: Не удалось выделить память для флага -l\n");
    }

    printf("\n2. Тестирование флага -r (реверс строки):\n");
    result = process_flag_r(test_str);
    if (result != NULL) {
        printf("Успех: \"%s\" -> \"%s\"\n", test_str, result);
        free(result);
    } else {
        printf("Ошибка: Не удалось выделить память для флага -r\n");
    }

    printf("\n3. Тестирование флага -u (верхний регистр):\n");
    result = process_flag_u(test_str);
    if (result != NULL) {
        printf("Успех: \"%s\" -> \"%s\"\n", test_str, result);
        free(result);
    } else {
        printf("Ошибка: Не удалось выделить память для флага -u\n");
    }

    printf("\n4. Тестирование флага -n (сортировка символов):\n");
    result = process_flag_n(test_str);
    if (result != NULL) {
        printf("   Успех: \"%s\" -> \"%s\"\n", test_str, result);
        free(result);
    } else {
        printf("Ошибка: Не удалось выделить память для флага -n\n");
    }

    printf("\n5. Тестирование флага -c (конкатенация):\n");
    char* test_strings[] = { "prog", "Hello", "World", "Test", "String" };
    result = process_flag_c(123, 5, test_strings);
    if (result != NULL) {
        printf("Успех: Конкатенация 5 строк = \"%s\"\n", result);
        printf("Исходные строки: ");
        for (int i = 0; i < 5; i++) {
            printf("\"%s\"", test_strings[i]);
            if (i < 4) printf(", ");
        }
        printf("\n");
        free(result);
    } else {
        printf("Ошибка: Не удалось выполнить конкатенацию для флага -c\n");
    }

    printf("\nТестирование ошибок L,R,C(проверка обработки NULL):\n");
    
    printf("   а) Проверка process_flag_l с NULL:\n");
    result = process_flag_l(NULL);
    if (result == NULL) {
        printf("Ожидаемо: NULL вход -> NULL выход\n");
    } else {
        printf("Неожиданно: Получен результат для NULL\n");
        free(result);
    }
    
    printf("   б) Проверка process_flag_r с пустой строкой:\n");
    result = process_flag_r("");
    if (result != NULL) {
        printf("Успех: Пустая строка обработана, результат = \"%s\"\n", result);
        free(result);
    } else {
        printf("Ошибка: Не удалось обработать пустую строку\n");
    }
    
    printf("   в) Проверка process_flag_c с недостаточным количеством строк:\n");
    result = process_flag_c(123, 1, test_strings);
    if (result == NULL) {
        printf("Ожидаемо: count < 2 -> NULL выход\n");
    } else {
        printf("Неожиданно: Получен результат при count < 2\n");
        free(result);
    }
    
    printf("   г) Проверка process_flag_c с NULL в массиве строк:\n");
    char* test_strings_with_null[] = { "Hello", NULL, "World" };
    result = process_flag_c(123, 3, test_strings_with_null);
    if (result != NULL) {
        printf("Успех: Обработано с NULL элементами = \"%s\"\n", result);
        free(result);
    } else {
        printf("Ошибка: Не удалось обработать массив с NULL\n");
    }

    printf("\nТестирование с разными входными данными:\n");
    
    printf("   а) Строка только из цифр:\n");
    const char* digits_str = "1234567890";
    result = process_flag_n(digits_str);
    if (result != NULL) {
        printf("Успех: \"%s\" -> \"%s\"\n", digits_str, result);
        free(result);
    }
    
    printf("   б) Строка только из букв:\n");
    const char* letters_str = "HelloWorld";
    result = process_flag_u(letters_str);
    if (result != NULL) {
        printf("Успех: \"%s\" -> \"%s\"\n", letters_str, result);
        free(result);
    }
    
    printf("   в) Строка с специальными символами:\n");
    const char* special_str = "!@#$%^&*()";
    result = process_flag_r(special_str);
    if (result != NULL) {
        printf("Успех: \"%s\" -> \"%s\"\n", special_str, result);
        free(result);
    }

    printf("\n=== Все тесты завершены ===\n");
    return 0;
}