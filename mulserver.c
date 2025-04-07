#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function to handle communication
void func(int connfd)
{
	char buff[MAX];
	int n;

	for (;;) {
		bzero(buff, MAX);

		// Read message from client
		read(connfd, buff, sizeof(buff));
		printf("From client: %s\tTo client: ", buff);

		// If client sent "exit", break loop
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}

		// Get server input to send back
		bzero(buff, MAX);
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;

		// Send to client
		write(connfd, buff, sizeof(buff));
	}
}

int main()
{
	int sockfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cli;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	bzero(&servaddr, sizeof(servaddr));

	// Assign IP and port
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Bind socket
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Listen for client
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	len = sizeof(cli);

	// Accept client
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accepted the client...\n");

	// Start chat
	func(connfd);

	// Close socket
	close(sockfd);
}
