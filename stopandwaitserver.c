//server
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
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    Frame recv_frame, ack_frame;
    socklen_t addr_len = sizeof(client_addr);
    int expected_id = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(1);
    }

    printf("[Receiver] Listening on port %s...\n", argv[1]);

    while (1) {
        recvfrom(sockfd, &recv_frame, sizeof(recv_frame), 0,
                 (struct sockaddr*)&client_addr, &addr_len);

        if (recv_frame.sq_no == expected_id && recv_frame.frame_kind == 1) {
            printf("[Receiver] Received: %s", recv_frame.packet.data);

            // Send ACK
            ack_frame.frame_kind = 0; // ack
            ack_frame.sq_no = expected_id;
            sendto(sockfd, &ack_frame, sizeof(ack_frame), 0,
                   (struct sockaddr*)&client_addr, addr_len);
            printf("[Receiver] Sent ACK %d\n", expected_id);

            expected_id = 1 - expected_id; // toggle expected sequence
        } else {
            // Resend last ACK
            ack_frame.frame_kind = 0;
            ack_frame.sq_no = 1 - expected_id;
            sendto(sockfd, &ack_frame, sizeof(ack_frame), 0,
                   (struct sockaddr*)&client_addr, addr_len);
            printf("[Receiver] Duplicate frame or out of order. Resent ACK %d\n", ack_frame.sq_no);
        }
    }

    close(sockfd);
    return 0;
}
//server