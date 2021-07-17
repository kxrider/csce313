#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

using namespace std;

void redirectRight(string, string);
void redirectLeft(string, string);
void pipeCmds(string, string);
void execute(string);
char** splitString(string, string);

#endif