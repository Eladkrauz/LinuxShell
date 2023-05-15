//SET COURSES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void main(int argc, char* argv[])
{
	int courses, spaces, i;
	if (argc != 6) {
        write(2, "SetCourses: missing parameters!\n", strlen("SetCourses: missing parameters!\n"));
		exit(2); //an error but not going to close the whole shell
	}

	//creating the file "courses.txt" in Courses directory (in case it doesn't exist)
	if ((courses = open("./Courses/courses.txt", O_WRONLY)) == -1) {
		if ((courses = open("./Courses/courses.txt", O_WRONLY | O_CREAT, 0777)) == -1) {
			write(2, "Opening courses file failure.\n", strlen("Opening courses file failure.\n"));
			exit(-1);
		}
		write(courses, "Code     Name                         Day          Start     End     ", 69);
		close(courses);
	}

	//now appending the file
	if ((courses = open("./Courses/courses.txt", O_WRONLY | O_APPEND)) == -1) {
		write(2, "Opening courses file failure.\n", strlen("Opening courses file failure.\n"));
		exit(-1);
	}
	
	//now writing the given arguments
	write(courses, "\n", 1);
	//code
	spaces = 9 - strlen(argv[1]);
	write(courses, argv[1], strlen(argv[1]));
	for (i = 0; i < spaces; i++)
		write(courses, " ", 1);
	//name
	spaces = 29 - strlen(argv[2]);
	write(courses, argv[2], strlen(argv[2]));
	for (i = 0; i < spaces; i++)
		write(courses, " ", 1);
	//day
	spaces = 13 - strlen(argv[3]);
	write(courses, argv[3], strlen(argv[3]));
	for (i = 0; i < spaces; i++)
		write(courses, " ", 1);
	//start hour
	spaces = 10 - strlen(argv[4]);
	write(courses, argv[4], strlen(argv[4]));
	for (i = 0; i < spaces; i++)
		write(courses, " ", 1);
	//end hour
	spaces = 8 - strlen(argv[5]);
	write(courses, argv[5], strlen(argv[5]));
	for (i = 0; i < spaces; i++)
		write(courses, " ", 1);

	close(courses);
	printf("Successfully created the course.\n");
	exit(0);
}
