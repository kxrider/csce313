#ifndef METACMD_H
#define METACMD_H

#include <string>
#include <vector>
#include "helpers.h"
using namespace std;

class Metacmd {
public:
    vector<string> parsedCmd;
    Metacmd(string);
    Metacmd() : parsedCmd(vector<string>()) {}
};

class Cmd : Metacmd {
public:
    vector<string> args;
    Cmd(string);
};

string parseChars = "|<>";

/*vector<string> metaConsHelper(string txt, string ls, int n) {
    vector<string> parsed = splitString(txt, ls.at(n));
    if (n+1 >= ls.size()) return parsed;

    vector<string> finalVec;
    for (int i = 0; i < parsed.size(); i++) {
        myUnion<string>(finalVec, metaConsHelper(parsed.at(i), ls, n+1) );
        
    }
    return finalVec;
}*/
Metacmd::Metacmd(string rawCmd) {
    // parse details oof
    // cat file.txt | grep howdy ---> ["|", "cat file.txt", "grep howdy"]
    parsedCmd = splitString(rawCmd, "|<>");
}

Cmd::Cmd(string rawCmd) : Metacmd(rawCmd) {
    // lol parse details again
    // need to pick out arguments i.e.
    // ls -a -l -w ---> {ls, -a, -l, -w}
    // i.e. split command at spaces
}


#endif