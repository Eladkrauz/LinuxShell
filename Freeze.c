//FREEZE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#define MAX 256
#define ID 9

void main(int argc, char* argv[]) {
    int i, itsAnId = 1, stdFile, blocked;
    char directory[MAX], id[ID + 1], buffer[MAX], ch;
    if (argc != 2) {
        write(2, "Freeze: missing parameters!\n", strlen("Freeze: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

    //first checking if the given argument is an id number
    if (strlen(argv[1]) == 9) {
        for (i = 0; i < ID; i++) {
            if (argv[1][i] < '0' || argv[1][i] > '9') {
                itsAnId = 0;
                break;
            }
        }
    }
    else //can't be an idea with less than 9 digits.
        itsAnId = 0;
    //if itsAnId = 1 >> argv[1] holds the ID number of the student
    if (itsAnId == 1) {
        strcpy(id, argv[1]);
    }
    else {
        //if not >> looking for the username in the students file
        if ((stdFile = open("std_pass.txt", O_RDONLY)) == -1) {
            write(2, "Opening students file failure.\n", strlen("Opening students file failure.\n"));
            exit(-1);
        }
        //moving to the second line (the first line is the title of the table - not relevant)
        while (read(stdFile, &ch, 1) == 1)
        {
            if (ch == '\n')
                break;
        }
        //now moving to the student's username >> must exist cause tried to log into stdShell
        while (1) {
            memset(buffer, '\0', MAX);
            if (read(stdFile, buffer, strlen(argv[1])) == -1) {
                write(2, "Reading students file failure.\n", strlen("Reading students file failure.\n"));
                exit(-1);
            }
            if (strcmp(buffer, argv[1]) == 0)
                break;
            else {
                //moving to the next line
                while (1) {
                    if (read(stdFile, &ch, 1) == -1) {
                        write(2, "Reading students file failure.\n", strlen("Reading students file failure.\n"));
                        exit(-1);
                    }
                    if (ch == '\n')
                        break;
                }
            }
        }
        //moving to the ID column and reading the ID
        i = 0; //counting two columns, from username to password (1), and than from password to id number (2)
        while (1) {
            if (read(stdFile, &ch, 1) == -1) {
                perror("Reading students file failure.\n");
                exit(-1);
            }
            if (ch == ' ') //between columns
                continue;
            else {
                i++;
                if (i == 2)
                    break;
                while (1) { //if i < 2, moving to the next column
                    if (read(stdFile, &ch, 1) == -1) {
                        perror("Reading students file failure.\n");
                        exit(-1);
                    }
                    if (ch != ' ')
                        continue;
                    else
                        break;
                }
            }
        }
        //now reading the id
        lseek(stdFile, -1, 1);
        memset(id, '\0', MAX);
        if (read(stdFile, id, 9) == -1) {
            perror("Reading students file failure.\n");
            exit(-1);
        }
        id[10] = '\0';
        close(stdFile);
    }

    //now getting the directory path of the student
    strcpy(directory, "./student/");
    strcat(directory, id);
    //checking if the student already blocked
    if (chmod(directory, 0777) == -1) {
        write(2, "Changing mode failure.\n", strlen("Changing mode failure.\n"));
        exit(-1);
    }
    strcat(directory, "/Blocked.txt");
    if (access(directory, F_OK) == 0) { //student is already blocked
        memset(directory, '\0', MAX);
        strcpy(directory, "./student/");
        strcat(directory, id);
        if (chmod(directory, 0000) == -1) {
            write(2, "Changing mode failure.\n", strlen("Changing mode failure.\n"));
            exit(-1);
        }
        exit(0);
    }
    else { //adding Blocked file to the directory
        if ((blocked = open(directory, O_WRONLY | O_CREAT, 0777)) == -1) {
            write(2, "Opening blocked file failure.\n", strlen("Opening blocked file failure.\n"));
            exit(-1);
        }
        write(blocked, "Blocked", 7);
        close(blocked);
        //changing the permission mode of the student's path
        memset(directory, '\0', MAX);
        strcpy(directory, "./student/");
        strcat(directory, id);
        if (chmod(directory, 0000) == -1) {
            write(2, "Changing mode failure.\n", strlen("Changing mode failure.\n"));
            exit(-1);
        }
        else {
            exit(0);
        }
    }
}
