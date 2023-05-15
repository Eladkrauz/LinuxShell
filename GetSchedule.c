//GET SCHEDULE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[]) { //argv[1] has the ID of the student, argv[2] has a flag
    //this flag indicates if the program executed from stdShell (0) or from lecShell(1)
    //if from lecShell, there's a need for checking the validation of the id number given
    int stdFile, schedule, isID, i, rbytes, flag = 0;
    char directory[MAX], buffer[MAX], ch, try[1024];
   
    if (argc != 3) {
        write(2, "GetSchedule: missing parameters!\n", strlen("GetSchedule: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

    //if argv[2] is 0, it means that the program executed from stdShell
    //and there is no need for checking the validation of the id, since the student
    //has logged in to stdShell, so for sure he is an existed student.

    if (strcmp(argv[2], "1") == 0) { //executed from lecShell, checking the id number
        //checking if the argument in argv[1] is even an ID
        if (strlen(argv[1]) == 9) { //start checking 
            isID = 1;
            for (i = 0; i < strlen(argv[1]); i++) {
                if (argv[1][i] < '0' || argv[1][i] > '9') {
                    isID = 0;
                    break;
                }
            }
        }
        else //not an ID
            isID = 0;

        //if isID is 0 now, it means that the given argument is not an id.
        if (isID == 0) {
            printf("The argument is not a valid id number.\n");
            exit(2);
        }

        else { //if an ID, checking if exists in the students file and then sets the directory
            if ((stdFile = open("std_pass.txt", O_RDONLY)) == -1) {
                write(2, "Opening students file failure.\n", strlen("Opening students file failure.\n"));
                exit(-1);
            }
            while (1) {
                lseek(stdFile, 32, 1); //moving to the ID column
                memset(buffer, '\0', MAX);
                if ((rbytes = read(stdFile, buffer, 9)) == -1) {
                    write(2, "Reading students file failure.\n", strlen("Reading students file failure.\n"));
                    exit(-1);
                }
                if (strcmp(buffer, argv[1]) == 0) {
                    flag = 1;
                    break;
                }
                else if (rbytes == 0) {
                    break;
                }
                else {
                    while (1) { //moving to the next line
                        if ((rbytes = read(stdFile, &ch, 1)) == -1) {
                            write(2, "Reading students file failure.\n", strlen("Reading students file failure.\n"));
                            exit(-1);
                        }
                        if (ch == '\n')
                            break;
                    }
                }
            }

            if (flag == 0) { //this id number does not exist in the students file
                printf("Student Not Found.\n");
                exit(0);
            }
        }
    }

    strcpy(directory, "./student/");
    strcat(directory, argv[1]);
    strcat(directory, "/Schedule.txt");
    directory[strlen("./student") + strlen(argv[1]) + strlen("/Schedule.txt") + 1] = '\0';

    //checking if there's a schedule in the student' directory
    if (access(directory, F_OK) != 0) { //if does not exist
		printf("Student Not Register!\n");
		exit(0);
	}
    else { //if exists, opens it and prints its content
        if ((schedule = open(directory, O_RDONLY)) == -1) {
            write(2, "Opening student's schedule file failure.\n", strlen("Opening student's schedule file failure.\n"));
            exit(-1);
        }

        while (1) { //reading and writing the content of the file
            memset(buffer, '\0', MAX);
            if ((rbytes = read(schedule, buffer, MAX)) == -1) {
                write(2, "Reading schedule failure.\n", strlen("Reading schedule failure.\n"));
                exit(-1);
            }
            if (rbytes == 0) //EOF
                break;
            else {
                write(1, buffer, strlen(buffer));
                continue;
            }
        }
    }
    printf("\n");
    close(stdFile);
    close(schedule);
    exit(0);
}
