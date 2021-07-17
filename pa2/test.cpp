#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

// send message from child to parent
int main(int argc, char* argv[]) {
    for (int i  = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }
    return 0;
}
