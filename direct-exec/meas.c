#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ITER 10000

long meas_syscall()
{
    char buf;
    struct timespec tps, tpe;
    if (clock_gettime(CLOCK_REALTIME, &tps)) {return -1;}
    for (int i = 0; i < ITER; i++) {
        read(STDOUT_FILENO, &buf, 0);
    }
    if (clock_gettime(CLOCK_REALTIME, &tpe)) {return -1;}

    long nsec = tpe.tv_nsec - tps.tv_nsec;
    return nsec/ITER;
}

long meas_ctx()
{
    char buf = '\0';
    int fd[2];
    int stat = pipe(fd);
    if (stat == -1) {return stat;}

    cpu_set_t set;
    CPU_ZERO(&set);
    switch(fork()) {
        case -1:
            return -1;
        case 0:
            close(fd[1]);
            CPU_SET(0, &set);
            if (sched_setaffinity(getpid(), sizeof(set), &set)) {exit(EXIT_FAILURE);}
            for (int i = 0; i < ITER; i++) {
                read(fd[0], &buf, 1);
            }
            exit(EXIT_SUCCESS);
        default:
            close(fd[0]);
            CPU_SET(0, &set);
            if (sched_setaffinity(getpid(), sizeof(set), &set)) {return -1;}
            struct timespec tps, tpe;
            clock_gettime(CLOCK_REALTIME, &tps);
            for (int i = 0; i < ITER; i++) {
                write(fd[1], &buf, 1);
            }
            wait(NULL);
            clock_gettime(CLOCK_REALTIME, &tpe);
            return (tpe.tv_nsec - tps.tv_nsec)/(ITER*2);
    }
    return 0;
}

int main()
{
    printf("Time between syscalls: %ldns\n", meas_syscall());

    struct timespec tps, tpe;
    clock_gettime(CLOCK_REALTIME, &tps);
    clock_gettime(CLOCK_REALTIME, &tpe);

    long nsec = tpe.tv_nsec - tps.tv_nsec;
    printf("Time between gettime: %ldns\n", nsec);

    printf("Time between context switches: %ldns\n", meas_ctx());

    return 0;
}
