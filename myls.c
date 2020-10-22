/**
 * @file myls.c
 * 
 * @brief Implementation of the ls command with options -ialR.
 * 
 * @author Marcos Antonios Charalambous. 
 * @bug No known bugs.
 **/

#include "myls.h"

/**
*   The main method constitutes the main entry to our myls program.
*   It can be called in various way using different options. In any way we call it,
*   it lists the entries of the current directory in a similar way as ls -ialR.
*   The various options are extracted with the help of the function getopt_long_only.
**/
int main(int argc, char *argv[]){
    
    static struct option long_options[] =
    {
        {"depth", required_argument, NULL, '0'},
        {"follow-symbolic", no_argument, NULL, '1'},
        {"output", required_argument, NULL, '2'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    int depth=0;
    int maxDepth=MAX;
    int followSymbolic=0;
    int output=0; 
    FILE * fp=NULL;
    // Loop over all of the options
    while ((opt = getopt_long_only(argc, argv, ":0:1:2:", long_options, NULL)) != -1)
    {
        // Check to see if a single character or long option came through
        switch (opt)
        {
            case '0': // -depth 
                depth=1;
                maxDepth=atoi(optarg);
                break;
            case '1': // -follow-symbolic
                followSymbolic=1;
                break;
            case '2': // -output
                output=1;
                fp = fopen (optarg,"w");
                char temp[256];
                if (getcwd(temp, sizeof(temp)) == NULL)
                    perror("getcwd() error");
                else
                    fprintf (fp, "%s/\n", temp);    
                break;  
            case ':':  
                printf("Option given needs a value.\n");  
                break;  
            case '?':  
                printf("Unknown option.\n"); 
                break;
        }
    }
    
    if (optind == argc || argc == 1){ // Case where options will run on current working directory.(no extra directory given)
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        else
            printdir(cwd, depth, 0, maxDepth, followSymbolic, output, fp);
    } else { // Directories are given for the options to run on.
        for(; optind < argc; optind++){
            char buf[PATH_MAX];
            char *res = realpath(argv[optind], buf);
            if (res == NULL){
                perror(argv[optind]);
                continue;
            }
            printdir(argv[optind], depth, 0, maxDepth, followSymbolic, output, fp);  
        } 
    }
    if (output == 1)
        fclose (fp);
    return 0;
}

char * permissionCreator(struct stat statbuf) {
    char *permissions = malloc(sizeof (char) * 10); // Permissions of an entry have a fix size of 10 chars.
    char c;

    switch (statbuf.st_mode & S_IFMT) // The type of the file can ve one of the following.
    {
        case S_IFLNK:
            c = 'l';
            break;
        case S_IFBLK:
            c = 'b';
            break;
        case S_IFCHR:
            c = 'c';
            break;
        case S_IFDIR:
            c = 'd';
            break;
        case S_IFIFO:
            c = 'p';
            break;
        case S_IFREG:
            c = '-';
            break;
        case S_IFSOCK:
            c = 's';
            break;
        default:
            c = '?';
            break;
    }
    permissions[0] = c;
    mode_t temp = statbuf.st_mode;
    permissions[1] = (temp & S_IRUSR) ? 'r' : '-'; // Permissions for user, group and others are calculated.
    permissions[2] = (temp & S_IWUSR) ? 'w' : '-';
    permissions[3] = (temp & S_IXUSR) ? 'x' : '-';
    permissions[4] = (temp & S_IRGRP) ? 'r' : '-';
    permissions[5] = (temp & S_IWGRP) ? 'w' : '-';
    permissions[6] = (temp & S_IXGRP) ? 'x' : '-';
    permissions[7] = (temp & S_IROTH) ? 'r' : '-';
    permissions[8] = (temp & S_IWOTH) ? 'w' : '-';
    permissions[9] = (temp & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';    

    return permissions;
}

int digitCounter(unsigned long int number){
    int digits = 0;
    if (number < 0)
        digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits; // Number of digits in an integer.
}

void printListings(struct stat statbuf, struct dirent *entry, int digits, int digitLinks){
    printf("%lu ",statbuf.st_ino);

    char *perms = permissionCreator(statbuf);
    printf("%s ", perms);
    free(perms);

    printf("%*lu ", digitLinks, statbuf.st_nlink);

    struct passwd * uid = getpwuid(statbuf.st_uid);
    printf("%s ", uid->pw_name);
    struct group * gid = getgrgid(statbuf.st_gid);
    printf("%s ", gid->gr_name);
    printf("%*lu ", digits, statbuf.st_size);

    char *modTime;
    modTime = strtok(ctime(&statbuf.st_mtime)," "); // Tokenize the result of ctime.
    int counter = 0;
    char *time;
    char *year;
    while (modTime != NULL){
        if (counter == 1)
            printf("%s ", modTime);
        if (counter == 2)
            printf("%2s ", modTime);
        if (counter == 3)
            time = modTime;
        if (counter == 4){
            year = modTime;
            year[4] = '\0'; // Avoids saving the new line also.
        }
        modTime = strtok(NULL, " ");
        counter++;
    }
    
    if (strcmp(year, "2019") == 0) // Files that have not been modified this year.
        printf("%s ", time);
    else
        printf("%s ", year);
    
    if (S_ISDIR(statbuf.st_mode))
        printf("%s/\n", entry->d_name); // If entry is a directory add /.
    else
        printf("%s\n", entry->d_name);
}

void sortDir(struct dirent ** arr, int n){ 
    struct dirent *temp; 
    int j;
    int i;
    // Sorting entries using bubble sort 
    for (j=0; j<n-1; j++){ 
        for (i=j+1; i<n; i++){ 
            if (strcmp(arr[j]->d_name, arr[i]->d_name) > 0){ 
                temp = arr[j]; 
                arr[j] = arr[i]; 
                arr[i] = temp; 
            } 
        } 
    } 
} 

void printdir(char *dir, int depthFun, int depth, int maxDepth, int symbolicFollow, int output, FILE *fp) {
    struct stat statbuf; // Stat for each entry.

    if (symbolicFollow == 1){ // If the follow symbolic flag is set, 
        if( stat(dir, &statbuf) == -1){ // we must avoid printing the same directories again.
            perror("stat");
            exit(1);
        }
        int r;
        for (r=0; r<visitedCounter; r++){
            if (visited[r] == statbuf.st_ino)
                return; // If the inode of entry has been explored already, no further processing is made.
        }
        visited[visitedCounter++] = statbuf.st_ino; // Add the entry to be explored in the visited array.
    }

    if (depthFun == 1 && depth > maxDepth) // Check if we reached the maximum depth given. (option -depth only)
        return;

    char buf[PATH_MAX];
    char *res = realpath(dir, buf);
    if (depth == 0)
        printf(".:\n"); // If the first time we print, we print it like that.
    else
        printf("\n%s:\n", res); // Print the full path of a directory.

    DIR *dp;

    struct dirent *entry; // Entry in a directory.
    
    if((dp = opendir(dir)) == NULL) { // Pointer of directory passed to dp.
        perror(dir);
        return;
    }

    chdir(dir); // change directory

    struct dirent **array = (struct dirent **) malloc(sizeof(struct dirent *) * MAX);
    if (array == NULL){
        perror("Malloc");
        exit(1);
    }
    
    char **directoryName = (char **) malloc(sizeof(char *) * MAX);
    if (directoryName == NULL){
        perror("Malloc");
        exit(1);
    }

    int num=0;
    int counter = 0;
    int total=0;
    while((entry = readdir(dp)) != NULL) { // For every entry in directory.
        if (symbolicFollow == 0){
            if( lstat(entry->d_name,&statbuf) == -1){ // We care about stats of symbolic link itself.
                perror("lstat");
                exit(1);
            }
        }else{
            if( stat(entry->d_name,&statbuf) == -1){ // We care about stats of original file(pointer of symbolic link).
                perror("stat");
                exit(1);
            }
        }

        array[num++]=entry; // Add all entries to array so can be printed later.

        total +=  1; // Summary of all entries.

        if (S_ISDIR(statbuf.st_mode)) {
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;
            directoryName[counter] = entry->d_name; // We add the directories in a different array so we can explore them later.
            counter++;
        }
    }

    printf("total %d\n",total);

    sortDir(array, total);

    int i;
    unsigned long int max = 0;
    for (i=0; i < num; i++){
        if (symbolicFollow == 0)
            lstat(array[i]->d_name, &statbuf);
        else
            stat(array[i]->d_name, &statbuf);
        
        if (statbuf.st_size >= max){
            max = statbuf.st_size;
        }
    }
    int numDigits = digitCounter(max); // Number of chars to leave when printing.

    max = 0;
    for (i=0; i <  num; i++){
        if (symbolicFollow == 0)
            lstat(array[i]->d_name, &statbuf);
        else
            stat(array[i]->d_name, &statbuf);
        if (statbuf.st_nlink >= max){
            max = statbuf.st_nlink;
        }
    }
    int numDigits2 = digitCounter(max);

    for (i=0; i<num; i++){
        if (symbolicFollow == 0)
            lstat(array[i]->d_name, &statbuf);
        else
            stat(array[i]->d_name, &statbuf);
        printListings(statbuf, array[i], numDigits, numDigits2); // Print all entries in a formatted way.
    }

    if (output ==1 ){ // If the option to print in a file is given we must perform some extra task so we
            for (i=0; i<num; i++){ // can write in a given file.
                if(strcmp(".",array[i]->d_name) == 0 || strcmp("..",array[i]->d_name) == 0)
                        continue; 
                if (symbolicFollow == 0)
                    lstat(array[i]->d_name, &statbuf);
                else
                    stat(array[i]->d_name, &statbuf);               
                char temp[MAX];
                strcpy(temp,res);
                strncat(temp,"/",2);
                strncat(temp, array[i]->d_name, MAX);               
                if (S_ISDIR(statbuf.st_mode)){
                    fprintf (fp, "%s/\n", temp); // If its a directory we add / at the end like the example given.
                    printdir(array[i]->d_name, depthFun, depth + 1, maxDepth, symbolicFollow, output, fp);
                }else
                    fprintf (fp, "%s\n", temp);           
            }           
    }else {
        for (i=0; i<counter; i++)
            printdir(directoryName[i], depthFun, depth + 1, maxDepth, symbolicFollow, output, fp);
    }

    free(array); // Free all allocated memory.
    free(directoryName);

    chdir("..");
    closedir(dp);
}
