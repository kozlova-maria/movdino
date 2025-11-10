#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 80
#define MAX_WORD_LEN 20
#define MAX_LINE_LENGTH 1024
#define MAX_FILENAME 100

int pars(char* inp_str, char str[][MAX_WORD_LEN])
{
    int idx = 0;
    char c;
    int i = 0, j = 0;
    int in_word = 0;
    int is_first_word = 1;
    
    while ((c = inp_str[idx]) != '\n' && c != '\0')
    {
        if (c == ' ')
        {
            if (is_first_word && !in_word)
            {
                if (j < MAX_WORD_LEN - 1) str[i][j++] = c;
            }
            else if (in_word)
            {
                str[i][j] = '\0'; i++; j = 0; in_word = 0; is_first_word = 0;
            }
        }
        else
        {
            if (i < MAX_WORDS && j < MAX_WORD_LEN - 1)
            {
                str[i][j++] = c; in_word = 1;
            }
        }
        idx++;
    }
    if (in_word && j > 0) str[i][j] = '\0';
    return i + (j > 0 ? 1 : 0);
}

int check(char ***matrix, int height, int width, int x, int y)
{
    if (x < 0 || x >= width )
    {
        return -1;
    }
    if (y < 0 || y >= height)
    {
        return 1;
    }
    if (matrix[x][y][0] == '_')
    {
        return 0;
    }
    if (matrix[x][y][0] == '^')
    {
        return 2;
    }
    if (matrix[x][y][0] == '%')
    {
        return 3;
    }
    if (matrix[x][y][0] == '&')
    {
        return 4;
    }
    if (matrix[x][y][0] == '@')
    {
        return 5;
    } else return -2;
}

void go(char ***matrix, int *x, int *y, int height, int width, int i, int j, int n, FILE *output_file)
{
    matrix[*x][*y][0] = '_';
    
    for (int k = 1; k <= n; k++)
    {
        int new_x = *x + i;
        int new_y = *y + j;
        
        // Обработка границ поля (зацикливание)
        if (new_x < 0) new_x = width - 1;
        else if (new_x >= width) new_x = 0;
        
        if (new_y < 0) new_y = height - 1;
        else if (new_y >= height) new_y = 0;
        
        int cell_status = check(matrix, height, width, new_x, new_y);
        
        // Проверяем препятствия на каждом шаге
        if (cell_status == 2 || cell_status == 4 || cell_status == 5)
        {
            fprintf(output_file, "предупреждение: невозможно переместиться (препятствие)\n");
            break;
        }
        
        // Перемещаемся на клетку
        *x = new_x;
        *y = new_y;
        
        // Проверяем яму ТОЛЬКО на последнем шаге прыжка
        if (k == n && cell_status == 3)
        {
            fprintf(output_file, "проигрыш: попали в яму на последнем шаге прыжка!\n");
            matrix[*x][*y][0] = '_';
            exit(1);
        }
    }
    
    matrix[*x][*y][0] = '#';
}

void grow(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && 
        check(matrix, height, width, new_x, new_y) == 0)
    {
        matrix[new_x][new_y][0] = '&';
    }
    else
    {
        fprintf(output_file, "здесь нельзя вырастить дерево\n");
    }
}

void cut(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && 
        check(matrix, height, width, new_x, new_y) == 4)
    {
        matrix[new_x][new_y][0] = '_';
    }
    else
    {
        fprintf(output_file, "здесь нет дерева\n");
    }
}

void make(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && 
        check(matrix, height, width, new_x, new_y) == 0)
    {
        matrix[new_x][new_y][0] = '@';
    }
    else
    {
        fprintf(output_file, "здесь нельзя поставить камень\n");
    }
}

void push(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    int push_x = x + 2*i;
    int push_y = y + 2*j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && 
        check(matrix, height, width, new_x, new_y) == 5)
    {
        if (push_x >= 0 && push_x < width && push_y >= 0 && push_y < height && 
            check(matrix, height, width, push_x, push_y) == 0)
        {
            matrix[new_x][new_y][0] = '_';
            matrix[push_x][push_y][0] = '@';
        }
        else if (push_x >= 0 && push_x < width && push_y >= 0 && push_y < height && 
                 check(matrix, height, width, push_x, push_y) == 3)
        {
            matrix[new_x][new_y][0] = '_';
            matrix[push_x][push_y][0] = '_';
        }
        else
        {
            fprintf(output_file, "камень толкнуть нельзя\n");
        }
    }
    else
    {
        fprintf(output_file, "здесь нет камня\n");
    }
}

void dig(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && 
        check(matrix, height, width, new_x, new_y) == 0)
    {
        matrix[new_x][new_y][0] = '%';
    }
    else
    {
        fprintf(output_file, "здесь нельзя вырыть яму\n");
    }
}

