/*
** Initializes socket depending on the value of mode which
** determines if the socket has to be initialized for a server
** or client.
**
** mode = 1, server
** mode = 2, client
**
**
*/
 
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int initSock(int mode, char* host, char* port, struct addrinfo* servinfo)
{
	int sockfd, new_fd;  
	struct addrinfo hints, *p;
	struct sockaddr_storage their_addr; 
	int yes=1;
	int rv;
   
    if (host != NULL) { printf(">> initSock: host "); puts(host); }
    // y u no work alright!!!

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

    if(mode == 1) {
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		    return -1;
	    }

    	for(p = servinfo; p != NULL; p = p->ai_next) {
	    	if ((sockfd = socket(p->ai_family, p->ai_socktype,
		    		p->ai_protocol)) == -1) {
			    perror("server: socket");
			    continue;
		    }

		    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			    	sizeof(int)) == -1) {
			    perror("setsockopt");
			    exit(1);
		    }

		    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			    close(sockfd);
			    perror("server: bind");
			    continue;
		    }

		    break;
	    }

	    if (p == NULL)  {
		    fprintf(stderr, "server: failed to bind\n");
		    return -1;
	    }
        
        return sockfd; // return socket file descriptor
    }

    else 
    if(mode == 2) {
	    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		    return 1;
	    }
	    for(p = servinfo; p != NULL; p = p->ai_next) {
		    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			    	p->ai_protocol)) == -1) {
			    perror("client: socket");
			    continue;
		    }

		    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			    close(sockfd);
			    perror("client: connect");
			    continue;
		    }

		    break;
	    }

	    if (p == NULL) {
		    fprintf(stderr, "client: failed to connect\n");
		    return -1;
        }
        
        return sockfd;
    }

    else {
        perror("initSock(): Valid mode not specified");
        return -1;
    }
}
