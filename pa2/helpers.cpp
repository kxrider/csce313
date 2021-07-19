#include "helpers.h"
#include <sstream>
#include <iostream>

using namespace std;
/*
void redirectRight(string cmd, string out) {
    int fd = open(out.c_str(), O_CREAT | O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(fd, 0);
    //execute(cmd);

}*/
void redirectLeft() {}
/*
vector<string> splitString(string str, string delims) {
    int c = 0;
    bool insideQuotes = false;
    bool inDelims = false;
    int cursor = 0;
    vector<string> output;
    while (c < str.size()) {

        if (!inDelims && isIn(str.at(c), delims)) {
            string thing = str.substr(cursor, cursor-c);
            inDelims = true;
            c++;
            while (c < str.size() && inDelims) {
                if (isIn(str.at(c), delims)) {
                    c++;
                } else {

                }
            }
        } else if (inDelims && isIn(str.at(c), delims)) {

        }

    }
}*/

vector<string> splitPipe(string str) {
    vector<string> outVec;
    int cursor = 0;
    bool insideQt = false;
    char curQt;

    for (int i = 0; i < str.size(); i++) {
        if (str.at(i) == '\"' || str.at(i) == '\'') {
            if (!insideQt) {
                insideQt = true;
                curQt = str.at(i);
            } else if (insideQt && (curQt == str.at(i))) {
                insideQt = false;
            }
            continue;
        }

        if (!insideQt) {
            if (str.at(i) == '|') {
                outVec.push_back(str.substr(cursor, i-cursor));
                cursor = i+1;
            }
        }
    }
    if (str.size() - cursor > 0)
        outVec.push_back(str.substr(cursor, str.size() - cursor));
    
    return outVec;
}

vector<string> splitString(string str, string delim) {}

vector<string> splitSpaces(string str) {
    vector<string> outVec;
    string cur;
    int cursor = 0;
    char curQt;
    bool inQuotes = false;
    for (int i = 0; i < str.size(); i++) {
        if (isIn(str.at(i), "\"\'")) {
            if (inQuotes && (curQt == str.at(i))) {
                inQuotes = false;
                if (cur.size() != 0)
                    outVec.push_back(cur);
                cursor = i+1;
                continue;
            } else if (!inQuotes) {
                inQuotes = true;
                curQt = str.at(i);
                cursor = i + 1;
                if (cur.size() != 0)
                    outVec.push_back(cur);
                continue;
            }
        }
        if (str.at(i) == ' ' && !inQuotes) {
            if (cur.size() != 0) outVec.push_back(cur);
            cur = "";
        } else {
            cur.push_back(str.at(i));
        }
    }
    if (cur.size() != 0) {
        outVec.push_back(cur);
    }
    return outVec;
}

int* locateQuotes(string str, int start = 0) {
    int qtct = 0;
    static int bounds[2];
    char curQt;
    for (int i = start; i < str.size(); i++) {
        if ((str.at(i) == '\"') || (str.at(i) == '\'')) {
            if (qtct++==0) {
                curQt = str.at(i);
                bounds[0] = i;
                continue;
            }
            if (str.at(i) == curQt) {
                bounds[1] = i;
                break;
            }
        }
    }
    return bounds;
}

template<typename T>
void myUnion(vector<T> &l1, const vector<T> &l2) {
    for (int i = 0; i < l2.size(); i++) {
        l1.push_back(l2.at(i));
    }
}


bool isIn(char elem, string l) {
    for (int i = 0; i < l.size(); i++) {
        if (elem == l.at(i)) return true;
    }
    return false;
}
/*
int performOp(string cmd, string arg, char op) {
    if (op == '<')
        return redirectLeft(cmd, arg);
    else if (op == '>')
        return redirectRight(cmd, arg);
    else
        return -1;
}*/
/*
int execute(Metacmd c) {
    if (c.parsedCmd.size() == 1) {
        // regular execvp
    }
    for (int i = 0; i < c.parsedCmd.size(); i++) {
        if (c.parsedCmd.at(i) == "|") {
            Metacmd lhs, rhs;
            lhs.parsedCmd = {c.parsedCmd.begin(), c.parsedCmd.begin() + i-1};
            rhs.parsedCmd = {c.parsedCmd.begin()+i+1, c.parsedCmd.end()};
            pipeCmds(execute(lhs), execute(rhs));
            break;
        }

        if (isIn(c.parsedCmd.at(i), "<>")) {
            // this is where i left off
            
        }
    }
    return 1;
}*/

void myExec(string leCmd) {
    vector<string> spaces = splitSpaces(leCmd);
    vector<const char*> needThis;
    for (int i = 0; i < spaces.size(); i++) {
        needThis.push_back(spaces.at(i).c_str());
    }
    needThis.push_back(NULL);
    execvp(needThis[0], (char *const *) &(needThis[0]));
}

void processCommand(string comnd) {
    vector<string> pipes = splitPipe(comnd);
    string curCmnd = pipes.at(0);
    for (int i = 0; i < pipes.size(); i++) {
        int fd[2];
        pipe(fd);
        int pid = fork();
        if (!pid) {
            if (i < pipes.size()-1)
                dup2(fd[1], 1);

            myExec(pipes.at(i));
        } else {
            if (i == pipes.size()-1) {
                waitpid(pid,0,0);
            }
            dup2(fd[0], 0);
            close(fd[1]);
        }
    }
}


int main() {
    string test = "cat haha.txt | grep epic | grep is";
    processCommand(test);
    return 0;
}