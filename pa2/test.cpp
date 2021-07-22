#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>
//#include "Metacmd.h"

using namespace std;

// send message from child to parent
int main(int argc, char* argv[]) {
    char* const stuff[3] = {const_cast<char*>("echo"), const_cast<char*>("howdy, I am writing random stuff idk."), NULL};
    
    if (!fork()) {
        int fd = open(const_cast<char*>("tstfile.txt"), O_CREAT | O_WRONLY | O_TRUNC, 0777);
        dup2(fd, 1);
        //write(fd, "howdy lol", 8);
        execvp(stuff[0], stuff);
    } else {
        wait(0);
        cout << "done!" << endl;
    }
    return 0;
}
