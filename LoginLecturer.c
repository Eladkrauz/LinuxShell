//LOGIN LECTURER
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAX 256

void main(int argc, char* argv[])
{
	int lecPasswords, flag = 0, count = 0, rbytes = 1, exitCode;
	char name[MAX], password[MAX], ch;
	pid_t pid;

	//checking arguments
	if (argc < 3) {
        write(2, "LoginLecturer: missing parameters!\n", strlen("LoginLecturer: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

	//opening the lec_pass file
	if ((lecPasswords = open("lec_pass.txt", O_RDONLY)) == -1) { //in case of an error
		write(2, "Opening lecturers file failure.\n", strlen("Opening lecturers file failure.\n"));
		exit(-1);
	}

	//moving to the second line of the lecturers file
	while (1) {
		if (read(lecPasswords, &ch, 1) == -1) { //in case of an error
			write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
			exit(-1);
		}
		if (ch == '\n') //if arrived to the end of the first line
			break;
	}

	//now starting getting names and comparing them to the given name (at argv[1])
	//in every new line, counting how many characters the username has.
	//than reading it from the file (after allocating memory for the variable)
	//and comparing it with the given name. If there's a match >> continuing to check password
	//otherwise, finishing >> cause the lecturer does not exist
	while (rbytes != 0) { //until file has not ended
		if ((rbytes = read(lecPasswords, &ch, 1)) == -1) { //in case of an error
			write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
			exit(-1);
		}
		else {
			if (ch != ' ') //if we are still in the username column
				count++;
			else { //a space >> username column ended
				//reading the next name from the file
				lseek(lecPasswords, (-1) * (count + 1), 1); //moving to the start of the line
				memset(name, 0, MAX);
				if ((rbytes = read(lecPasswords, name, count)) == -1) { //in case of an error
					write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
					exit(-1);
				}
				name[count] = '\0';
				if (strcmp(name, argv[1]) == 0) { //if the name from the file and the argument are the same
					flag = 1; //flag 1 means: lecturer exists.
					break;
				}
				//if not, moving to the next line
				count = 0;
				while (rbytes != 0) {
					if ((rbytes = read(lecPasswords, &ch, 1)) == -1) { //in case of an error
						write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
						exit(-1);
					}
					if (ch == '\n') //if arrived to the end of the line
						break;
				}
			}
		}
	}

	if (flag == 0) { //in case the lecturer does not found in the file
		printf("Lecturer Not Found!\n");
		close(lecPasswords);
		exit(0);
	}

	else { //if exists, starting to check the password
		//skipping spaces after name and before the password
		while (1) {
			if ((rbytes = read(lecPasswords, &ch, 1)) == -1) {
				write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
			}
			if (ch != ' ') { //found a char that is not a space between columns
				lseek(lecPasswords, -1, 1); //taking one step back
				break;
			}
		}
		
		//checking the password
		count = 0;
		while (rbytes != 0) { //until file has not ended
			if ((rbytes = read(lecPasswords, &ch, 1)) == -1) { //in case of an error
				write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
				exit(-1);
			}
			else {
				if (ch != ' ')
					count++;
				else { //a space >> column ended
					//reading the password from the file
					lseek(lecPasswords, (-1) * (count + 1), 1);
					if ((rbytes = read(lecPasswords, password, count)) == -1) { //in case of an error
						write(2, "Reading lecturers file failure.\n", strlen("Reading lecturers file failure.\n"));
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

		if (flag == 0) { //in case the password is incorrect
			printf("Incorrect Password\n");
			exit(1);
		}
		else {
			pid_t pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				exit(-1);
			}
			else if (pid == 0) { //child process
				execlp("./LecShell", "./LecShell", (char*)NULL);
				write(2, "LecShell execution failure.\n", strlen("LecShell execution failure.\n"));
				exit(-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				close(lecPasswords);
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0)
					exit(0);
				else
					exit(-1);
			}
		}
	}
}