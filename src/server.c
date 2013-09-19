#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"
#include "program.h"

#define MAXBUFLEN 500
#define MAXCLIENTS 2

struct server_conf ambitv_server_conf;

int ambitv_server_init(int tcpport)
{
    int yes = 1;
	/* port we're listening on */
	int port;
	/* server address */
	struct sockaddr_in serveraddr;

	/* port we're listening on */
	port = tcpport;

	/* clear the master and temp sets */
	FD_ZERO(&ambitv_server_conf.master);

	/* get the listener */
	if((ambitv_server_conf.listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Server-socket() error lol!");
		/*just exit lol!*/
		return -1;
	}
	printf("Server-socket() is OK...\n");
	/*"address already in use" error message */
	if(setsockopt(ambitv_server_conf.listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("Server-setsockopt() error lol!");
		return -1;
	}
	printf("Server-setsockopt() is OK...\n");

	/* bind */
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(port);
	memset(&(serveraddr.sin_zero), '\0', 8);

	if(bind(ambitv_server_conf.listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		perror("Server-bind() error lol!");
		return -1;
	}
	printf("Server-bind() is OK...\n");

	/* listen */
	if(listen(ambitv_server_conf.listener, MAXCLIENTS) == -1)
	{
		perror("Server-listen() error lol!");
		return -1;
	}
	printf("Server-listen() is OK...\n");

	/* add the listener to the master set */
	FD_SET(ambitv_server_conf.listener, &ambitv_server_conf.master);
	/* keep track of the biggest file descriptor */
	ambitv_server_conf.fdmax = ambitv_server_conf.listener; /* so far, it's this one*/

	return 0;
}

void ambitv_server_run()
{
	/* newly accept()ed socket descriptor */
	int newfd;
	/* temp file descriptor list for select() */
	fd_set read_fds;
	/* client address */
	struct sockaddr_in clientaddr;
	/* buffer for client data */
	char buf[MAXBUFLEN];
	int nbytes;
	/* select timeout */
	struct timeval tvout;
	/* for setsockopt() SO_REUSEADDR, below */
	int addrlen;
	int i;

     /* select timeout */
     tvout.tv_sec = 0;
     tvout.tv_usec  = 500000;

	/* clear the master and temp sets */
	FD_ZERO(&read_fds);

	/* copy it */
	read_fds = ambitv_server_conf.master;

	if(select(ambitv_server_conf.fdmax+1, &read_fds, NULL, NULL, &tvout) == -1)
	{
		perror("Server-select() error lol!");
		//exit(1);
	}

	/*run through the existing connections looking for data to be read*/
	for(i = 0; i <= ambitv_server_conf.fdmax; i++)
	{
		if(FD_ISSET(i, &read_fds))
		{ /* we got one... */
			if(i == ambitv_server_conf.listener)
			{
				/* handle new connections */
				addrlen = sizeof(clientaddr);
				if((newfd = accept(ambitv_server_conf.listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
				{
					perror("Server-accept() error lol!");
				}
				else
				{
					printf("Server-accept() is OK...\n");

					FD_SET(newfd, &ambitv_server_conf.master); /* add to master set */
					if(newfd > ambitv_server_conf.fdmax)
					{ /* keep track of the maximum */
						ambitv_server_conf.fdmax = newfd;
					}
					printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
				}
			}
			else
			{
				/* handle data from a client */
				if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
				{
					/* got error or connection closed by client */
					if(nbytes == 0)
						/* connection closed */
						printf("socket %d hung up\n", i);

					else
						perror("recv() error lol!");

					/* close it... */
					close(i);
					/* remove from master set */
					FD_CLR(i, &ambitv_server_conf.master);
				}
				else
				{
					printf("Here is the message: %s\n",buf);
					close(i);
					FD_CLR(i, &ambitv_server_conf.master);
				}
			}
		}
	}

}
