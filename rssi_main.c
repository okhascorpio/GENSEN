/*
okhascorpio@gmail.com
2009 University of Vaasa FInland
*/

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <signal.h>

#include "port.h"

void printCommands();
int readCommand();
void startMeas();

void writeMeas(char *buffer);

void readHandler();

FILE *fp;
short measCount = 0;

int main(int argv, const char **argc)
{

	int command, i;
	char *buffer;
	char device;
	short measActive = 0;
	short sleepMode = 0;
	char *file_name, *ptr;
	
	device = 0;
	buffer = malloc(50 * sizeof(char));
	file_name = malloc(20 * sizeof(char));
	
	/*Catch I/O*/
	signal(SIGIO, readHandler);
	
	sprintf(file_name,"test.txt");
	
	port_open(&device);
	
	fp = 0;
	
	while(1)
	{
		if(measActive)
		{
			if(measCount < 50)
			{
				/*Wait until measurements are done*/
				int again = 1;
				while(again)
				{
					if(sleep(2) == 0)
					{
						if(measCount < 50)
						{
							again = 1;
						}
						else
						{
							again = 0;
						}
					}
				}
				
				measActive = 0;
			}
		}
		/*Print menu*/
		printCommands();
		command = readCommand();

		switch(command)
		{
			case 1: /*Increase Transmit power*/
			{
				port_write("i\r\n", 3);
				break;
			}
			case 2: /*Decrease transmit power*/
			{
				port_write("d\r\n", 3);
				break;
			}
			case 3:
			{
				measActive = 1;
				measCount = 0;
				fputs("Measurements started, please wait!", stdout);
				fp = fopen(file_name, "a+");
				port_write("r\r\n", 3);
				break;
			}
			case 4:
			{
				file_name = malloc(20 * sizeof(char));
				/*Choose file*/
				fputs("Give file name: ", stdout);
				
				scanf("%s", file_name);
				break;
			}
			case 5:
			{
				/*Close the file such that measuremnts are saved*/
				
				if(fp != 0)
				{
					fclose(fp);
				}
				port_close();
				return (0);
			}
		}
	}



}

/*Prints available commands*/
void printCommands()
{
	fputs("\r\n----------------------------------------\r\n", stdout);
	fputs("Commands availables\r\n\r\n", stdout);
	fputs("1. Increase power\r\n", stdout);
	fputs("2. Decrease power\r\n", stdout);
	fputs("3. Start measurements\r\n", stdout);
	fputs("4. Choose file\r\n", stdout);
	fputs("5. Quit\r\n", stdout);
	fputs("----------------------------------------\r\n\r\n", stdout);
}

/*Read command*/
int readCommand()
{
	char *cRead;
	int command = 0;

	cRead = malloc(10 * sizeof(char));
	fputs("Give command, 1-5: ", stdout);
	
	do
	{
		fgets(cRead, 2, stdin);
		command = atoi(cRead);		
	}
	while(command == 0);

	return command;
}

void startMeas()
{
	
	
}


void writeMeas(char *buffer)
{	
	fprintf(fp, "%s", buffer);
	fprintf(fp, " ");
}



	
void readHandler()
{
	
	char *data_buf;
	data_buf = malloc(30 * sizeof(char));
	port_read(data_buf, 10);
	
	if(strstr(data_buf, "start") != NULL)
	{
	
			return;
	}
	if(strstr(data_buf, "power") != NULL)
	{
		fputs(data_buf, stdout);
		return;
	}
	if(strstr(data_buf, "Tries") != NULL)
	{
		if(measCount < 50)
		{
			int i;
			fputs("meas < 50 ", stdout);
			for(i=measCount; i<50;i++)
			{
				fprintf(fp, "0 ");
			}
		}
			writeMeas(data_buf + 7);
			measCount = 51;
			return;
	}
	measCount++;
	writeMeas(data_buf);
	free(data_buf);
}
