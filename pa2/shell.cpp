#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include "helpers.h"
#include <pwd.h>
#include <chrono>
#include <time.h>

using namespace std;
using namespace std::chrono;
//using namespace date;

int main(){
    int stdInBackup = dup(0);
    int stdOutBackup = dup(1);
    vector<int> bgprocess;

    // initial prompt
    uid_t uid = getuid();
    passwd* pwd = getpwuid(uid);
    char* username = pwd->pw_name;

    time_t leTime;
    time(&leTime);
    struct tm * timestrt = localtime(&leTime);
    string actualTime = asctime(timestrt);

    cout << "Welcome to the  h o w d y  shell, " << username << "!" << " The current time is " << actualTime << endl;


   /* string date;
    int fdp[2];
    pipe(fdp);
    if (!fork()) {

    }
    //auto t = zoned_time{current_zone(), system_clock::now()};
    cout << system_clock << endl;*/

    while (true){
        
        dup2(stdInBackup, 0);
        dup2(stdOutBackup, 1);

        for (int i = 0; i < bgprocess.size(); i++) {
            int wstatus;
            int code = waitpid(bgprocess.at(i), &wstatus, WNOHANG);
            if (code != 0 && code != -1 && WIFEXITED(wstatus)) {
                bgprocess.erase(bgprocess.begin() + i);
                //printf("something erased\n");
            }
                
        }

        char dir[200];
        getcwd(dir, 200);

        cout << dir << ": h o w d y >  "; // print a prompt
        string inputline;
        getline(cin, inputline);// get a line from standard input

        vector<string> checkForCd = splitSpaces(inputline);

        if (inputline == string("exit")){
            cout << "Goodbye..." << endl;
            break;
        }
        if (trimSpace(checkForCd.at(0)) == "cd") {
            string newPath = string(dir) + "/" + trimSpace(checkForCd.at(1));
            chdir(newPath.c_str());
            continue;
        }
        if (inputline == "") continue;
        processCommand(inputline, bgprocess);

        
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