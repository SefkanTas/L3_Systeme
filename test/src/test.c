#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "../header/hello.h"
#include "../header/hello_world.h"

#include <limits.h>

int main(int argc, char const *argv[]) {

    hello_world();
    hello();

    unsigned int a = 100;
    long b = 200;
    a += b;

    printf("%u\n", a);

    return 0;
}
