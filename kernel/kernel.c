#include "include/mstd.h"

void kernelMain (void) {

    Terminal* terminal;

    terminalInitialize (terminal, 80, 25);

    terminalWriteString(terminal, "Hello Terminal");

}
