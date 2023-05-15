//STD SHELL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[])
{
	int stdPasswords, rbytes, exitCode, accessCode;
	char input[MAX], *token, directory[MAX];
	pid_t pid;

	//checking if the student has access to directory
	//three options here:
	//1) student has a directory and access to it
	//2) student does not have directory >>> he got deleted by a lecturer
	//3) student has a directory, without permissions >>> blocked
	memset(directory, '\0', MAX);
	strcpy(directory, "./student/");
	strcat(directory, argv[2]);
	//checking access to the directory
	if (access(directory, R_OK | W_OK | X_OK) == 0) {
		accessCode = 1; //option no. 1
	}
	else {
		if (access(directory, F_OK) != 0) {
			accessCode = 2; //option no. 2
		}
		else {
			accessCode = 3; //option no. 3
		}
	}

	//Now starting the shell's' action >> getting inputs from the user
	while (1) {
		printf("StdShell> ");
		memset(input, 0, MAX); //clearing the buffer
		fgets(input, MAX, stdin);
		if (strlen(input) > 0 && input[strlen(input) - 1] == '\n')
			input[strlen(input) - 1] = '\0';

		//checking the input
		token = strtok(input, " ");
		//OPTION NUMBER 1 : SHOW COURSES COMMAND
		if (strcmp(token, "ShowCourses") == 0) {
			pid = fork();
			if (pid == -1) { //error
				perror("Fork failure.\n");
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./ShowCourses", "./ShowCourses", NULL);
				perror("ShowCourses execution failure.\n");
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0)
					continue;
				else
					exit(-1);
			}
		}

		//OPTION NUMBER 2 : GET NEW COURSE COMMAND
		else if (strcmp(token, "GetNewCourse") == 0) {
			pid = fork();
			if (pid == -1) { //error
				perror("Fork failure.\n");
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./GetNewCourse", "./GetNewCourse", NULL);
				perror("GetNewCourse execution failure.\n");
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0)
					continue;
				else
					exit(-1);
			}
		}

		//OPTION NUMBER 3 : MAKE SCHEDULE COMMAND
		else if (strcmp(token, "MakeSchedule") == 0) {
			if (accessCode == 1) { //has access to directory
				pid = fork();
				if (pid == -1) { //error
					perror("Fork failure.\n");
					exit(-1);
				}
				else if (pid == 0) { //child process
					execlp("./MakeSchedule", "./MakeSchedule", argv[2], NULL); //argv[2] has the id of the student who logged in
					perror("MakeSchedule execution failure.\n");
					exit(-1);
				}
				else { //parent process
					waitpid(pid, &exitCode, 0);
					if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0) {
						continue;
					}
					else
						exit(-1);
				}
			}
			else if (accessCode == 2) { //deleted student
				printf("Student does not exist!\n");
			}
			else { //blocked student
				printf("Blocked Account! permission denied.\n");
			}
		}

		//OPTION NUMBER 4 : GET SCHEDULE COMMAND
		else if (strcmp(token, "GetSchedule") == 0) {
			if (accessCode == 1) { //has access to directory
				pid = fork();
				if (pid == -1) { //error
					perror("Fork failure.\n");
					exit(-1);
				}
				else if (pid == 0) { //child process
					execlp("./GetSchedule", "./GetSchedule", argv[2], "0", NULL); //sending the id, and flag 0
					perror("GetSchedule execution failure.\n");
					exit(-1);
				}
				else { //parent process
					waitpid(pid, &exitCode, 0);
					if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0)
						continue;
					else
						exit(-1);
				}
			}
			else if (accessCode == 2) { //deleted student
				printf("Student does not exist!\n");
			}
			else { //blocked student
				printf("Blocked Account! permission denied.\n");
			}
		}

		//OPTION NUMBER 5 : LOG OUT COMMAND
		else if (strcmp(token, "LogOut") == 0) {
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./LogOut", "./LogOut", NULL);
				write(2, "LogOut execution failure.\n", strlen("LogOut execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				exit(0);
			}
		}

		//OTHERWISE: NOT SUPPORTED
		else {
			printf("Not Supported\n");
		}
	}		

}
