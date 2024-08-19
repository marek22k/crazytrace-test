#include "../src/seccomp_sandbox.c"
#include <assert.h>
#include <stdio.h>

void test_seccomp_initialization() {
    printf("Testing seccomp initialization...\n");
    initialize_seccomp();
    // If the program reaches this point, seccomp was initialized successfully
    printf("Seccomp initialized successfully.\n");
}

int main() {
    test_seccomp_initialization();
    printf("All tests passed.\n");
    return 0;
}
