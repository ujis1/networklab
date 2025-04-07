#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct {
    char data[1024];
} Packet;

typedef struct {
    int frame_kind;  // 1 = data, 0 = ack
    int sq_no;
    Packet packet;
} Frame;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <receiver_ip> <port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in receiver_addr;
    Frame send_frame, recv_frame;
    socklen_t addr_len = sizeof(receiver_addr);
    int frame_id = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(atoi(argv[2]));
    receiver_addr.sin_addr.s_addr = inet_addr(argv[1]);

    while (1) {
        printf("Enter data to send: ");
        fgets(send_frame.packet.data, 1024, stdin);

        send_frame.frame_kind = 1; // data frame
        send_frame.sq_no = frame_id;

        // Send frame
        sendto(sockfd, &send_frame, sizeof(send_frame), 0,
               (struct sockaddr*)&receiver_addr, addr_len);
        printf("[Sender] Frame with seq %d sent.\n", frame_id);

        // Wait for ACK
        recvfrom(sockfd, &recv_frame, sizeof(recv_frame), 0,
                 (struct sockaddr*)&receiver_addr, &addr_len);

        if (recv_frame.sq_no == frame_id && recv_frame.frame_kind == 0) {
            printf("[Sender] ACK %d received.\n", recv_frame.sq_no);
            frame_id = 1 - frame_id; // toggle between 0 and 1
        } else {
            printf("[Sender] Wrong ACK. Resending frame...\n");
        }
    }

    close(sockfd);
    return 0;}