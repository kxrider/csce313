#include "helpers.h"
#include <sstream>
#include <iostream>

using namespace std;

string trimSpace(string str) {
    return str.substr(str.find_first_not_of(" "), str.find_last_not_of(" ")+1);
}

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
                outVec.push_back(trimSpace(str.substr(cursor, i-cursor)));
                cursor = i+1;
            }
        }
    }
    if (str.size() - cursor > 0)
        outVec.push_back(trimSpace(str.substr(cursor, str.size() - cursor)));
    
    return outVec;
}

vector<string> splitRedirs(string str) {
    vector<string> outVec;
    int cursor = str.size();
    bool insideQt = false;
    char curQt;
    string charSel;
    for (int i = str.size()-1; i >= 0; i--) {
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
            if (isIn(str.at(i), "<>")) {
                cursor = i;
                charSel = string(1, str.at(i));
                break;
            }
        }
    }
    if (cursor == str.size()) {
        return outVec;
    }
    if (cursor != str.size()) {
        string left = trimSpace(str.substr(0, cursor-1));
        string right = trimSpace(str.substr(cursor+1, str.size()-1-cursor));
        outVec.push_back(trimSpace(charSel));
        outVec.push_back(trimSpace(left));
        outVec.push_back(trimSpace(right));
    }
    
    return outVec;
}

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
                if (cur.size() != 0) {
                    outVec.push_back(cur);
                    cur="";
                }
                cursor = i+1;
                continue;
            } else if (!inQuotes) {
                inQuotes = true;
                curQt = str.at(i);
                cursor = i + 1;
                if (cur.size() != 0) {
                    outVec.push_back(cur);
                    cur="";
                }
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
        outVec.push_back(trimSpace(cur));
    }
    return outVec;
}

bool isIn(char elem, string l) {
    for (int i = 0; i < l.size(); i++) {
        if (elem == l.at(i)) return true;
    }
    return false;
}

bool isIn(int elem, const vector<int>& l) {
    for (int i = 0; i < l.size(); i++)
        if (elem == l.at(i)) return true;
    return false;
}

void redirRight(string cmd, string out) {
    int fd = open(out.c_str(), O_CREAT | O_WRONLY | O_TRUNC, O_CREAT | O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(fd, 1);
    myExec(cmd);

}

void redirLeft(string cmd, string in) {
    int fd = open(in.c_str(), O_RDONLY);
    dup2(fd, 0);
    myExec(cmd);
}

void myExec(string leCmd) {
    vector<string> redirs = splitRedirs(leCmd);
    if (redirs.size() != 0) {
        if (redirs[0] == "<")
            redirLeft(redirs[1], redirs[2]);
        else
            redirRight(redirs[1], redirs[2]);
    }
    vector<string> spaces = splitSpaces(leCmd);
    vector<const char*> needThis;
    for (int i = 0; i < spaces.size(); i++) {
        needThis.push_back(spaces.at(i).c_str());
    }
    needThis.push_back(NULL);
    if (execvp(needThis[0], (char *const *) &(needThis[0])) == -1) exit(0);
}

void processCommand(string comnd, vector<int>& bgprocess) {
    string edcomnd = trimSpace(comnd);
    bool runInBg = false;
    if (comnd.at(comnd.size()-1) == '&') {
        runInBg = true;
        edcomnd = trimSpace(edcomnd.substr(0, edcomnd.size()-1));
    }

    vector<string> pipes = splitPipe(edcomnd);
    string curCmnd = pipes.at(0);
    for (int i = 0; i < pipes.size(); i++) {
        int fd[2];
        pipe(fd);
        int pid = fork();
        if (!pid) {
            if (i < pipes.size()-1)
                dup2(fd[1], 1);

            myExec(trimSpace(pipes.at(i)));
        } else {
            if (!runInBg && i == pipes.size()-1) {
                waitpid(pid,0,0);
            } else
                bgprocess.push_back(pid);
            dup2(fd[0], 0);
            close(fd[1]);
        }
    }
}

/*
int main() {
    string test = "cat < a | grep is";
    vector<string> asdf = splitPipe(test);
    for (int i = 0; i < asdf.size(); i++) {
        cout << asdf.at(i) << endl;
    }
    vector<string> othertest = splitRedirs(asdf[0]);
    for (int i = 0; i < othertest.size(); i++) {
        cout << othertest.at(i) << endl;
    }
    //vector<int> bg;
    //processCommand(test, bg);
    return 0;
}*/