#include "Include.h"

int main(int argc, char** argv){
    int result = 0;
    initShell();

    //runBatch("batch.txt");
    //return 0;

    switch (argc) {
    case 1:
        result = runInteractive();
        break;
    case 2:
        result = runBatch(argv[1]);
        break;
    default:
        logError();
        result = 1;
    }

	return result;
}