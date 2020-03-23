#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "../header/hello.h"
#include "../header/hello_world.h"

int main(int argc, char const *argv[]) {

    hello_world();
    hello();

    return 0;
}
