#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define BUFFER_SIZE 300

void report_data_summary(int num_stud);
void get_args(char *student_name_grades, char **argv);

int main(int argc, char *argv[]) {
    int students = 0;
    FILE *grade_file;
    FILE *output_file;
    char line[100];
    char output_file_name[100];
    int pipefd[2];
    char *args[100];
    char buffer[300];

    sprintf(output_file_name, "%d", getpid());
    strcat(output_file_name, ".temp");


    output_file = fopen(output_file_name, "w");

    if (pipe(pipefd) == -1) { // Create a pipe for communication
        perror("pipe");
        exit(EXIT_FAILURE);
    }


    grade_file = fopen(argv[1], "r");//open input file

    if (grade_file == NULL) {
        printf("Error: could not open file\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), grade_file) != NULL) { // read lines from the file until NULL is returned
        get_args(line, args);
        pid_t pid = fork();

        if (pid == -1)
            printf("error");

        if (pid == 0) {
            dup2(pipefd[1], STDOUT_FILENO);//redirect child prints to pipe
            //char *args[] = {"./one_student", "Avi", "80", "90", "75", NULL};
            int ret = execv("./one_student", args);
            if (ret == -1) {
                perror("execv failed");
            }
        }
        else{
            ssize_t bytes_read = read(pipefd[0], buffer, BUFFER_SIZE);//blocking until child finish
            buffer[bytes_read] = '\0';
            fprintf(output_file, "%s\n", buffer);
            students++;
        }
    }

    fclose(grade_file);
    fclose(output_file);

    report_data_summary(students);

    return 0;
}

void get_args(char *student_name_grades, char **argv) {
    int argc = 0;
    argv[argc] = "./one_student";

    char *token = strtok(student_name_grades, " ");
    while (token != NULL && argc < 99) {
        argc++;
        argv[argc] = token;
        token = strtok(NULL, " ");
    }
    argv[argc+1] = NULL; // Set last argument to NULL for use with execvp
}



void report_data_summary(int num_stud) {
    printf("process: %d: Completed grade calculation for %d students.\n",
           getpid(), num_stud);
}
