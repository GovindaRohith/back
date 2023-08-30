/*
Code by
Govinda Rohith Y
CS21BTECH11062
Operating Systems–2(CS3523)
Programming Assignment 2: Validating Sudoku Solution
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include <sys/time.h>
int n, k, smalln, eff_k; // n value here is 'N'(Refer "Assgn2Readme-CS21BTECH11062" file)
// smalln=square root of N
int **sudoku; // array to store sudoku
int *eval;    // array in which each thread stores its work done
int defno_points, excepno_points;
// defno_points is number of works done by each thread except last thread
// excepno_points is number of works
bool row_eval(int row)
{
    // function to evaluate the each row in sudoku
    // returns true if given row is valid else returns false
    int i;
    bool *arr;
    arr = (bool *)calloc(sizeof(bool), n);
    for (i = 0; i < n; i++)
    {
        if (sudoku[row][i] - 1 < 0 || sudoku[row][i] - 1 >= n)
            return false;
        if (arr[sudoku[row][i] - 1] == true)
        {
            free(arr);
            return false;
        }
        else
            arr[sudoku[row][i] - 1] = true;
    }
    free(arr);
    return true;
}
bool col_eval(int col)
{
    // function to evaluate the each column in sudoku
    // returns true if given column is valid else returns false
    int i;
    bool *arr;
    arr = (bool *)calloc(sizeof(bool), n);
    for (i = 0; i < n; i++)
    {
        if (sudoku[i][col] <= 0 || sudoku[i][col] > n)
            return false;
        if (arr[sudoku[i][col] - 1] == true)
        {
            free(arr);
            return false;
        }
        else
            arr[sudoku[i][col] - 1] = true;
    }
    free(arr);
    return true;
}
bool grid_eval(int grid)
{
    // function to evaluate the each grid in sudoku
    // returns true if given grid is valid else returns false
    int row, col; // derive row and col from grid no
    row = (grid / smalln) * smalln;
    col = (grid % smalln) * smalln;
    int i, j;
    bool *arr;
    arr = (bool *)calloc(sizeof(bool), n);
    for (i = row; i < row + smalln; i++)
    {
        for (j = col; j < col + smalln; j++)
        {
            if (sudoku[i][j] <= 0 || sudoku[i][j] > n)
                return false;
            if (arr[sudoku[i][j] - 1] == true)
            {
                free(arr);
                return false;
            }
            else
                arr[sudoku[i][j] - 1] = true;
        }
    }
    free(arr);
    return true;
}
void *sudo_eval(void *arg)
{
    // function to divide the tasks to each thread accordingly
    bool temp;
    int t_no = *((int *)arg); // get thread number
    free(arg);
    int start = 1 + (t_no - 1) * defno_points, end, i;
    if (t_no == k)
        end = 3 * n;
    else
        end = start + defno_points - 1;
    for (i = start; i <= end; i++)
    {
        if ((double)i / n > 2)
            temp = grid_eval((i - 1) % n);
        else if ((double)i / n > 1)
            temp = col_eval((i - 1) % n);
        else
            temp = row_eval((i - 1) % n);
        if (temp)
            eval[i] = i;
        else
            eval[i] = -i;
    }
    return NULL;
}
void file_printer(unsigned long time)
{
    // function to print output file
    int i;
    FILE *fp1;
    fp1 = fopen("output.txt", "w");
    int j;
    char namer[50] = "row";
    char validater[20] = "Column";
    bool is_valid_su = true;
    for (i = 1; i <= k; i++)
    {
        // format to print
        //  Thread 1 checks row 1 and is valid.
        //  Thread 2 checks column 2 and is valid.
        //  Thread 3 checks grid 1 and is valid.
        int start = 1 + (i - 1) * defno_points, end, temp;
        if (i == k)
            end = 3 * n;
        else
            end = start + defno_points - 1;
        for (j = start; j <= end; j++)
        {
            if ((double)j / n > 2)
                strcpy(namer, "checks grid");
            else if ((double)j / n > 1)
                strcpy(namer, "checks column");
            else
                strcpy(namer, "checks row");
            if (eval[j] > 0)
                strcpy(validater, "and is valid.\n");
            else
            {
                is_valid_su = false;
                strcpy(validater, "and is invalid.\n");
            }
            temp = j % n;
            if (temp == 0)
                temp = n;
            fprintf(fp1, "Thread %d %s %d %s", i, namer, temp, validater);
        }
    }
    if (is_valid_su)
        fprintf(fp1, "Sudoku is valid.\n");
    else
        fprintf(fp1, "Sudoku is invalid.\n");
    fprintf(fp1, "The total time taken is %lu microseconds\n", time);
    fclose(fp1);
}
bool p_approach()
{
    // function to link all functions to evaluate the sudoku
    int i;
    eval = (int *)calloc(sizeof(int), (3 * n + 1));
    pthread_t tid = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t *pid = (pthread_t *)calloc((sizeof(pthread_t)), eff_k);
    struct timeval now1;
    uint64_t timestamp1;
    struct timeval now2;
    uint64_t timestamp2;
    gettimeofday(&now1, NULL); // timer starts now
    for (i = 1; i <= eff_k; i++)
    {
        int *arg = (int *)malloc(sizeof(*arg));
        *arg = i;
        if (pthread_create(&tid, &attr, sudo_eval, arg) != 0)
        {
            printf("Error occured in creating the thread \n");
            return false;
        }
        pid[i - 1] = tid;
    }
    for (i = 1; i <= eff_k; i++)
        pthread_join(pid[i - 1], NULL);
    gettimeofday(&now2, NULL); // timer ends now
    timestamp1 = (uint64_t)now1.tv_sec * 1000000 + (uint64_t)now1.tv_usec;
    timestamp2 = (uint64_t)now2.tv_sec * 1000000 + (uint64_t)now2.tv_usec;
    file_printer(timestamp2 - timestamp1);
    // printf("%lu\n",timestamp2-timestamp1);
    free(eval);
    free(pid);
    return true;
}
int main()
{
    FILE *fp;
    fp = fopen("input.txt", "r");
    if (fp == NULL || fscanf(fp, "%d %d", &k, &n) == EOF || k <= 0 || n <= 0)
    {
        printf("Invalid input\n");
        fclose(fp);
        return 0;
    }
    sudoku = (int **)calloc(sizeof(int *), (n));
    int i, j;
    defno_points = floor((double)3 * n / k);
    excepno_points = 3 * n - (k - 1) * defno_points;
    smalln = sqrt(n);
    if (k > 3 * n)
    {
        printf("Number of threads are more than max number of validations can be performed on given sudoku so taking k=3*n\n");
    }
    if (k <= 3 * n)
        eff_k = k;
    else
        eff_k = 3 * n;
    for (i = 0; i < n; i++)
    {
        sudoku[i] = (int *)calloc(sizeof(int), n);
        for (j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &sudoku[i][j]);
        }
    }
    p_approach();
    for (i = 0; i < n; i++)
    {
        free(sudoku[i]);
    }
    free(sudoku);
    fclose(fp);
    return 0;
}