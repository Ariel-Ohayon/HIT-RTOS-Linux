#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#define Port 9002

// --- My Functions --- //
void  checkForFile     	   (char str[]);
float HourCalc         	   (char str[]);
void  printToFile      	   (char str[]);
int   FileNumberLines      (char filename[]);
float collectDataForSalary (int client_socket);
// --- My Functions --- //


int main (int argc, char *argv[])
{
	int server_socket = socket(AF_INET, SOCK_STREAM, 0), client_socket;
	int pid = fork();
	if (pid != 0)	//Parent Process.
	{
		int x = 1;
		kill(pid, SIGSTOP);	//send signal to child process for stop.
		printf ("Enter 0 to open the server.\n");
		while (x)
		{
			scanf("%d",&x);
		}
		x = 1;
//		system("clear");
		kill(pid,SIGCONT);	// send signal to child process for continue.
		printf ("Enter 0 to close the server.\n");
		while (x)
		{
			scanf("%d",&x);	// wait for get 0 from the user in the server to close the server.
		}
		kill(pid,SIGKILL);	// Terminating from child process.
		close(server_socket);
//		system("clear");
		return 0;
	}
	else	//Child Process.
	{
		FILE *f;
		float sal;
		int val, i;
		char parm[100];
		if (server_socket == -1)
		{
			perror("Failed open socket.");
			exit(1);
		}

		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_port = htons(Port);	// htons() - (host to network short) define port.
		address.sin_addr.s_addr = INADDR_ANY;
		if (bind(server_socket, (struct sockaddr*) &address, sizeof(address) ) == -1)
		{
			perror("Failed to bind socket and address.");
			exit(1);
		}
		while (1)
		{
			printf ("Wait for connection...\n");
			if (listen(server_socket,1) == -1)	//1 client can connect to the server every time.
			{
				perror("Failed for listen.");
				exit(1);
			}
			if( (client_socket = accept(server_socket, NULL,NULL)) == -1)	//
			{
				perror("Failed for accept.");
				exit(1);
			}
			printf("There is a connection!\n");
			if (recv( client_socket, &parm, sizeof(parm), 0 ) == -1)
			{
				perror("Failed to recieve.");
				exit(1);
			}
			if (strcmp(parm,"Teacher") == 0)
			{
				printf ("Teacher get into the server.\n");
				printf("Collecting data from Teacher ...\n");
				recv(client_socket , &parm, sizeof(parm), 0 );
				printf ("The Data: %s\n", parm);
				printToFile(parm);
			}
			if ( strcmp(parm , "Admin") == 0 )
			{
				printf ("Admin get into the server.\n");
				printf ("Admin choose mode of operation.\n");
				if(recv(client_socket, &val, sizeof(val), 0) == -1)
				{
					perror("Failed to recieve.");
					exit(1);
				}
				if (val == 1)
				{
					printf ("Admin want to calculate the salary of ");
					printf ("specific worker.\n");
					printf ("Get data from the admin.\n");
					if ( (sal = collectDataForSalary(client_socket)) != -1)
					{
						sprintf(parm,"Salary: %.3f",sal);
					}
					else
					{
						strcpy(parm, "File not found.\n");
					}
					printf ("%s\n",parm);
					if(send(client_socket,parm,sizeof(parm), 0) == -1)
					{
						perror("send error ");
						exit(1);
					}
				}
				else
				{
					printf ("Admin want information about specific month.\n");
					printf ("Get the file name from the admin.\n");
					if (recv(client_socket, &parm, sizeof(parm), 0 ) == -1)
					{
						perror("failed to recieve.");
						exit(1);
					}
					if ( (val = FileNumberLines(parm)) == -1 )
					{
						if (send(client_socket, (char*) &val,sizeof(val),0) == -1)
						{
							perror("Failed to send.");
							exit(1);
						}
						printf("File not Found.\n");
					}
					else
					{
						if ( send ( client_socket, (char*) &val, sizeof(val), 0 ) == -1)
						{
							perror ("Failed to send.");
							exit(1);
						}
						f = fopen(parm,"r");
						for (i = 0; i < val; i++)
						{
							fgets(parm,100,f);
							if(send(client_socket, parm, sizeof(parm), 0 ) == -1)
							{
								perror("Failed to send.");
								exit(1);
							}
						}
						fclose(f);
					}
				}
			}
		}
	}
	return 0;
}


