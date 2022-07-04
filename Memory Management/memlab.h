// Group No- 53
// Suryam Arnav Kalra - 19CS30050
// Kunal Singh - 19CS30025

#ifndef MEMLAB_H
#define MEMLAB_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define INT 1
#define CHAR 2
#define MEDINT 3
#define BOOL 4
#define MAX_SIZE 100
#define MAX_CHAR 10
#define REQD 3000000

void createMem(int);   // size
void createVar(char *, int);  // name, type
void assignVar(char *, int, int); // name, type, val
void createArr(char *, int, int); // name, type, size
void assignArr(char *, int, int, int); // name, type, val, which array element
int getVar(char *);
int getArrElem(char *, int);
void freeElem(char *);  // name
void sweepPhase(char *);
void compactPhase();
void gc_run();


#endif