#include "samples/simplefunction.proxy.cpp"
#include <stdio.h>

int rpc_main(int argc, char *argv[]) {
    (void)argc, (void)argv;
    //
    // Call (possibly remote) func1
    //
    printf("Calling func1()\n");
    func1(); // remote call (we hope!)
    printf("Returned from func1()\n");

    //
    // Call (possibly remote) func2
    //
    printf("Calling func2()\n");
    func2(); // remote call (we hope!)
    printf("Returned from func2()\n");

    //
    // Call (possibly remote) func3
    //
    printf("Calling func3()\n");
    func3(); // remote call (we hope!)
    printf("Returned from func3()\n");

    return 0;
}
