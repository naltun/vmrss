#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to search through /proc/PID/status and print VmRSS data
void print_vmrss_info(int pid) {
    FILE *fp;
    char line[1024];
    char *token;
    char path[1024];
    int i = 0;
    int vmrss = 0;
    sprintf(path, "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            token = strtok(line, " ");
            while (token != NULL) {
                if (i == 1) {
                    vmrss = atoi(token);
                    break;
                }
                token = strtok(NULL, " ");
                i++;
            }
            break;
        }
    }
    fclose(fp);
    printf("VmRSS: %d\n", vmrss);
}

int main(int argc, char **argv) {
    int pid;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    pid = atoi(argv[1]);
    print_vmrss_info(pid);

    return EXIT_SUCCESS;
}
