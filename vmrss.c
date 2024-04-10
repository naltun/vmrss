/*
 *          --== V m R S S ==--
 *
 * Print parent, child process Resident Set Size (VmRSS)
 *
 * `vmrss' uses the `proc' pseudo-filesystem, which provides an interface to process data structures
 * in the kernel. For more information on procfs, please see:
 *
 *      https://man7.org/linux/man-pages/man5/proc.5.html
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int *find_child_processes(int ppid, int *size) {
    char statpath[269], line[50];
    DIR *dir;
    FILE *fp;
    int *child_pids, capacity , counter, ppid_tmp;
    struct dirent *entry;

    child_pids = NULL;
    capacity = 10;
    counter = 0;

    dir = opendir("/proc");
    if (!dir) {
        *size = 0;
        return NULL; // Correctly handle directory open failure
    }

    child_pids = malloc(capacity * sizeof(int)); // Initial allocation
    if (!child_pids) {
        closedir(dir);
        *size = 0;
        return NULL; // Handle allocation failure
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            sprintf(statpath, "/proc/%s/status", entry->d_name);

            fp = fopen(statpath, "r");
            if (!fp) continue;

            while (fgets(line, sizeof(line), fp)) {
                if (sscanf(line, "PPid: %d", &ppid_tmp) == 1 && ppid_tmp == ppid) {
                    // Check if we need to dynamically allocate more memory
                    if (counter >= capacity) {
                        capacity *= 2;
                        int *new_ptr = realloc(child_pids, capacity * sizeof(int));
                        if (!new_ptr) {
                            free(child_pids);
                            fclose(fp);
                            closedir(dir);
                            *size = 0;
                            return NULL;
                        }
                        child_pids = new_ptr;
                    }

                    child_pids[counter++] = atoi(entry->d_name);
                    break;
                }
            }
            fclose(fp);
        }
    }
    closedir(dir);

    *size = counter;
    return child_pids;
}

static float get_parent_vmrss(int pid) {
    char statpath[269], line[50], procname[50];
    FILE *fp;
    float mb;
    int kb;

    // Set statpath to /proc/PID/status
    sprintf(statpath, "/proc/%d/status", pid);
    fp = fopen(statpath, "r");

    if (!fp) {
        fprintf(stderr, "Total: 0 MB");
        exit(EXIT_FAILURE);
    }

    // Initialize procname to check for an update later
    procname[0] = '\0';

    // Iterate over the lines of /proc/PID/status...
    while (fgets(line, sizeof(line), fp)) {
        // ...to get the PID name
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "%*s %s", procname);
        // ...and the VmRSS (Resident Set Size) value
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            sscanf(line, "%*s %d", &kb);
        } else {
            continue;
        }
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
    char statpath[269], line[50], procname[50];
    FILE *fp;
    float total;
    int *child_pids, kb, size_child_pids;

    // Get PID VmRSS
    total = get_parent_vmrss(pid);

    // Check for child processes
    child_pids = find_child_processes(pid, &size_child_pids);
    if (child_pids) {
        procname[0] = '\0';

        for (int i = 0; i < size_child_pids; i++) {
            sprintf(statpath, "/proc/%d/status", child_pids[i]);

            fp = fopen(statpath, "r");
            if (!fp) continue;

            while (fgets(line, sizeof(line), fp)) {
                if (strncmp(line, "Name:", 5) == 0) {
                    sscanf(line, "%*s %s", procname);
                } else if (strncmp(line, "VmRSS:", 6) == 0) {
                    sscanf(line, "%*s %d", &kb);
                } else if (procname[0] != '\0' && kb > 0) {
                    printf("  %s(%d): %.2f MB\n", procname, child_pids[i], (float)kb * 0.001);
                    total += (float)kb * 0.001;

                    // Clean up
                    memset(&procname[0], 0, sizeof(procname));
                    kb = 0;

                    break;
                }
            }
        }

        fclose(fp);
        free(child_pids);
    }

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
