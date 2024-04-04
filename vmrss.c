#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static float get_vmrss(int pid) {
    char statpath[50], line[50], procname[50];
    FILE *fp;
    float mb;
    int kb;

    // Set statpath to /proc/PID/status
    sprintf(statpath, "/proc/%d/status", pid);
    fp = fopen(statpath, "r");

    // Initialize procname to check for an update later
    procname[0] = '\0';

	if (!fp) {
	    fprintf(stderr, "Total: 0 MB");
	    exit(EXIT_FAILURE);
	}

    // Iterate over the lines of /proc/PID/status...
    while (fgets(line, sizeof(line), fp)) {
        // ...to get the PID name
        if (strncmp(line, "Name:", 5) == 0)
            sscanf(line, "%*s %s", procname);
        // ...and the VmRSS (Resident Set Size) value
        else if (strncmp(line, "VmRSS:", 6) == 0)
            sscanf(line, "%*s %d", &kb);
	}

    fclose(fp);

    // Check if procname has been updated and if kb has a positive value
    if (procname[0] != '\0' && kb > 0) {
        // 1 kB = 10 ** -3
        mb = (float)kb * 0.001;
        printf("%s(%d) %.2f MB\n", procname, pid, mb);
    } else {
        fprintf(stderr, "Total: 0 MB\n");
        exit(EXIT_FAILURE);
    }

    return mb;
}

static float print_vmrss(int pid) {
    float total;

    total = get_vmrss(pid);

    return total;
}

int main(int argc, char **argv) {
    // Process ID to work with
    int pid;
    // Total Resident Set Size for parent, child processes
    float total;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert string (e.g. "1530") to int
    pid = atoi(argv[1]);
    // Print VmRSS info from PID and return total memory used in MB
    total = print_vmrss(pid);
    printf("Total: %.2f MB\n", total);

    return EXIT_SUCCESS;
}
