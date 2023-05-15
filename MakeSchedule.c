//MAKE SCHEDULE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[]) { //argv[1] has the id of the student
	int stdFile, courses, i, width = 0, rbytes, validCode = 0, codeExists, haveConfirmed = 0, getInput = 1, flag, exitCode;
	char ch, *directory, code[6], code2[6], input[MAX], buffer[MAX];
	pid_t pid;
	if (access("./Courses/courses.txt", F_OK) != 0) { //courses file does not exists
		printf("No courses available.\n");
		exit(0);
	}

	//checking if there's a schedule in the student's directory
	if ((directory = (char*)malloc(strlen("./student/") + 10 + strlen("/Schedule.txt"))) == NULL) {
		perror("Memory allocation failure.\n");
		exit(-1);
	}
	strcpy(directory, "./student/");
	strcat(directory, argv[1]);
	strcat(directory, "/Schedule.txt");
	if (access(directory, F_OK) == 0) { //if exists already
		printf("Schedule already exists\n");
		free(directory);
		exit(0);
	}

	//if does not exist, creating it
	if ((stdFile = open(directory, O_RDWR | O_CREAT | O_TRUNC, 0777)) == -1) {
		perror("Opening schedule file failure.\n");
		free(directory);
		exit(-1);
	}
	//if opened properly, checking the width of the table in the courses file in order to scan it
	if ((courses = open("./Courses/courses.txt", O_RDONLY)) == -1) {
		perror("Opening courses file failure.\n");
		free(directory);
		exit(-1);
	}
	while (1) {
		if (read(courses, &ch, 1) == -1) {
			perror("Reading failure.\n");
			free(directory);
			exit(-1);
		}
		if (ch == '\n')
			break;
		width++; // width has the width of the table, without \n
	}

	//writing the title row of the courses file onto the schedule file
	lseek(courses, 0, 0);
	memset(buffer, '\0', MAX);
	if (read(courses, buffer, width) == -1) {
		perror("Reading failure.\n");
		free(directory);
		exit(-1);
	}
	write(stdFile, buffer, width);

	//NOW STARTING THE ACTION OF THE COMMAND
	//keeps getting input until "Exit" is provided
	printf("Insert course ID:\n");
	while (1) {
		if (getInput == 1) {
			scanf("%s", input);
			if (strcmp(input, "Exit") == 0)
				break;
			if (strcmp(input, "Confirm") == 0) {
				printf("Invalid input.\n");
				getInput = 1;
				continue;
			}
		}
		//checking if the code is valid: valid code = 1, invalid code = 0
		validCode = (strlen(input) == 5);
		for (i = 0; i < strlen(input); i++) {
			if (input[i] < '0' || input[i] > '9') { //not a digit
				validCode = 0;
				break;
			}
		}

		//if the code's valid, checking if the code exists in the courses file
		//if exists, codeExists will change to 1, otherwise will stay 0
		lseek(courses, width + 1, 0); // +1 is to include the \n char
		codeExists = 0;
		while (validCode == 1) { //reading code from the courses file
			if ((rbytes = read(courses, code, 5)) == -1) {
				perror("Reading course code failure.\n");
				free(directory);
				exit(-1);
			}
			code[5] = '\0';
			if (strcmp(input, code) == 0) { //found it
				codeExists = 1;
				break;
			}
			else { //the current comparison returned false
				//moving to the next line
				lseek(courses, -5, 1); //to the start of the line
				lseek(courses, width, 1); //to the end of the line
				//now trying to read the next line. If read will get the '\n', it means
				//that there is a next line. Otherwise, read will return 0 >>> EOF
				if ((rbytes = read(courses, &ch, 1)) == -1) {
					perror("Reading courses file failure.\n");
					free(directory);
					exit(-1);
				}
				if (rbytes == 0) { //nothing left to read from the courses file
					break;
				}
				else { //keeps reading the next line
					continue;
				}
			}
		}

		if (codeExists == 0 && validCode == 0) { //code provided does not exist
			printf("Invalid input.\n");
			getInput = 1;
			continue;
		}
		else { //codeExists = 1 >> found the code and prints the course onto stdout
			lseek(courses, -5, 1); //move to the start of the line
			if (read(courses, code2, 5) == -1) {
				perror("Reading course info failure.\n");
				free(directory);
				exit(-1);
			}
			lseek(courses, -5, 1); //move to the start of the line
			//reading the info of the course
			memset(buffer, '\0', MAX);
			if (read(courses, buffer, width) == -1) {
				perror("Reading course info failure.\n");
				free(directory);
				exit(-1);
			}
			//printing the course to stdout
			printf("%s\n", buffer);
			//now getting action from user
			scanf("%s", input);
			if (strcmp(input, "Confirm") == 0) { //adding the course
				//first checking if already registerd
				lseek(stdFile, width, 0);
				flag = 0;
				while (1) {
					if ((rbytes = read(stdFile, &ch, 1)) == -1) {
						write(2, "Reading schedule file failure.\n", strlen("Reading schedule file failure.\n"));
						free(directory);
						exit(-1);
					}
					if (rbytes == 0) {
						flag = 0;
						break;
					}
					if (ch == '\n') {
						if (read(stdFile, code, 5) == -1) {
							write(2, "Reading schedule file failure.\n", strlen("Reading schedule file failure.\n"));
							free(directory);
							exit(-1);
						}
						if (strcmp(code, code2) == 0) {
							flag = 1;
							break;
						}
						else
							continue;
					}
				}
				lseek(stdFile, 0, 2);
				if (flag == 1) { //already exists in the student's schedule file
					printf("Student is already registerd to this course.\n");
				}
				else {
					write(stdFile, "\n", 1);
					write(stdFile, buffer, width);
					haveConfirmed = 1; //so if Exit is entered - won't remove the schedule file
				}
				getInput = 1; //get
				continue;
			}
			else if (strcmp(input, "Exit") == 0) {
				break;
			}
			else {
				getInput = 0; //don't get
				continue;
			}
		}
	}
	//checking if the stdFile contains at least one course.
	//if not, removing it from the student's directory
	if (haveConfirmed == 0) {
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
	else {
		close(stdFile);
	}
	close(courses);
	free(directory);
	exit(0);
}