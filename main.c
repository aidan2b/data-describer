#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>

#define MAX_LINES 27500

typedef enum {
    NUMERIC,
    NON_NUMERIC
} ColumnType;

typedef struct TreeNode {
    char *value;
    int count;
    int height; // New field to store the height of the node in the tree
    struct TreeNode *left, *right;
} TreeNode;

int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(TreeNode *node) {
    return node ? node->height : 0;
}

TreeNode *newNode(char *value) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->value = strdup(value);
    node->count = 1;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

TreeNode *rightRotate(TreeNode *y) {
    TreeNode *x = y->left;
    TreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x; // New root
}

TreeNode *leftRotate(TreeNode *x) {
    TreeNode *y = x->right;
    TreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y; // New root
}

int getBalance(TreeNode *node) {
    return node ? (height(node->left) - height(node->right)) : 0;
}

TreeNode *insert(TreeNode *node, char *value) {
    if (!node)
        return newNode(value);

    if (strcmp(value, node->value) < 0)
        node->left = insert(node->left, value);
    else if (strcmp(value, node->value) > 0)
        node->right = insert(node->right, value);
    else {
        node->count++;
        return node;
    }

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1) {
        if (strcmp(value, node->left->value) < 0)
            return rightRotate(node);
        if (strcmp(value, node->left->value) > 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
    }

    if (balance < -1) {
        if (strcmp(value, node->right->value) > 0)
            return leftRotate(node);
        if (strcmp(value, node->right->value) < 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
    }

    return node;
}

void findMostCommonAndCountUnique(TreeNode *root, char **most_common, int *most_common_count, int *unique_count) {
    if(!root) return;

    (*unique_count)++; // Increment the unique count for each node encountered.

    findMostCommonAndCountUnique(root->left, most_common, most_common_count, unique_count);

    if(root->count > *most_common_count) {
        *most_common_count = root->count;
        *most_common = root->value;
    }

    findMostCommonAndCountUnique(root->right, most_common, most_common_count, unique_count);
}

void freeTree(TreeNode *root) {
    if(!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root->value);
    free(root);
}

char **parse_line(char *str, int *size) {
    char **result = NULL;
    *size = 0;
    bool in_quotes = false;
    char *start = str;

    for(char *ptr = str; *ptr; ptr++) {
        if (*ptr == '\"') in_quotes = !in_quotes;
        if (*ptr == ',' && !in_quotes) {
            *ptr = '\0';
            result = realloc(result, (*size + 1) * sizeof(char *));
            result[*size] = strdup(start);
            (*size)++;
            start = ptr + 1;
        }
    }

    result = realloc(result, (*size + 1) * sizeof(char *));
    result[*size] = strdup(start);
    (*size)++;

    return result;
}

char *read_line(FILE *file) {
    bool in_quotes = false;
    char *line = NULL;
    size_t len = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\"') in_quotes = !in_quotes;
        if (c == '\n' && !in_quotes) break;
        line = realloc(line, len + 2);
        line[len++] = c;
    }

    if(line) line[len] = '\0';
    return line;
}

int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int compareDouble(const void *a, const void *b) {
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;

    if(arg1 < arg2) return -1;
    if(arg1 > arg2) return 1;
    return 0;
}

