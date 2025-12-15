 ;
        }
    }
    result[pos] = '\0';

    free(indices);
    return result;
}

int main() {
    setlocale(LC_ALL, "Russian");
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
    return 0;
}
