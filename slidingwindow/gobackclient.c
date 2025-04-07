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

#define W 5  // Window size

char a[10];  // Buffer to store frame numbers
char b[10];  // Buffer for timeout message

void alpha9(int z);

int main() {
    int s, f, wl, c = 1, i = 0, j, n, p = 0, e = 0;
    struct sockaddr_in ser;

    // Create socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up server address
    ser.sin_family = AF_INET;
    ser.sin_port = htons(6500);  // Port 6500
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost address

    // Connect to the server
    if (connect(s, (struct sockaddr *)&ser, sizeof(ser)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("\nTCP Connection Established.\n");
    printf("\nEnter the number of Frames: ");
    scanf("%d", &f);  // Get the total number of frames
    alpha9(f);
    send(s, a, sizeof(a), 0);  // Send the number of frames to the server

    // Initialize timeout message and other variables
    strcpy(b, "Time Out ");
    int base = 1;  // Base of the window (start of the window)
    int nextSeqNum = 1;  // Next sequence number to send

    while (base <= f) {
        // Send frames up to window size W
        for (i = 0; i < W && nextSeqNum <= f; i++) {
            alpha9(nextSeqNum);
            send(s, a, sizeof(a), 0);  // Send the frame
            printf("\nFrame %d Sent", nextSeqNum);
            nextSeqNum++;
        }

        // Set timeout for receiving acknowledgment (3 seconds)
        struct timeval timeout;
        timeout.tv_sec = 3;  // 3-second timeout
        timeout.tv_usec = 0;
        setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        // Wait for acknowledgments or handle timeouts
        i = 0;
        wl = W;  // Current window size
        while (i < wl) {
            recv(s, a, sizeof(a), 0);  // Receive acknowledgment
            p = atoi(a);  // Convert the received acknowledgment to an integer

            if (strcmp(a, b) == 0) {
                // Timeout occurred, resend frames starting from 'base'
                printf("\nTimeout: Resending frames from %d onwards\n", base);
                nextSeqNum = base;
                break;
            } else {
                // Valid acknowledgment received
                if (p == base) {
                    printf("\nAcknowledgment for Frame %d Received", p);
                    base++;  // Slide the window forward
                    wl--;  // Decrease the window size
                } else {
                    // Out-of-order acknowledgment, break the loop and handle it
                    printf("\nOut-of-order acknowledgment received for Frame %d\n", p);
                    break;
                }
            }
            i++;  // Increment counter for received acknowledgments
        }
    }

    // Close the socket after sending all frames and receiving acknowledgments
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

