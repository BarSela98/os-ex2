#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int num_of_files = 0;
    char *output_file_str = argv[2];
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

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                num_of_files++;
        }
        num_of_files++;
    }

    return 0;
}
