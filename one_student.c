#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_GRADES 10

double calculate_average(int *grades, int num_grades);

int get_grades(int argc, char *const *argv, int *grades, int num_grades);

int main(int argc, char *argv[]) {
    char *student_name;
    int grades[MAX_GRADES];
    int num_grades = 0;
    double average;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s student_name grades\n", argv[0]);
        exit(1);
    }

    student_name = argv[1];
    num_grades = get_grades(argc, argv, grades, num_grades);

    average = calculate_average(grades, num_grades);


    printf("%s %.1f", student_name, average);

    return 0;
}

int get_grades(int argc, char *const *argv, int *grades, int num_grades) {
    for (int i = 2; i < argc; ++i) {
        char *grades_str = argv[i];

        // split the grades string into individual grades
        char *token = strtok(grades_str, " ");
        while (token != NULL) {
            int grade = atoi(token);
            grades[num_grades] = grade;
            num_grades++;
            token = strtok(NULL, " ");
        }
    }
    return num_grades;
}


double calculate_average(int *grades, int num_grades) {
    double sum = 0.0;
    for (int i = 0; i < num_grades; i++) {
        sum += grades[i];
    }
    return sum / num_grades;
}