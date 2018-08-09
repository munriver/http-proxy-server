/*
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "initSock.c"
#include "util.c"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define MAXDATASIZE 4096 // max number of bytes we can get at once 


int main(void)
{
	int sockfd, sockfd2, new_fd, new_fd2, numbytes;  // listen on sock_fd, new connection on new_fd
	struct addrinfo *servinfo;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
    char buf[MAXDATASIZE];

    if ((sockfd = initSock(1, NULL, PORT, servinfo)) == -1) {
        fprintf(stderr, "Failed to initialize as server\n");
        return -1;
    }

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

//	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			return -1;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

	    if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	        perror("recv");
	        exit(1);
        }
	        
        buf[numbytes] = '\0';
        printf("%d\n", numbytes);
	    printf("received \n%s",buf);
        char *hostnm = malloc(numbytes/2);
        hostextract(buf, hostnm);
        puts(">>> i gots the host:");
        puts(hostnm);
        
        if ((sockfd2 = initSock(2, hostnm , "80", servinfo)) == -1) {
            fprintf(stderr, "Failed to initialize as client\n");
            return -1; 
        }
        freeaddrinfo(servinfo);

        if (sendall(sockfd2, buf, &numbytes) == -1) { 
            fprintf(stderr, "Unable to send to server. Could only send %d bytes\n", numbytes);
            return -1;
        }
        printf("Sent request\n");
        
        if ((numbytes == recv(sockfd2, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        
        buf[numbytes] = '\0';
        printf("%d\n", numbytes);
	    printf("received from server \n%s",buf);

        if (sendall(new_fd, buf, &numbytes) == -1) {
            fprintf(stderr, "Unable to send back to browser. Could only send %d bytes\n", numbytes);
            return -1;
        }
        printf("Sent to browser\n");
        
        if((numbytes == recv(sockfd2, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%d\n", numbytes);
	    printf("received from server \n%s",buf);

        if (sendall(new_fd, buf, &numbytes) == -1) {
            fprintf(stderr, "Unable to send back to browser. Could only send %d bytes\n", numbytes);
            return -1;
        }
    
		close(new_fd2);  
        close(new_fd);
//	}
    close(sockfd);
    close(sockfd2);

	return 0;
}

