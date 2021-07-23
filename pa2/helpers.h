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

using namespace std;
using std::string;

void redirRight(string, string);
void redirLeft(string, string);
void pipeCmds(int, int);
vector<string> splitString(string, string);
vector<string> splitSpaces(string);
void processCommand(string, vector<int>&);
void myExec(string);
int* locateQuotes(string, int);
template<typename T>
vector<T> myUnion(vector<T>&, const vector<T>&);
bool isIn(char, string);
bool isIn(int, const vector<int>&);
string trimSpace(string);

#endif