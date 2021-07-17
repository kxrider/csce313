#ifndef METACMD_H
#define METACMD_H

#include <string>
#include <vector>
using namespace std;

class Metacmd {
public:
    vector<string> parsedCmd;
    Metacmd(string);
};

class Cmd : Metacmd {
public:
    char** args;
    Cmd(string);
};

Metacmd::Metacmd(string rawCmd) {
    // parse details oof
    // cat file.txt | grep howdy ---> ["|", "cat file.txt", "grep howdy"]
}

Cmd::Cmd(string rawCmd) : Metacmd(rawCmd) {
    // lol parse details again
    // need to pick out arguments i.e.
    // ls -a -l -w ---> {ls, -a, -l, -w}
    // i.e. split command at spaces
}


#endif