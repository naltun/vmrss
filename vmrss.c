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

// Global variables
char fileline[50], procname[50], statpath[50];
DIR *dir;
FILE *fp;
int *child_pids, kilobytes, ppid_tmp, totalbytes;
struct dirent *entry;

static int *find_child_processes(int ppid, int *size) {
    int arrayindex;
    int arraylimit;

    arrayindex = 0;
    arraylimit = 10;
    child_pids = NULL;

    dir = opendir("/proc");
    if (!dir) {
        *size = 0;
        return NULL; // Correctly handle directory open failure
    }

    child_pids = malloc(arraylimit * sizeof(int)); // Initial allocation
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

            while (fgets(fileline, sizeof(fileline), fp)) {
                if (sscanf(fileline, "PPid: %d", &ppid_tmp) == 1 && ppid_tmp == ppid) {
                    // Check if we need to dynamically allocate more memory
                    if (arrayindex >= arraylimit) {
                        arraylimit *= 2;
                        int *new_ptr = realloc(child_pids, arraylimit * sizeof(int));
                        if (!new_ptr) {
                            free(child_pids);
                            fclose(fp);
                            closedir(dir);
                            *size = 0;
                            return NULL;
                        }
                        child_pids = new_ptr;
                    }

                    child_pids[arrayindex++] = atoi(entry->d_name);
                    break;
                }
            }
            fclose(fp);
        }
    }
    closedir(dir);

    *size = arrayindex;
    return child_pids;
}

static float get_parent_vmrss(int pid) {
    int megabytes;

    // Set statpath to /proc/PID/status
    sprintf(statpath, "/proc/%d/status", pid);
    fp = fopen(statpath, "r");

    if (!fp) {
        fprintf(stderr, "Total: 0 MB\n");
        exit(EXIT_FAILURE);
    }

    // Initialize procname to check for an update later
    procname[0] = '\0';

    // Iterate over the lines of /proc/PID/status...
    while (fgets(fileline, sizeof(fileline), fp)) {
        // ...to get the PID name
        if (strncmp(fileline, "Name:", 5) == 0) {
            sscanf(fileline, "%*s %s", procname);
        // ...and the VmRSS (Resident Set Size) value
        } else if (strncmp(fileline, "VmRSS:", 6) == 0) {
            sscanf(fileline, "%*s %d", &kilobytes);
        } else {
            continue;
        }
    }

    fclose(fp);

    // Check if procname has been updated and if kilobytes has a positive value
    if (procname[0] != '\0' && kilobytes > 0) {
        // 1 MB = 1 kB * 10 ** -3
        megabytes = kilobytes * 0.001;
        printf("%s(%d) %.2f MB\n", procname, pid, (float)megabytes);
    } else {
        fprintf(stderr, "Total: 0 MB\n");
        exit(EXIT_FAILURE);
    }

    return megabytes;
}

static int print_vmrss(int pid) {
    int size_child_pids;

    // Get PID VmRSS in MB
    totalbytes = get_parent_vmrss(pid);

    // Check for child processes
    child_pids = find_child_processes(pid, &size_child_pids);
    if (child_pids) {
        procname[0] = '\0';

        for (int i = 0; i < size_child_pids; i++) {
            sprintf(statpath, "/proc/%d/status", child_pids[i]);

            fp = fopen(statpath, "r");
            if (!fp) continue;

            while (fgets(fileline, sizeof(fileline), fp)) {
                if (strncmp(fileline, "Name:", 5) == 0) {
                    sscanf(fileline, "%*s %s", procname);
                } else if (strncmp(fileline, "VmRSS:", 6) == 0) {
                    sscanf(fileline, "%*s %d", &kilobytes);
                } else if (procname[0] != '\0' && kilobytes > 0) {
                    printf("  %s(%d): %.2f MB\n", procname, child_pids[i], (float)kilobytes * 0.001);
                    // 1 MB = 1 kB * 10 ** -3
                    totalbytes += kilobytes * 0.001;

                    // Clean up
                    memset(&procname[0], 0, sizeof(procname));
                    kilobytes = 0;

                    break;
                }
            }
        }

        fclose(fp);
        free(child_pids);
    }

    return totalbytes;
}

int main(int argc, char **argv) {
    // Process ID to work with
    int pid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert string (e.g. "1530") to int
    pid = atoi(argv[1]);
    // Print VmRSS info from PID and return total memory used in MB
    totalbytes = print_vmrss(pid);
    printf("Total: %.2f MB\n", (float)totalbytes);

    return EXIT_SUCCESS;
}
