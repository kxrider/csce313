#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include "Metacmd.h"

using namespace std;

void redirectRight(string, string);
void redirectLeft(string, string);
void pipeCmds(string, string);
void execute(Metacmd);
vector<string> splitString(string, string);
int* locateQuotes(string, int);
template<typename T>
vector<T> myUnion(vector<T>&, const vector<T>&);
bool isIn(char, string);

#endif