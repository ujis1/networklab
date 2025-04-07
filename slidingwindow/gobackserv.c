#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#define W 5        // Window size
#define P1 50      // Probability factor for packet loss
#define P2 10      // Packet loss chance out of P1

char a[10];  // Buffer for frame number
char b[10];  // Buffer for timeout message

void alpha9(int z);

int main() {
    struct sockaddr_in ser, cli;
    int s, n, sock, i, c = 1, f;
    unsigned int s1;
    
    // Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set socket option to allow address reuse
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set up server address
    ser.sin_family = AF_INET;
    ser.sin_port = htons(6500);  // Port number
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost address

    // Bind the socket to the address
    if (bind(s, (struct sockaddr *)&ser, sizeof(ser)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    listen(s, 1);
    n = sizeof(cli);

    // Accept the incoming connection
    sock = accept(s, (struct sockaddr *)&cli, &n);
    if (sock < 0) {
        perror("Accept failed");
        return 1;
    }

    printf("\nTCP Connection Established.\n");

    // Seed the random number generator
    s1 = (unsigned int)time(NULL);
    srand(s1);

    // Receive the total number of frames to be received
    recv(sock, a, sizeof(a), 0);
    f = atoi(a);  // Get the number of frames

    int expectedSeqNum = 1;  // Expected sequence number to receive

    while (expectedSeqNum <= f) {
        for (i = 0; i < W; i++) {
            recv(sock, a, sizeof(a), 0);  // Receive the frame

            // Simulate packet loss with a probability of P2 out of P1
            int j = rand() % P1;
            if (j < P2) {
                printf("\nSimulated packet loss for Frame %s\n", a);
                continue;  // Skip this frame to simulate packet loss
            }

            // Print received frame
            printf("\nFrame %s Received\n", a);

            // Send acknowledgment for the received frame
            send(sock, a, sizeof(a), 0);
            printf("Acknowledgment for Frame %s Sent\n", a);
            
            // Check for sequence number match
            if (atoi(a) == expectedSeqNum) {
                expectedSeqNum++;  // Move to the next expected frame
            } else {
                printf("Out of order frame, expecting %d, but got %s\n", expectedSeqNum, a);
            }
        }
    }

    // Close the connection after processing all frames
    close(sock);
    close(s);
    return 0;
}

void alpha9(int z) {
    int k, i = 0, g;
    k = z;

    // Count number of digits
    while (k > 0) {
        i++;
        k = k / 10;
    }

    g = i;
    i--;

    // Convert integer to string (ASCII characters)
    while (z > 0) {
        k = z % 10;
        a[i] = k + 48;  // Convert to ASCII character
        i--;
        z = z / 10;
    }
    a[g] = '\0';  // Null-terminate the string
}

