#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include "helpers.h"

using namespace std;

int main(){
    while (true){
        cout << "h o w d y >  "; // print a prompt
        string inputline;
        getline(cin, inputline);// get a line from standard input
        if (inputline == string("exit")){
            cout << "Goodbye..." << endl;
            break;
        }
        processCommand(inputline);
        /*
        int pid = fork();
        if (pid == 0){// child process
            // preparing the input command for execution
            char* args[] = {(char*) inputline.c_str(), nullptr};
            execvp(args[0],args);
        } else {
            waitpid(pid, 0, 0); // parent waits for childprocess
        }*/
    }

    return 0;
}