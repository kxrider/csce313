#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

using namespace std;


int num_children_done = 0;
void my_handler (int sig){
     cout<<++num_children_done<< " children done"<<endl;    
}
int main (){
    // install handler
    signal (SIGCHLD, my_handler);
    // create 5 child procs, 
    for (int i=1; i<=5; i++){
        int pid = fork ();
        if (pid == 0){
            sleep (5);//all die simultaneously     
   	      return 0; 
        }
    }
    while (true)
       sleep (1); 
}
