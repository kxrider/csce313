#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include "helpers.h"
#include <pwd.h>
#include <chrono>
#include <time.h>

using namespace std;

char* initPrompt() {
    uid_t uid = getuid();
    passwd* pwd = getpwuid(uid);
    char* username = pwd->pw_name;

    time_t leTime;
    time(&leTime);
    struct tm * timestrt = localtime(&leTime);
    string actualTime = asctime(timestrt);

    cout << "Welcome to the  h o w d y  shell, " << username << "!" << " The current time is " << actualTime << endl;

    return username;
}

int main(){
    int stdInBackup = dup(0);
    int stdOutBackup = dup(1);
    vector<int> bgprocess;
    string prevDir;
    char tempidk[200];
    getcwd(tempidk, 200);
    prevDir = string(tempidk);
    // initial prompt
    char* user = initPrompt();
    string username(user);

    while (true){
        
        dup2(stdInBackup, 0);
        dup2(stdOutBackup, 1);

        // reaping child processes
        for (int i = 0; i < bgprocess.size(); i++) {
            int wstatus;
            int code = waitpid(bgprocess.at(i), &wstatus, WNOHANG);
            if (code != 0 && code != -1 && WIFEXITED(wstatus)) {
                bgprocess.erase(bgprocess.begin() + i);
            }
                
        }

        char dir[200];
        getcwd(dir, 200);
        cout << dir << ": h o w d y >  "; // print a prompt
        string inputline;
        getline(cin, inputline);// get a line from standard input

        vector<string> checkForCd = splitSpaces(inputline);

        if (inputline == "") continue;

        if (inputline == string("exit")){
            cout << "T's & G's" << endl;
            break;
        } else if (inputline == "prompt") {
            initPrompt();
            continue;
        }
        if (trimSpace(checkForCd.at(0)) == "cd") {
            if (checkForCd.size() > 1) {
                if (checkForCd.at(1).at(0) == '~') {
                    chdir(("/home/" + username + checkForCd.at(1).substr(1, checkForCd.at(1).size()-1)).c_str());
                    continue;
                } else if (checkForCd.at(1).substr(0, 5) == "/home") {
                    chdir(checkForCd.at(1).c_str());
                    continue;
                } else if (checkForCd.at(1) == "-") {
                    chdir(prevDir.c_str());
                    prevDir = dir;
                }
            }
            
            string newPath = string(dir) + "/" + trimSpace(checkForCd.at(1));
            chdir(newPath.c_str());
            continue;
        }
        if (inputline == "") continue;
        processCommand(inputline, bgprocess);
    }

    return 0;
}