void mound(char ***matrix, int height, int width, int x, int y, int i, int j, FILE *output_file)
{
    int new_x = x + i;
    int new_y = y + j;
    
    if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height)
    {
        int cell_status = check(matrix, height, width, new_x, new_y);
        if (cell_status == 0)
        {
            matrix[new_x][new_y][0] = '^';
        }
        else if (cell_status == 3)
        {
            matrix[new_x][new_y][0] = '_';
        }
        else
        {
            fprintf(output_file, "здесь нельзя поставить гору\n");
        }
    }
    else
    {
        fprintf(output_file, "здесь нельзя поставить гору\n");
    }
}

// PAINT красит клетку под динозавром, без направления
void paint(char ***matrix, int width, int height, int x, int y, char a, FILE *output_file)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        matrix[x][y][1] = a;
    }
}

void print(char*** matrix, int width, int height, int depth) {
    // Выводим слой 0 (типы клеток)
    printf("   ");
    for (int x = 0; x < width; x++) {
        printf("%2d  ", x);
    }
    printf("\n");
    
    for (int y = 0; y < height; y++) {
        printf("%2d: ", y);
        for (int x = 0; x < width; x++) {
            printf(" %c  ", matrix[x][y][0]);
        }
        printf("\n");
    }
    
    // Выводим слой 1 (цвета)
    printf("\n   ");
    for (int x = 0; x < width; x++) {
        printf("%2d  ", x);
    }
    printf("\n");
    
    for (int y = 0; y < height; y++) {
        printf("%2d: ", y);
        for (int x = 0; x < width; x++) {
            printf(" %c  ", matrix[x][y][1]);
        }
        printf("\n");
    }
    printf("\n");
}

void write_field_to_file(FILE* file, char*** matrix, int width, int height) {
    // Записываем слой 0 (типы клеток) в файл
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            fprintf(file, " %c", matrix[x][y][0]);
        }
        fprintf(file, "\n");
    }
    
    // Записываем слой 1 (цвета) в файл
    fprintf(file, "\n");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            fprintf(file, " %c", matrix[x][y][1]);
        }
        fprintf(file, "\n");
    }
}

