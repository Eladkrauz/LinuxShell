//LEC SHELL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[])
{
	int stdPasswords, rbytes, count = 0, exitCode;
	char buff[9], ch, *newFolder, input[MAX], *token;
	char* courseCode, *courseName, *day, *startHour, *endHour, *studentIdentification;
	pid_t pid;

	//Now starting the shell action >> getting inputs from the user
	while (1) {
		write(1, "LecShell> ", 10);
		memset(input, 0, MAX);
		if (read(0, input, MAX) == -1) {
			write(2, "Reading failure\n", strlen("Reading failure\n"));
			exit(-1);
		}
		if (strlen(input) > 0 && input[strlen(input) - 1] == '\n')
			input[strlen(input) - 1] = '\0';

		//checking the inputs
		token = strtok(input, " ");
		//OPTION NUMBER 1 : SET COURSES COMMAND
		if (strcmp(token, "SetCourses") == 0) {
			courseCode = strtok(NULL, " ");
			courseName = strtok(NULL, " ");
			day = strtok(NULL, " ");
			startHour = strtok(NULL, " ");
			endHour = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./SetCourses", "./SetCourses", courseCode, courseName, day, startHour, endHour, NULL);
				write(2, "SetCourses execution failure.\n", strlen("SetCourses execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && (WEXITSTATUS(exitCode) == 0 || WEXITSTATUS(exitCode) == 2)) //ok or missing arguments
					continue;
				else //error
					exit(-1);
			}
		}

		//OPTION NUMBER 2 : GET SCHEDULE COMMAND
		else if (strcmp(token, "GetSchedule") == 0) {
			studentIdentification = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./GetSchedule", "./GetSchedule", studentIdentification, "1", NULL); //sending the id, and flag 1
				write(2, "GetSchedule executin failure.\n", strlen("GetSchedule executin failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && (WEXITSTATUS(exitCode) == 0 || WEXITSTATUS(exitCode) == 2)) //ok or missing arguments
					continue;
				else //error
					exit(-1);
			}
		}

		//OPTION NUMBER 3 : DELETE STUDENT COMMAND
		else if (strcmp(token, "DeleteStudent") == 0) {
			studentIdentification = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./DeleteStudent", "./DeleteStudent", studentIdentification, NULL);
				write(2, "DeleteStudent execution failure.\n", strlen("DeleteStudent execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && (WEXITSTATUS(exitCode) == 0 || WEXITSTATUS(exitCode) == 2)) //ok or missing arguments
					continue;
				else //error
					exit(-1);
			}
		}

		//OPTION NUMBER 4 : FREEZE COMMAND
		else if (strcmp(token, "Freeze") == 0) {
			studentIdentification = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./Freeze", "./Freeze", studentIdentification, NULL);
				write(2, "Freeze execution failure.\n", strlen("Freeze execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0) { //ok
					printf("Student %s account blocked!\n", studentIdentification);
					continue;
				}
				else if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 2) //missing arguments
					continue;
				else //error
					exit(-1);
			}
		}

		//OPTION NUMBER 5 : APPROVE COMMAND
		else if (strcmp(token, "Approve") == 0) {
			studentIdentification = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./Approve", "./Approve", studentIdentification, NULL);
				write(2, "Approve execution failure.\n", strlen("Approve execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0) { //ok
					printf("Student %s successfully approved\n", studentIdentification);
					continue;
				}
				else if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 2) //missing arguments
					continue;
				else
					exit(-1);
			}
		}

		//OPTION NUMBER 6 : SHOW COURSES COMMAND
		else if (strcmp(token, "ShowCourses") == 0) {
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./ShowCourses", "./ShowCourses", NULL);
				write(2, "ShowCourses execution failure.\n", strlen("ShowCourses execution failure.\n"));
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

		//OPTION NUMBER 7 : LOG OUT COMMAND
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
			write(1, "Not Supported\n", 14);
		}
	}		
}