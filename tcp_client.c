#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define Port 9002

int main (int argc, char *argv[])
{
//	system("clear");
	float sal;
	int i,val, client_socket;
	char str[100],x[50];
	if (argc != 2)
	{
		printf ("Error.\n");
		return -1;
	}
	if ( (strcmp (argv[1], "Teacher") != 0) && (strcmp (argv[1], "Admin") != 0) )
	{
		printf("You have entered wrong arguments.\n");
		return -1;
	}
	if ( (client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Failed to open socket.");
		exit(1);
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(Port);
	address.sin_addr.s_addr = INADDR_ANY;
	if (connect ( client_socket, (struct sockaddr*) &address,sizeof(address) ) == -1)
	{
		perror("Failed to connect the server.");
		exit(1);
	}
	if ( send( client_socket,argv[1], sizeof(argv[1]),0 ) == -1)
	{
		perror("Failed to send.");
		exit(1);
	}
	if (strcmp(argv[1], "Teacher") == 0)
	{
//		system ("clear");
		printf ("Hello Teacher.\n");
		printf ("Enter ID: ");
		scanf ("%s",&str);
		strcat(str," ");
		printf ("Enter Date: ");
		scanf("%s",&x);
		strcat(str,x);
		strcat(str," ");
		printf("Enter Hour: ");
		scanf("%s",x);
		strcat(str,x);
		if ( send(client_socket, str, sizeof(str), 0 ) == -1)
		{
			perror("Failed to send.");
			exit(1);
		}
		printf("send: %s\n",str);
	}
	else if ( strcmp (argv[1], "Admin") == 0 )
	{
//		system ("clear");
		printf ("Hello Admin.\nWhat operation you want to do?\n");
		printf ("1. Calculate the salary of specific worker in specific month.\n");
		printf ("2. Get information from the server about a specific month.\n");
		scanf("%d",&val);
		if ( send(client_socket, (char *) &val, sizeof(val), 0) == -1)
		{
			perror("Failed to send.");
			exit(1);
		}
		if (val == 1)
		{
			printf ("Enter Month: ");
			scanf  ("%s",&str);
			strcat (str," ");
			printf ("Enter Year: ");
			scanf  ("%s",&x);
			strcat (str, x);
			strcat (str, " ");
			printf ("Enter ID: ");
			scanf  ("%s",&x);
			strcat (str, x);
			if ( send   ( client_socket,  str, sizeof(str), 0 ) == -1)	//send ID
			{
				perror("Failed to send.");
				exit(1);
			}
			if ( recv   ( client_socket, &str, sizeof(str), 0 ) == -1)
			{
				perror("Failed to recieve.");
				exit(1);
			}
			printf ("%s\n",str);
		}
		else
		{
			printf ("Enter file name for read: ");
			scanf("%s",&str);
			if ( send(client_socket, str,  sizeof(str), 0 ) == -1)
			{
				perror ("Failed to send.");
				exit(1);
			}
			if ( recv(client_socket, &val, sizeof(val), 0 ) == -1)
			// in val we get the number of lines we have in the file that we want to send to the client.
			// if we get -1 in val the file with this name is not exist in the server.
			{
				perror("Failed to recieve.");
				exit(1);
			}
			if (val != -1)	// the file exist.
			{
				for (i = 0; i < val; i++)
				{
					if( recv(client_socket, &str, sizeof(str), 0) == -1 )
					{
						perror ("Failed to recieve.");
						exit(1);
					}
					printf("%s",str);
				}
			}
			else		// the file not exist.
			{
				printf("File not Found.\n");
			}
		}
	}
	close(client_socket);
	return 0;
}
