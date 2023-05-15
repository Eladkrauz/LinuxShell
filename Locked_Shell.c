//LOCKED SHELL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define MAX 256
#define ATTEMPTS 3

typedef struct Node {
	char name[MAX];
	int wrongAttempts;
	struct Node *next, *prev;
} Node;

int main(int argc, char* argv[])
{
	int stdPasswords, rbytes, flag, count = 0, exitCode;
	char ch, *newFolder, buffer[MAX], *token, *username, *password;
	Node *head = NULL, *temp, *node;
	pid_t pid, pid2;

	//creating a new directory called "Courses" and "student"
	if (mkdir("Courses", 0777) == -1 && errno != EEXIST) {
		write(2, "Opening directories failure.\n", strlen("Opening directories failure.\n"));
		return (-1);
	}
	if (mkdir("student", 0777) == -1 && errno != EEXIST) {
		write(2, "Opening directories failure.\n", strlen("Opening directories failure.\n"));
		return (-1);
	}
	//opening the std_pass text file for creating folders for each student
	if ((stdPasswords = open("std_pass.txt", O_RDONLY)) == -1) {
		write(2, "Opening students file failure.\n", strlen("Opening students file failure.\n"));
		return (-1);
	}
	//first moving to the second line (the first line is the title of the table - not relevant)
	while (read(stdPasswords, &ch, 1) == 1)
	{
		if (ch == '\n')
			break;
	}
	//now reading each student
	while (read(stdPasswords, &ch, 1) == 1)
	{
		if (ch == ' ') //moving from one column of the table in the text file - to the next column
		{
			count++;
			while(ch == ' ')
			{
				if (read(stdPasswords, &ch, 1) != 1)
				{
					write(2, "Reading failure.\n", strlen("Reading failure.\n"));
					return (-1);
				}
			}
			lseek(stdPasswords, -1, 1);
		}
		if (count == 2) //column no.2 is the ID column >> getting the ID
		{
			if (read(stdPasswords, buffer, 9) != 9)
			{
				write(2, "Reading failure.\n", strlen("Reading failure.\n"));
				return (-1);
			}
			buffer[9] = '\0';
			if ((newFolder = (char*)malloc(strlen("./student/") + 10)) == NULL)
			{
				write(2, "memory allocation failure\n", strlen("memory allocation failure\n"));
				return (-1);
			}
			strcpy(newFolder, "./student/");
			strcat(newFolder, buffer);
			newFolder[strlen("./student/") + 9] = '\0';
			if (mkdir(newFolder, 0777) == -1 && errno != EEXIST) {
				write(2, "Opening directories failure.\n", strlen("Opening directories failure.\n"));
				return (-1);
			}
		}
		if (ch == '\n') //a new line means it is the next student
			count = 0;
	}
	close(stdPasswords);

	//Now starting the shell's action >> getting inputs from the user
	while (1) {
		printf("LockShell> ");
		memset(buffer, 0, MAX); //clearing the buffer
		fgets(buffer, MAX, stdin);
		if (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = '\0';

		//checking the inputs
		token = strtok(buffer, " ");
		//OPTION NUMBER 1 : LOGIN STUDENT COMMAND
		if (strcmp(token, "LoginStudent") == 0) {
			username = strtok(NULL, " ");
			password = strtok(NULL, " ");
			//adding the name to the linked list.
			//It is done to keep track on how many times the same student entered wrong passwords
			if (head == NULL) { //if the list's empty
				if ((head = (Node*)malloc(sizeof(Node))) == NULL) {
					write(2, "memory allocation failure\n", strlen("memory allocation failure\n"));
					return (-1);
				}
				strcpy(head->name, username);
				head->wrongAttempts = 0;
				head->next = NULL;
				head->prev = NULL;
			}
			else { //checking if the student is already in the list. if not - adding him
				temp = head;
				flag = 0; //to check if the name exists
				while (temp != NULL) {
					if (strcmp(temp->name, username) == 0) { //exists
						flag = 1;
						break;
					}
					temp = temp->next;					
				}
				if (flag == 0) { //does not exist, adding it to the head
					if ((node = (Node*)malloc(sizeof(Node))) == NULL) {
						write(2, "memory allocation failure\n", strlen("memory allocation failure\n"));
						return (-1);
					}
					strcpy(node->name, username);
					node->wrongAttempts = 0;
					node->next = head;
					node->prev = NULL;
					head->prev = node;
					head = node;
				}
			}

			//now calling the process with the given arguments
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				return (-1);
			}
			else if (pid == 0) { //child process
				execlp("./LoginStudent", "./LoginStudent", username, password, NULL);
				write(2, "LoginStudent execution failure.\n", strlen("LoginStudent execution failure.\n"));
				return (-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				//the child process will return:
				//2 if missing parameters
				//1 if the password is incorrect
				//0 if correct or if the name does not exist in the students file.
				if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 1) { //adding 1 to the wrong password attempts of the student
					//searching the student
					temp = head;
					while (temp != NULL) {
						if (strcmp(temp->name, username) == 0)
							break;
						temp = temp->next;
					}
					temp->wrongAttempts++;
					if (temp->wrongAttempts >= ATTEMPTS) {
					//BLOCKING THE STUDENT USING FREEZE PROGRAM
						pid2 = fork();
						if (pid2 == -1) { //error
							write(2, "Fork failure.\n", strlen("Fork failure.\n"));
							return (-1);
						}
						else if (pid2 == 0) { //child process
							execlp("./Freeze", "./Freeze", username, NULL);
							write(2, "Freeze execution failure.\n", strlen("Freeze execution failure.\n"));
							return (-1);
						}
						else { //parent process
							waitpid(pid2, &exitCode, 0);
							if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0) {
								printf("%s account Blocked!\n", username);
								continue;
							}
							else {
								return (-1);
							}
						}
					}
					else {
						printf("Incorrect Password\n");
					}
				}
				else if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 0) { //otherwise, removing the name from the list
					temp = head;
					while (temp != NULL) {
						if (strcmp(temp->name, username) == 0) {
							break;
						}
						temp = temp->next;
					}
					//freeing the node
					if (temp->prev != NULL)
						temp->prev->next = temp->next;
					else { //it is the head
						head = head->next;
						if (head != NULL) //list's empty?
							head->prev = NULL;
					}
					if (temp->next != NULL)
						temp->next->prev = temp->prev;
					free(temp);
				}
				else if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 2) { //missing parameters
					continue;
				}
				else { //finished with an error
					return(-1);
				}
			}
		}

		//OPTION NUMBER 2 : LOGIN LECTURER COMMAND
		else if (strcmp(token, "LoginLecturer") == 0) {
			username = strtok(NULL, " ");
			password = strtok(NULL, " ");
			pid = fork();
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				return (-1);
			}
			else if (pid == 0) { //child process
				execlp("./LoginLecturer", "./LoginLecturer", username, password, NULL);
				write(2, "LoginLecturer execution failure.\n", strlen("LoginLecturer execution failure.\n"));
				return (-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				//0 >> okay, 1 >> incorrect password
				if (WIFEXITED(exitCode) && (WEXITSTATUS(exitCode) == 0 || WEXITSTATUS(exitCode) == 1)) //ok or incorrect password
					continue;
				else if (WIFEXITED(exitCode) && WEXITSTATUS(exitCode) == 2) //missing paraameters
					continue;
				else
					return (-1);
			}
		}

		//OPTION NUMBER 3 : EXIT COMMAND
		else if (strcmp(token, "exit") == 0) {
			if (pid == -1) { //error
				write(2, "Fork failure.\n", strlen("Fork failure.\n"));
				return (-1);
			}
			else if (pid == 0) { //child process
				execlp("./exit", "./exit", NULL);
				write(2, "Exit execution failure.\n", strlen("Exit execution failure.\n"));
				return (-1);
			}
			else { //parent process
				waitpid(pid, &exitCode, 0);
				return 0;
			}
		}

		//OTHERWISE: NOT SUPPORTED
		else {
			printf("Not Supported\n");
		}
	}
}
