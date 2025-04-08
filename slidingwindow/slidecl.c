#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct packet {
    char data[1024];
} Packet;

typedef struct frame {
    int frame_kind, sq_no, ack;
    Packet packet;
} Frame;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: %s <server_port>\n", argv[0]);
        exit(0);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[1024];
    socklen_t addr_size;
    int frame_id = 0;
    Frame frame_send;
    Frame frame_recv;
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address setup
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  // Port passed from command line argument
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost server

    // Don't need to bind client socket explicitly here

    addr_size = sizeof(serverAddr); // Receive from server

    while (1) {
        // Prompt for user input and send frame
        frame_send.sq_no = frame_id;
        frame_send.frame_kind = 1;  // Frame kind 1 indicates a data frame
        frame_send.ack = 0;  // No acknowledgment field for data frame

        printf("Enter data: ");
        scanf("%s", buffer);
        strcpy(frame_send.packet.data, buffer);

        // Send frame to server
        if (sendto(sockfd, &frame_send, sizeof(Frame), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Sendto failed");
            close(sockfd);
            exit(1);
        }
        printf("[+] Frame Sent (Seq No: %d)\n", frame_send.sq_no);

        // Receive acknowledgment frame
        int f_recv_size = recvfrom(sockfd, &frame_recv, sizeof(frame_recv), 0, (struct sockaddr*)&serverAddr, &addr_size);
        if (f_recv_size > 0 && frame_recv.frame_kind == 0 && frame_recv.ack == frame_id + 1) {
            printf("[+] ACK Received for Frame %d (Expected Ack: %d)\n", frame_id, frame_id + 1);
        } else {
            printf("[!] Error: ACK not received or mismatch.\n");
        }

        frame_id++;  // Increment sequence number for next frame
    }

    // Close the socket
    close(sockfd);
    return 0;
}

