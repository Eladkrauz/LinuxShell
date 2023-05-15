//SHOW COURSES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 1024

void main(int argc, char* argv[])
{
	int courses, rbytes;
	char buff[MAX];
	if (access("./Courses/courses.txt", F_OK) != 0) { //courses file does not exists
		printf("No courses available.\n");
		exit(0);
	}
	if ((courses = open("./Courses/courses.txt", O_RDONLY)) == -1) {
		write(2, "Opening courses file failure.\n", strlen("Opening courses file failure.\n"));
		exit(-1);
	}
	
	while (1) { //writing the contents of the courses.txt file to stdout
		if ((rbytes = read(courses, buff, MAX - 1)) == -1) {
			write(2, "Reading courses failure.\n", strlen("Reading courses failure.\n"));
			exit(-1);
		}
		buff[rbytes] = '\0';
		write(1, buff, rbytes);
		if (rbytes == 0)
			break;
	}

	write(1, "\n\n", 2);
	close(courses);
	exit(0);
}
