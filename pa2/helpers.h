#ifndef HELPERS_H
#define HELPERS_H

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
using std::string;

int redirectRight(string, string);
int redirectLeft(string, string);
void pipeCmds(int, int);
//int execute(Metacmd);
vector<string> splitString(string, string);
vector<string> splitSpaces(string);
void processCommand(string);
void myExec(string);
int* locateQuotes(string, int);
template<typename T>
vector<T> myUnion(vector<T>&, const vector<T>&);
bool isIn(char, string);

#endif