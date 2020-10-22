/**
 * @file myls.h
 * 
 * @brief Implementation of the ls command with options -ialR.
 * 
 * @author Marcos Antonios Charalambous. 
 * @bug No known bugs.
 **/

#include <stdio.h> // NULL
#include <sys/types.h>
#include <dirent.h> // opendir, readdir, closedir
#include <unistd.h> // chdir
#include <sys/stat.h> // lstat
#include <stdlib.h> // exit
#include <string.h> // strcmp
#include <pwd.h> // working directory
#include <grp.h>
#include <time.h> // ctime
#include <limits.h>
#include <getopt.h> // getopt

#define MAX 1024 // We will use a MAX value to allocate space always.

// function prototypes

/**
*   In this function, the permissions that would appear for each
*   entry in a directory, are printed. Various checks are made to
*   see which permissions apply for each file. For the first charachter,
*   that represents the type of file, a sepperate check is made 
*   to ensure that it will be only one type. For the permission of
*   the user, group and others, depending if read, write or execute are
*   allowed, we add the symbols r w x respectively. If a permission
*   is not allowed for a group is represented with -.
**/
char * permissionCreator(struct stat);

/**
*   Method to calculate the digits in a long integer given as a parameter.
**/
int digitCounter(unsigned long int);

/**
*   In printListings, we print the entries in a directory passed in
*   the method as the argument entry in the exact way they are printed
*   with ls -ialR. The information for an entry in a directory can be found
*   in the statbuf argument. For fields hard links number, size and day we 
*   count the number of the maximum instance for each one and allocate that
*   many spaces for everyone. Also for files that have not been modified this
*   year we show the year instead of the time.   
**/
void printListings(struct stat, struct dirent *, int, int);

/**
*   Bellow we can find the implementation of a bubble sort
*   algorithm for sorting all the entries in a directory. This
*   way, printing the contents of a directory with myls will be 
*   identical with the output of ls.
**/
void sortDir(struct dirent **, int);

/**
 *  In the method printdir the recursion is implemented. The various options that
 *  our myls program can take, are passed inside the method as boolean parameters.
 *  If a valid option must be taken in to consideration when the program executes, 
 *  the parameter will have the value 1, otherwise it will have the value 0. Moreover,
 *  here is where we explore all the directories in a DFS manner. This means that if a 
 *  directory is found, we explore it first and all its entries and then we return back
 *  to continue. All entries of a directory can be viewd either with lstat or stat, depending
 *  whether we care about info about the symbolic link itself or the orginal file. All entries
 *  are sorted with bubble sort before printing and if an entry is a directory we append a /
 *  at the end.
 * */
void printdir(char *, int, int, int, int, int, FILE *);

int visited[MAX];
int visitedCounter = 0;