float collectDataForSalary (int client_socket)
{
	FILE *f;
	int i;
	float sum = 0;
	char data[100], m[3], y[5], id[10], idcmp[10],filename[50];
	recv (client_socket, &data,  sizeof (data),  0 );
	printf ("the data we collect from admin: %s\n", data);
	for (i = 0; i < 17; i++)
	{
		if (i < 2)
		{
			m[i] = data[i];
		}
		if (i < 7)
		{
			y[i] = data[i+3];
		}

		id[i] = data[i+8];
	}
	m[2] = NULL;
	y[4] = NULL;
	id[9] = NULL;

	filename[0] = NULL;
	strcat(filename, m);
	strcat(filename, ":");
	strcat(filename, y);
	strcat(filename, ".txt");

	int j=0;
	if ( (f = fopen (filename, "r"))  == NULL)
	{
		return -1;
	}

	fgets(data,100,f);
	printf("%s",data);
	while (fgets(data,100,f) != NULL)
	{
		printf("%s",data);
		data[9] = NULL;
		strcpy (idcmp, data);
		if ( strcmp(id,idcmp) == 0 )
		{
			data[9] = " ";
			sum += HourCalc(data);
		}
	}
	fclose(f);
	sum = sum * 50;		// 50 ILS per Hour
	return sum;
}


int FileNumberLines (char filename[])	//function for calculate the number of lines in the file.
{
	FILE *f = fopen (filename,"r");
	if (f == NULL)		// file does not exist
	{
		return -1;	// return -1 (error)
	}
	int out = 0;
	char str[200];
	while ( fgets(str,200,f) != NULL )
	{
		out++;
	}
	fclose(f);
	return out;
}

void printToFile(char str[])		//this function update the file with
					//new data that we get from Teacher.
{
	FILE *f;
	char filename[8],i, id[9];
	for (i = 0; i < 7; i++)	//the loop organize the name of the file name.
	{
		filename[i] = str[i + 13];
		if (filename[i] == 0x2F || filename[i] == 0x2E)	//0x2F = "/" , 0x2E = "."
		{
			filename[i] = 0x3A;	//0x3A = ":"
		}
	}
	filename[7] = NULL;
	strcat(filename,".txt");

	checkForFile(filename);	//check if the file exist.

	f = fopen (filename,"a");	//append the file.
	for (i=0;i<9;i++)
	{
		id[i] = str[i];
	}
	id[9] = NULL;
	fprintf(f,"%s\n",str);
	fclose(f);

}

void checkForFile(char filename[])	//this function check if file exist.
{
	FILE *f;
	if ( fopen (filename, "r") == NULL )	//if the file does not exist
	{
		f = fopen( filename, "w" );	//create it
		fprintf(f,"%-10s%-11s%s\n","ID","Date","Hours");
		fclose(f);
	}
}

float HourCalc (char str[])			// calculate the total hours that specific worker work.
{
	float result = 0, i;
	float x, xf, y, yf;

	x  = (str[21] - 0x30) * 10;	//calculate the hours.
	x += (str[22] - 0x30);
	y  = (str[27] - 0x30) * 10;
	y += (str[28] - 0x30);


	xf  = (str[24] - 0x30) * 10;	//calculate the minutes.
	xf += (str[25] - 0x30);
	yf  = (str[30] - 0x30) * 10;
	yf += (str[31] - 0x30);

	// to get the precentege in the hour we need to divide by 60
	xf = (xf/60);
	yf = (yf/60);

	x += xf;
	y += yf;

	result = y - x;
	return result;
}
