//LOGIN STUDENT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX 256

void main(int argc, char* argv[])
{
	int stdPasswords, flag = 0, count = 0, rbytes = 1, exitCode;
	char name[MAX], password[MAX], ch, id[10];
	pid_t pid;

	//checking arguments
	if (argc < 3) {
        write(2, "LoginStudent: missing parameters!\n", strlen("LoginStudent: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

	//opening the std_pass file
	if ((stdPasswords = open("std_pass.txt", O_RDONLY)) == -1) { //in case of an error
		write(2, "Opening students file failure.\n", strlen("Opening lecturers file failure.\n"));
		exit(-1);
	}

	//moving to the second line of the students file
	while (1) {
		if (read(stdPasswords, &ch, 1) == -1) { //in case of an error
			write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
			exit(-1);
		}
		if (ch == '\n') //if arrived to the end of the first line
			break;
	}

	//now starting getting names and comparing them to the given name (at argv[1])
	//in every new line, counting how many characters the username has.
	//than reading it from the file (after allocating memory for the variable)
	//and comparing it with the given name. If there's a match >> continuing to check password
	//otherwise, finishing >> cause the student does not exist
	while (rbytes != 0) { //until file has not ended
		if ((rbytes = read(stdPasswords, &ch, 1)) == -1) { //in case of an error
			write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
			exit(-1);
		}
		else {
			if (ch != ' ') //if we are still in the username column
				count++;
			else { //a space >> username column ended
				//reading the next name from the file
				lseek(stdPasswords, (-1) * (count + 1), 1); //moving to the start of the line
				memset(name, 0, MAX);
				if ((rbytes = read(stdPasswords, name, count)) == -1) { //in case of an error
					write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
					exit(-1);
				}
				name[count] = '\0';
				if (strcmp(name, argv[1]) == 0) { //if the name from the file and the argument are the same
					flag = 1; //flag 1 means: student exists.
					break;
				}
				//if not, moving to the next line
				count = 0;
				while (rbytes != 0) {
					if ((rbytes = read(stdPasswords, &ch, 1)) == -1) { //in case of an error
						write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
						exit(-1);
					}
					if (ch == '\n') //if arrived to the end of the line
						break;
				}
			}
		}
	}

	if (flag == 0) { //in case the student does not found in the file
		printf("Student Not Found!\n");
		close(stdPasswords);
		exit(0);
	}
	else { //if exists, starting to check the password
		//skipping spaces after name and before the password
		while (1) {
			if ((rbytes = read(stdPasswords, &ch, 1)) == -1) {
				write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
			}
			if (ch != ' ') { //found a char that is not a space between columns
				lseek(stdPasswords, -1, 1); //taking one step back
				break;
			}
		}
		
		//checking the password
		count = 0;
		while (rbytes != 0) { //until file has not ended
			if ((rbytes = read(stdPasswords, &ch, 1)) == -1) { //in case of an error
				write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
			}
			else {
				if (ch != ' ')
					count++;
				else { //a space >> column ended
					//reading the password from the file
					lseek(stdPasswords, (-1) * (count + 1), 1);
					if ((rbytes = read(stdPasswords, password, count)) == -1) { //in case of an error
						write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
						exit(-1);
					}
					password[count] = '\0';
					if (strcmp(password, argv[2]) == 0) {
						flag = 1;
						break;
					}
					else { //if not, incorrect password
						flag = 0;
						break;					
					}
				}
			}
		}

		//getting the id of the student
		while (1) {
			if ((rbytes = read(stdPasswords, &ch, 1)) == -1) { //in case of an error
				write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
			}
			if (ch == ' ') //
				continue;
			else {
				lseek(stdPasswords, -1, 1);
				break;
			}
		}
		//reading the id
		if ((rbytes = read(stdPasswords, id, 9)) == -1) { //in case of an error
				write(2, "Reading students file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
		}
		id[9] = '\0';

		if (flag == 0) { //in case the password is incorrect
			exit(1);
		}
		else { //if the password is correct
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./StdShell", "./StdShell", name, id, (char*)NULL); //sending the name and id as parameters
				write(2, "StdShell execution failure.\n", strlen("StdShell execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				close(stdPasswords);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0)
					exit(0);
				else
					exit(-1);
			}
		}
	}
}
