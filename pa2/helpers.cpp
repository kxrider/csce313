#include "helpers.h"
#include <sstream>
#include <iostream>

using namespace std;

void redirectRight(string cmd, string out) {
    int fd = open(out.c_str(), O_CREAT | O_WRONLY | O_TRUNC | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    dup2(fd, 0);
    //execute(cmd);

}
void redirectLeft() {}

vector<string> splitString(string str, string delims) {

    vector<string> outVec;
    string inp;

    char curQt;
    bool insideQt = false;
    int cursor = 0;
    for (int i = 0; i < str.size(); i++) {
        if ((str.at(i) == '\"') || (str.at(i) == '\'')) {
            if (insideQt && (str.at(i) == curQt)) {
                insideQt = false;
            } else if (!insideQt) {
                curQt = str.at(i);
                insideQt = true;
            }
            continue;
        }
        if (!insideQt) {
            if (isIn(str.at(i), delims)) {
                outVec.push_back(str.substr(cursor, i-cursor));
                string op;
                op += str.at(i);
                outVec.push_back(op);
                cursor = i+1;
            }
        }
    }
    outVec.push_back(str.substr(cursor, str.size()-cursor));

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
}


int main() {
    string test = "cat -e -f < file.txt | grep idk";
    vector<string> asdf = splitString(test, "|<>");
    for (int i = 0; i < asdf.size(); i++) {
        cout << asdf.at(i) << endl;
    }
    return 0;
}