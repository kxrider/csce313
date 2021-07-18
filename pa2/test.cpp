#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include "Metacmd.h"

using namespace std;

// send message from child to parent
int main(int argc, char* argv[]) {
    string test = "cmd c > f | grep h";
    Metacmd theCmd(test);
    for (int i = 0; i < theCmd.parsedCmd.size(); i++) {
        cout << theCmd.parsedCmd.at(i) << " ";
    }
    cout << endl;

    return 0;
}
