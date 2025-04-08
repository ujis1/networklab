#include <stdio.h>

int main() {
    int incoming, outgoing, buck_size, n, store = 0;

    // User input for bucket size, outgoing rate, and number of incoming packets
    printf("Enter bucket size, outgoing rate and number of inputs: ");
    scanf("%d %d %d", &buck_size, &outgoing, &n);

    while (n > 0) {
        printf("\nEnter the incoming packet size: ");
        scanf("%d", &incoming);

        printf("Incoming packet size: %d\n", incoming);

        // Check if the incoming packet can fit into the buffer
        if (incoming <= (buck_size - store)) {
            store += incoming;
            printf("Bucket buffer size: %d out of %d\n", store, buck_size);
        } else {
            printf("Dropped %d packets\n", incoming - (buck_size - store));
            store = buck_size;
            printf("Bucket buffer size: %d out of %d\n", store, buck_size);
        }

        // Simulate the outgoing packets
        store -= outgoing;
        if (store < 0) {
            store = 0;
        }

        printf("After outgoing, %d packets left out of %d in buffer\n", store, buck_size);
        n--;
    }

    return 0;
}