int main(int argc, char *argv[]) {
    int width = 0, height = 0, depth = 2;
    int x = 0, y = 0;
    
    // Проверка аргументов командной строки
    if (argc < 2) {
        printf("Использование: %s input.txt\n", argv[0]);
        printf("Будет создан файл output.txt с результатами\n");
        return 1;
    }
    
    char input_filename[MAX_FILENAME];
    char output_filename[] = "output.txt";
    
    strncpy(input_filename, argv[1], MAX_FILENAME - 1);
    input_filename[MAX_FILENAME - 1] = '\0';
    
    printf("Входной файл: %s\n", input_filename);
    printf("Выходной файл: %s\n", output_filename);
    
    // Открытие входного файла
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s'\n", input_filename);
        return 1;
    }
    
    // Создание выходного файла
    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Ошибка: не удалось создать файл '%s'\n", output_filename);
        fclose(input_file);
        return 1;
    }
    
    // Чтение SIZE и START из файла
    char line[MAX_LINE_LENGTH];
    int size_found = 0;
    int start_found = 0;
    int line_number = 0;
    
    while (fgets(line, sizeof(line), input_file) != NULL) {
        line_number++;
        line[strcspn(line, "\n")] = '\0';
        char str[MAX_WORDS][MAX_WORD_LEN] = {0};
        int wordcount = pars(line, str);
        
        // Пропуск комментариев
        if (wordcount > 0 && str[0][0] == '/' && str[0][1] == '/') {
            continue;
        }
        
        // Обработка SIZE
        if (!size_found && wordcount == 3 && strcmp(str[0], "SIZE") == 0 && 
            isdigit(str[1][0]) && isdigit(str[2][0]) &&
            atoi(str[1]) >= 10 && atoi(str[1]) <= 100 && 
            atoi(str[2]) >= 10 && atoi(str[2]) <= 100) {
            
            width = atoi(str[1]);
            height = atoi(str[2]);
            size_found = 1;
            continue;
        }
        
        // Обработка START
        if (size_found && !start_found && wordcount == 3 && strcmp(str[0], "START") == 0 && 
            isdigit(str[1][0]) && isdigit(str[2][0])) {
            
            int start_x = atoi(str[1]);
            int start_y = atoi(str[2]);
            
            if (start_x >= 0 && start_x < width && start_y >= 0 && start_y < height) {
                x = start_x;
                y = start_y;
                start_found = 1;
            } else {
                fprintf(output_file, "Ошибка: стартовые координаты вне диапазона поля\n");
                fclose(input_file);
                fclose(output_file);
                return 1;
                
            }
            continue;
        }
    }
    
    if (!size_found || !start_found) {
        fprintf(output_file, "Ошибка: не найдены корректные команды SIZE и START\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }
    
    // Выделение памяти для матрицы
    char ***matrix = (char***)malloc(width * sizeof(char**));
    if (matrix == NULL) {
        printf("Ошибка выделения памяти\n");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }
    
    for (int i = 0; i < width; i++) {
        matrix[i] = (char**)malloc(height * sizeof(char*));
        if (matrix[i] == NULL) {
            printf("Ошибка выделения памяти\n");
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            fclose(input_file);
            fclose(output_file);
            return 1;
        }
        
        for (int j = 0; j < height; j++) {
            matrix[i][j] = (char*)malloc(depth * sizeof(char));
            if (matrix[i][j] == NULL) {
                printf("Ошибка выделения памяти\n");
                for (int k = 0; k < j; k++) {
                    free(matrix[i][k]);
                }
                free(matrix[i]);
                for (int k = 0; k < i; k++) {
                    for (int l = 0; l < height; l++) {
                        free(matrix[k][l]);
                    }
                    free(matrix[k]);
                }
                free(matrix);
                fclose(input_file);
                fclose(output_file);
                return 1;
            }
        }
    }
    
    // Инициализация массива
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            matrix[i][j][0] = '_';
            matrix[i][j][1] = '_';
        }
    }
    matrix[x][y][0] = '#';
    
    // Перемотка файла и выполнение команд
    rewind(input_file);
    line_number = 0;
    
    // Выполнение команд
    while (fgets(line, sizeof(line), input_file) != NULL) {
        line_number++;
        line[strcspn(line, "\n")] = '\0';
        
        char str[MAX_WORDS][MAX_WORD_LEN] = {0};
        int wordcount = pars(line, str);
        
        // Пропуск комментариев и уже обработанных команд
        if (wordcount > 0 && str[0][0] == '/' && str[0][1] == '/') {
            continue;
        }
        if ((wordcount == 3 && strcmp(str[0], "SIZE") == 0) || 
            (wordcount == 3 && strcmp(str[0], "START") == 0)) {
            continue;
        }
        if (wordcount == 0) {
            continue;
        }
        
        printf("Выполнение команды: %s\n", line);
        
        // Проверяем команду
        int i = 0, j = 0;
        int direction_used = 0;
        int command_valid = 0;

        // Определяем направление (для всех команд кроме PAINT)
        if (wordcount >= 2 && strcmp(str[0], "PAINT") != 0) {
            if (strcmp(str[1], "UP") == 0) {
                i = 0; j = -1; direction_used = 1;
            } else if (strcmp(str[1], "DOWN") == 0) {
                i = 0; j = 1; direction_used = 1;
            } else if (strcmp(str[1], "RIGHT") == 0) {
                i = 1; j = 0; direction_used = 1;
            } else if (strcmp(str[1], "LEFT") == 0) {
                i = -1; j = 0; direction_used = 1;
            }
        }

        // Проверяем известные команды
        if (wordcount == 2 && strcmp(str[0], "MOVE") == 0 && direction_used) {
            go(matrix, &x, &y, height, width, i, j, 1, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "DIG") == 0 && direction_used) {
            dig(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "MOUND") == 0 && direction_used) {
            mound(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "GROW") == 0 && direction_used) {
            grow(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "CUT") == 0 && direction_used) {
            cut(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "MAKE") == 0 && direction_used) {
            make(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "PUSH") == 0 && direction_used) {
            push(matrix, height, width, x, y, i, j, output_file);
            command_valid = 1;
        } else if (wordcount == 3 && strcmp(str[0], "JUMP") == 0 && direction_used && isdigit(str[2][0])) {
            go(matrix, &x, &y, height, width, i, j, atoi(str[2]), output_file);
            command_valid = 1;
        } else if (wordcount == 2 && strcmp(str[0], "PAINT") == 0 && islower(str[1][0])) {
            paint(matrix, width, height, x, y, str[1][0], output_file);
            command_valid = 1;
        }
        
        // Немедленное завершение при неизвестной команде
        if (!command_valid) {
            fprintf(output_file, "Ошибка\n");
            
            // Немедленное завершение программы
            fclose(input_file);
            fclose(output_file);
            
            for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                    free(matrix[i][j]);
                }
                free(matrix[i]);
            }
            free(matrix);
            
            return 1;
        }
        
        // Вывод состояния после каждой команды в консоль
        print(matrix, width, height, depth);
        
        // Запись в выходной файл
        write_field_to_file(output_file, matrix, width, height);
    }
    
    // Закрытие файлов и освобождение памяти
    fclose(input_file);
    fclose(output_file);
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            free(matrix[i][j]);
        }
        free(matrix[i]);
    }
    free(matrix);
    
    printf("Программа завершена. Результаты записаны в файл '%s'\n", output_filename);
    
    return 0;
}