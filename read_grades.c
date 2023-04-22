#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 300

void report_data_summary(int num_stud);
char **split_string(char *str);

int main(int argc, char *argv[]) {
    int students = 0;
    FILE *fp;
    FILE *fp2;
    char *line = NULL;
    size_t len = 0;
    char output_file_name[100];
    int pipefd[2];
    char buffer[300];

    sprintf(output_file_name, "%d", getpid());
    strcat(output_file_name, ".temp");

    if (pipe(pipefd) == -1) { // Create a pipe for communication
        perror("pipe");
        exit(EXIT_FAILURE);
    }


    fp = fopen(argv[1], "r");//open input file
    fp2 = fopen(output_file_name, "w");//open input file

    if (fp == NULL || fp2 == NULL) {
        printf("Error: could not open file\n");
        exit(1);
    }

    while (getline(&line, &len, fp) != -1) {//read line from file and send each on to one_student.c
        if (!fork()) {
            close(pipefd[0]); // Close the read end of the pipe
            dup2(pipefd[1], STDOUT_FILENO);//redirect child prints to pipe
            execv("./one_student.c", split_string(line));//run one_student.c from child procced
        }
        ssize_t bytes_read = read(pipefd[0], buffer, BUFFER_SIZE);//blocking until child finish
        buffer[bytes_read] = '\0';
        fprintf(fp2, "%s\n", buffer); //write to file
        students++;
    }

    fclose(fp);
    fclose(fp2);

    report_data_summary(students);

    return 0;
}

char **split_string(char *str) {
    char **tokens = malloc(100 * sizeof(char *));
    char *token;
    int i = 1;
    strcpy(tokens[0],"./one_student.c");
    // Split the string into tokens
    token = strtok(str, " ");
    while (token != NULL && i < 100) {
        tokens[i] = malloc(strlen(token) + 1);
        strcpy(tokens[i], token);
        token = strtok(NULL, " ");
        i++;
    }

    // Set the last token to NULL
    tokens[i] = NULL;

    return tokens;
}



void report_data_summary(int num_stud) {
    printf("process: %d: Completed grade calculation for %d students.\n",
           getpid(), num_stud);
}