void free_memory(char ***arr, int *sizes, int lines, int cols) {
    for(int i = 0; i < lines; i++) {
        for(int j = 0; j < sizes[i]; j++) free(arr[i][j]);
        free(arr[i]);
    }
    free(arr);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <filename.csv>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (!file) return perror("Unable to open file"), 1;

    char *line = read_line(file);
    if (!line) return perror("Unable to read line"), fclose(file), 1;

    int column_size;
    char **columns = parse_line(line, &column_size);
    free(line);

    char ***values_array = malloc(MAX_LINES * sizeof(char **));
    int sizes[MAX_LINES];

    double min_values[column_size];
    double max_values[column_size];
    ColumnType column_types[column_size];

    for(int j = 0; j < column_size; j++) {
        min_values[j] = DBL_MAX;
        max_values[j] = DBL_MIN;
        column_types[j] = NUMERIC;
    }

    for(int i = 0; i < MAX_LINES; i++) {
        line = read_line(file);
        if (!line) {
            perror("Unable to read line");
            free_memory(values_array, sizes, i, column_size);
            fclose(file);
            return 1;
        }

        values_array[i] = parse_line(line, &sizes[i]);
        free(line);

        for(int j = 0; j < column_size && j < sizes[i]; j++) {
            char *endptr;
            double value = strtod(values_array[i][j], &endptr);

            if(*endptr != '\0') // Non-numeric column
                column_types[j] = NON_NUMERIC;
            else {
                if(value < min_values[j]) min_values[j] = value;
                if(value > max_values[j]) max_values[j] = value;
            }
        }
    }

    fclose(file);

    for(int j = 0; j < column_size; j++) {
        printf("Column %d (%s):\n", j + 1, columns[j]);

        char **column_values = malloc(MAX_LINES * sizeof(char *));

        for(int i = 0; i < MAX_LINES && j < sizes[i]; i++) {
            column_values[i] = values_array[i][j];
        }

        if(column_types[j] == NUMERIC) {
            double sum = 0, sq_sum = 0, cube_sum = 0, fourth_sum = 0, mode = 0, median, range, iqr, skewness, kurtosis;
            int mode_freq = 0, freq_count[MAX_LINES] = {0}, missing_values_count = 0;
            double *values_sorted = malloc(MAX_LINES * sizeof(double));
            range = max_values[j] - min_values[j];

            for(int i = 0; i < MAX_LINES && j < sizes[i]; i++) {
                if(column_values[i] == NULL) { // Check if the value is missing
                    missing_values_count++;
                    continue; // Skip the current iteration if the value is missing
                }

                double value = strtod(column_values[i], NULL);
                values_sorted[i] = value;  // store the value in the sorted array
                sum += value;
                sq_sum += value * value;
                cube_sum += pow(value, 3);
                fourth_sum += pow(value, 4);

                freq_count[i]++;
                if(freq_count[i] > mode_freq) {
                    mode_freq = freq_count[i];
                    mode = value;
                }
            }

            // Sort the array to find the median
            qsort(values_sorted, MAX_LINES, sizeof(double), compareDouble);
            if(MAX_LINES % 2 == 0)
                median = (values_sorted[MAX_LINES / 2 - 1] + values_sorted[MAX_LINES / 2]) / 2.0;
            else
                median = values_sorted[MAX_LINES / 2];

            double q1 = values_sorted[MAX_LINES / 4]; // First quartile
            double q3 = values_sorted[3 * MAX_LINES / 4]; // Third quartile
            iqr = q3 - q1;  // Interquartile range

            double mean = (MAX_LINES - missing_values_count) > 0 ? sum / (MAX_LINES - missing_values_count) : 0;
            double variance = (sq_sum / MAX_LINES) - (mean * mean);
            double std_dev = sqrt(variance);
            skewness = (MAX_LINES * cube_sum - 3 * mean * sq_sum + 3 * pow(mean, 2) * sum - pow(mean, 3) * MAX_LINES) / pow(std_dev, 3) / MAX_LINES;
            kurtosis = (MAX_LINES * (MAX_LINES + 1) * fourth_sum - 4 * mean * (MAX_LINES - 1) * cube_sum + 6 * pow(mean, 2) * sq_sum - 3 * pow(mean, 4) * MAX_LINES * (MAX_LINES + 1)) / pow(std_dev, 4) / (MAX_LINES * (MAX_LINES - 1)) - 3;

            printf("  Type: Numeric\n");
            printf("  Mean: %.2f\n", mean);
            printf("  Variance: %.2f\n", variance);
            printf("  Standard Deviation: %.2f\n", std_dev);
            printf("  Min: %.2f\n", min_values[j]);
            printf("  Max: %.2f\n", max_values[j]);
            printf("  Median: %.2f\n", median);
            printf("  Mode: %.2f\n", mode);
            printf("  Range: %.2f\n", range);
            printf("  IQR: %.2f\n", iqr);
            printf("  Skewness: %.2f\n", skewness);
            printf("  Kurtosis: %.2f\n", kurtosis);
            printf("  Missing Values: %d\n", missing_values_count); // Print the number of missing values for numeric columns

            free(values_sorted);

        } else {

            TreeNode *root = NULL;
            int unique_count = 0, max_length = 0, min_length = INT_MAX, total_length = 0, missing_values_count = 0;

            for(int i = 0; i < MAX_LINES && j < sizes[i]; i++) {
                // You will need to count unique values, calculate string lengths, and count missing values here
                root = insert(root, values_array[i][j]);
                if(values_array[i][j] == NULL) missing_values_count++;
                else {
                    int len = strlen(values_array[i][j]);
                    max_length = max(max_length, len);
                    min_length = min(min_length, len);
                    total_length += len;
                }
            }

            char *most_common = NULL;
            int most_common_count = 0;

            findMostCommonAndCountUnique(root, &most_common, &most_common_count, &unique_count);

            double avg_length = (double)total_length / (MAX_LINES - missing_values_count);

            printf("  Type: Non-Numeric\n");
            printf("  Most Common Value: %s\n", most_common);
            printf("  Occurrences: %d\n", most_common_count);
            printf("  Unique Values: %d\n", unique_count);
            printf("  Max Length: %d\n", max_length);
            printf("  Min Length: %d\n", min_length);
            printf("  Avg Length: %.2f\n", avg_length);
            printf("  Missing Values: %d\n", missing_values_count);

            freeTree(root);
        }

        free(column_values);
        printf("\n");

    }

    // Make sure to free the rest of the allocated memory
    for(int i = 0; i < column_size; i++) free(columns[i]);
    free(columns);

    free_memory(values_array, sizes, MAX_LINES, column_size);

    return 0;}
