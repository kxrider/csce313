#include <helpers.h>
#include <sstream>

using namespace std;

void redirectRight(string cmd, string out) {
    int fd = open(out.c_str(), O_CREAT | O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(fd, 0);
    //execute(cmd);

}
void redirectLeft() {}

char** splitString(string str, string delim) {
    istringstream ss(str);
    vector<string> outVec;
    string in;
    while (ss >> in)
        outVec.push_back(in);

    // TODO: put in char** array. Could be issue if length must be decided at compile time.
}