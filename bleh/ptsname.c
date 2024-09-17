#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>

int main() {
    char *pts = ptsname(0);
    printf("%s\n", pts ? pts : "");
    return 0;
}
