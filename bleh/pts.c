#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>

int main() {
    int status;
    
    status = grantpt(0);
    if (status)
        return status;

    status = unlockpt(0);
    if (status)
        return status;
    
    char *pts = ptsname(0);
    printf("%s\n", pts ? pts : "");
    return pts ? 0 : 1;
}
