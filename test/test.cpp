#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

void signal_handler (int signo) {
    printf("Got SIGUSR1\n");
}

int main() {
    //signal(SIGUSR1, signal_handler);
    int pid = fork();
    if (pid==0) {
        for (int i = 0; i < 5; i++) {
            kill(getppid(), SIGUSR1);
            sleep(1);
        }
    } else {
        wait(0);
    }
}