//DELETE STUDENT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[]) {
    char directory[MAX], command[MAX];
    int exitCode;
    pid_t pid;
    if (argc != 2) {
        write(2, "DeleteStudent: missing parameters!\n", strlen("DeleteStudent: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}
    //first going to check the permissions of the student's directory.
    memset(directory, '\0', MAX);
    strcpy(directory, "./student/");
    strcat(directory, argv[1]);
    //in case there are no permissions
    chmod(directory, 0777);

    //checking if there's a schedule file in the directory of the student
    strcat(directory, "/Schedule.txt");
    if (access(directory, F_OK) == 0) { //if exists
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
        }
    }

    //checking if there's a blocked file in the directory of the student
    memset(directory, '\0', MAX);
    strcpy(directory, "./student/");
    strcat(directory, argv[1]);
    strcat(directory, "/Blocked.txt");
    if (access(directory, F_OK) == 0) { //if exists
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
        }
    }

    //now removing the directory
    memset(directory, '\0', MAX);
    strcpy(directory, "./student/");
    strcat(directory, argv[1]);
    if (rmdir(directory) == -1) {
        write(2, "Removing directory error.\n", strlen("Removing directory error.\n"));
        exit(-1);
    }
}