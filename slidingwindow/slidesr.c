#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct packet {
    char data[1024];
} Packet;

typedef struct frame {
    int frame_kind, sq_no, ack;
    Packet packet;
} Frame;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    int frame_id = 0;
    Frame frame_recv;
    Frame frame_send;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));  // Clear memory
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Use INADDR_ANY to accept packets from any address

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    addr_size = sizeof(clientAddr);

    while (1) {
        // Receive a frame from client
        int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(Frame), 0, (struct sockaddr*)&clientAddr, &addr_size);
        if (f_recv_size < 0) {
            perror("recvfrom failed");
            continue;  // Try again if recvfrom fails
        }

        // Display received frame and message
        printf("[+] Frame Received (Seq No: %d): %s\n", frame_recv.sq_no, frame_recv.packet.data);

        // Send ACK frame back to client
        frame_send.frame_kind = 0;  // Frame kind 0 indicates acknowledgment
        frame_send.ack = frame_recv.sq_no + 1;  // Acknowledge the next sequence number

        if (sendto(sockfd, &frame_send, sizeof(frame_send), 0, (struct sockaddr*)&clientAddr, addr_size) < 0) {
            perror("Sendto failed");
            close(sockfd);
            exit(1);
        }
        printf("[+] ACK Sent (Ack No: %d)\n", frame_send.ack);

        // Wait for next frame
        frame_id++;  // Increment the expected sequence number for the next frame
    }

    // Close the socket
    close(sockfd);
    return 0;
}

