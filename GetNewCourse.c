//GET NEW COURSE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX 256

void main(int argc, char* argv[]) {
	int courses, width = 0;
	char ch, buff[MAX];
	if (access("./Courses/courses.txt", F_OK) != 0) { //courses file does not exists
		printf("No courses available.\n");
		exit(0);
	}
	if ((courses = open("./Courses/courses.txt", O_RDONLY)) == -1) { //opening the courses text file
		write(2, "Opening courses file failure.\n", strlen("Opening courses file failure.\n"));
		exit(-1);
	}

	//if opened properly, checking the width of the table in the text file
	while (1) {
		if (read(courses, &ch, 1) == -1) {
			write(2, "Reading failure.\n", strlen("Reading failure.\n"));
			exit(-1);
		}
		width++;
		if (ch == '\n')
			break;
	}

	//now width has the width of the table, including '\n', so we'll decrease 1
	width -= 1;

	//first reading the title row, this is for printing purposes
	lseek(courses, 0, 0);
	if (read(courses, buff, width) == -1) {
		write(2, "Reading failure.\n", strlen("Reading failure.\n"));
		exit(-1);
	}
	//writing the title to stdout
	write(1, buff, width);
	write(1, "\n", 1);

	//now reading the last row of the table - the last course
	lseek(courses, (-1) * width, 2);
	if (read(courses, buff, width) == -1) {
		write(2, "Reading failure.\n", strlen("Reading failure.\n"));
		exit(-1);
	}

	//writing the course to stdout
	write(1, buff, width);
	write(1, "\n", 1);

	//done
	exit(0);
}