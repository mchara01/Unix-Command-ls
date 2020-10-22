## ls -ialR
Implementation of a subset of functions of the ls command, namely the (-ialR) arguments, without the use of the exec command family. The program allows users to set the maximum depth of recursion (which is not provided by the standard ls command). The ls command represents the contents of a directory given as a parameter. If the user fails to give a parameter then the command automatically displays the contents of the "." directory (current directory). The user also has the ability to pass multiple files as arguments. In this case, they will present the contents of all user-defined directories. More details of the command and the aforementioned arguments can be seen at the image below.

![alt text](https://github.com/mchara01/Unix-Command-ls/blob/main/images/ls_def.JPG?raw=true)

### Functionality
Compile the source code first with: `gcc myls.c -o myls`

The different ways the program can execute are listed below

`myls` : Performs a corresponding function with the command ls –ialR<br />
`myls a/ b/ c/` : Performs a corresponding function with the command ls –ialR a/ b/ c/ <br />
`myls –depth 2` : Performs a corresponding function with the command ls –ialR but the depth of investigation is limited to two levels.<br />
`myls -follow-symbolic` : The algorithm follows both the symbolic files and directories. If it is a directory then it prints its contents normally. In this case, a check is made to see if the directory has already been printed so that it does not re-print it (nor further edit it). This control it is as efficient as possible since scanning the entire list of previous paths is avoided. Note that the search algorithm in all cases is Depth-First-Search.<br />
`myls –output traversal-log.txt` : Prints to a file named traversal-log.txt the depth-first-search path followed by the myls command.<br />

Note that the application supports any combination of the above arguments and produces the expected result.
