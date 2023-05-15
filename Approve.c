//APPROVE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX 256
#define ID 9

void main(int argc, char* argv[]) {
    int i, exitCode;
    char directory[MAX], command[MAX];
    pid_t pid;
    if (argc != 2 || strlen(argv[1]) != 9) {
        write(2, "Approve: missing parameters!\n", strlen("Approve: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

    //first checking if the given argument is an id number
    for (i = 0; i < ID; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            write(2, "Problem with the ID number.\n", strlen("Problem with the ID number.\n"));
            exit(-1);
        }
    }
    
    //now getting the directory path of the student
    strcpy(directory, "./student/");
    strcat(directory, argv[1]);

    //changing the permission mode of the student's path
    if (chmod(directory, 0777) == -1) {
        write(2, "Changing mode failure.\n", strlen("Changing mode failure.\n"));
        exit(-1);
    }
    else { //removing the blocked file from the directory
        strcat(directory, "/Blocked.txt");
        //calling the rm command from bin
        pid = fork();
        if (pid < 0) { //fork error
            perror("Fork failure.\n");
			exit(-1);
        }
        else if (pid == 0) { //child process
            execlp("/bin/rm", "bin/rm", "-r", directory, NULL);
            perror("Remove execution failure.\n");
			exit(-1);
        }
        else { //parend process
            waitpid(pid, &exitCode, 0);
            if (WIFEXITED(exitCode)) {
                exit(0);
            }
        }
    }
}
