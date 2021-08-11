#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

using namespace std;

int num_primes = 0; // holds # of primes

void siginthandler(int sig) {
    cout << endl << "Found " << num_primes << " prime numbers in the range" << endl;
    signal(sig, SIG_DFL);
    raise(sig);
}

void count_primes (int start, int end){
	// check each number in range [start, end] for primality
for (int num = start; num <= end; ++num) {
	int i;
	for (i = 2; (i <= num) && (num % i != 0); ++i)
		;
	if (i == num)  // if no divisor found, it is a prime
		++num_primes;
}
}
int main (int ac, char** av){
    signal(SIGINT, siginthandler);
    count_primes (1, 1000000); // count all primes <= 1 million
}
