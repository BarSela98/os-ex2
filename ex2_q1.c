#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <fcntl.h>

void report_data_summary(int num_stud);
int* get_process_numbers(const char* filename, int* num_processes);
void write_avg_grade_to_file(FILE *file, char student_name[10], float avg_grade);

int main(int argc, char *argv[]) {
    int num_of_files = 0;
    char *output_file_str = argv[1];
    char output_file[100];
    int output_fd;

    // get output file name
    char *token = strtok(output_file_str, " ");
    while (token != NULL) {
        strcpy(output_file, token);
        token = strtok(NULL, " ");
    }

    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        fprintf(stderr, "Error: could not open output file %s\n", output_file);
        exit(1);
    }


    for (int i = 2; i < argc; i++) {
        char *filename = argv[i];
        pid_t pid = fork();


        if (pid == -1) {
            fprintf(stderr, "Error: fork failed\n");
            exit(1);
        } else if (pid == 0) {
            dup2(output_fd, STDOUT_FILENO); // redirect stdout to output file
            char *args[] = {"./read_grades", filename, NULL};
            execv(args[0], args);
            fprintf(stderr, "Error: execv failed\n");
            exit(1);

        } else {
            int status;
            waitpid(pid, &status, 0);
        }
        num_of_files++;
    }

    while (wait(NULL) != -1);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int total_students = 0;
    int parent_to_child[2];
    int child_to_parent[2];
    int* pid_arr;
    int num_processes[1];
    char line[100];
    bool first_word = true;
    char student_name[10];

    pid_arr = get_process_numbers(output_file, num_processes);

    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t all_std_fork = fork();

    if (all_std_fork == -1){
        printf("Failed to create a new process\n");
        return (0);
    }

    if (all_std_fork == 0) {//read from temp files in child process
        char file_name[20];
        FILE *all_std_file;

        all_std_file = fopen("all_std.log", "w");
        for (int file_number = 0; file_number < num_of_files; ++file_number) {
            sprintf(file_name, "%d", pid_arr[file_number]);
            strcat(file_name, ".temp");

            FILE *grade_file = fopen(file_name, "r"); // open the file for reading

            if (grade_file == NULL) { // check if the file was opened successfully
                printf("Error opening file\n");
                return (0);
            }

            while (fgets(line, sizeof(line), grade_file) != NULL) { // read lines from the file until NULL is returned
                char *token = strtok(line, " ");
                float avg_grade = 0;
                while (token != NULL) {
                    if (!first_word) {
                        avg_grade = atoi(token);
                    } else {
                        first_word = false;
                        strcpy(student_name, token);
                    }
                    token = strtok(NULL, " ");
                }
                first_word = true;
                total_students++;

                write_avg_grade_to_file(all_std_file, student_name, avg_grade);
            }
            fclose(grade_file);
        }
        fclose(all_std_file);

        write(child_to_parent[1], &total_students, sizeof(total_students));

        close(parent_to_child[0]);
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        close(child_to_parent[1]);
    } else {
        // send the num_of_files to the child
        write(parent_to_child[1], &num_of_files, sizeof(int));

        // send the array to the child
        write(parent_to_child[1], &(pid_arr[1]), 99 * sizeof(int));

        while (wait(NULL) != -1);

        total_students = child_to_parent[0];

        close(parent_to_child[0]);
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        close(child_to_parent[1]);

        report_data_summary(total_students);
    }
    return 0;
}

void write_avg_grade_to_file(FILE *file, char student_name[10], float avg_grade) {
    if (file == NULL) {
        printf("Error: could not open file\n");
        exit(1);
    }

    fprintf(file, "%s ", student_name);
    fprintf(file, "%.1f\n", avg_grade);
}

void report_data_summary(int num_stud) {
    fprintf(stderr, "grade calculation for %d students is done\n", num_stud);
}


int* get_process_numbers(const char* filename, int* num_processes) {
    const int max_processes = 10;
    int* processes = malloc(max_processes * sizeof(int));
    if (processes == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

    // Open the file for reading
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Read the file line by line
    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), fp)) {
        // Try to parse the process number from the line
        int process;
        if (sscanf(line, "process: %d:", &process) == 1) {
            // Add the process number to the array
            processes[i] = process;
            i++;

            // Check if we have reached the maximum number of processes
            if (i == max_processes) {
                break;
            }
        }
    }

    // Close the file
    fclose(fp);

    *num_processes = i;

    return processes;
}
