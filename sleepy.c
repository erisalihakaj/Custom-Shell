#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
        return 1;
    }

    int sleep_time = atoi(argv[1]);

    sleep(sleep_time);  // Sleep for the given number of seconds

    printf("\nSleepy process finished after %d seconds.\n", sleep_time);
    fflush(stdout);

    return 0;
}
