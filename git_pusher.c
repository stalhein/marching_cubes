#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

int str_to_int(char *str, int len) {
    int result = 0;
    for (int i = 0; i < len; ++i) {
        int base = len - i - 1;
        result += (*str++ - '0') * pow(10, base);
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 2) return -1;
    int pause = str_to_int(argv[1], strlen(argv[1]));

    int commit = 28;

    while (1) {
        char command[512];
        snprintf(command, sizeof(command),
                "git add . && git commit -m \"Commit #%d to stop hours being deducted because a commit per 1.4 hours is so substandard and so I don't have to get 3 hours for another ship because now I'm 0.6 hours behind what I wanted from the shop. Currently adding logging and debug mode (--verbose).\" && git push", commit);

        int r = system(command);
        if (r != 0) {
            printf("Git commit failed (%d)\n", r);
        } else {
            commit++;
        }

        sleep(pause);
    }

    return 0;
}
