#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "sys/wait.h"
#include "fcntl.h"
#include <string.h>

#define BUFFER 512


int read_line(char* line)
{
	int buffer_size = BUFFER;
	int c;
	int i = 0;

	while(1)
	{
		c = getchar();

		if(c==EOF || c=='\n')
		{
			line[i] = '\0';
			return EXIT_SUCCESS;
		}
		line[i++] = c;

		// Since we dont know how much memory would be needed for line, we keep on reallocating more memory as and when it exceeds.
		if(i >= buffer_size)
        {
            buffer_size+=BUFFER;
            line = (char*)realloc(line, buffer_size);
            if(!line)
            {
                printf("Error: memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
	}
}

int split_line()
{
	
}
void command_loop()
{
	int status;
	char* line;

	do
		{
			/* code */
			line = (char *)malloc(sizeof(char)*BUFFER);
			status = read_line(line);
		} while (status==EXIT_SUCCESS);	
}

int main(int argc, char const *argv[])
{

	command_loop();
	return 0;
}