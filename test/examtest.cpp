#include <iostream>
#include <thread>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int count = 0;

void count_handler (int sig){

    count ++;

    wait (0);

}

 

int main (){

    signal (SIGCHLD, count_handler);

    for (int i=1; i<=5; i++){

        int pid = fork ();

        if (pid == 0){

            sleep (5);//all die simultaneously 

            return 0;

        }

    }

    sleep (8);

    cout << "Reaped "  <<count  << " children" << endl;

}

