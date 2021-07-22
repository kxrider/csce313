#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <vector>

using namespace std;

int main() {
    int p = getpid();
    cout << "parent: " << getpid() << endl;
for (int i=0; i<4; i++){
    int cid = fork ();
    if (i < 2)
        wait (0);
    cout << "ID=" << getpid () - p + 1000 << endl;
}
}