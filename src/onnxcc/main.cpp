#include "onnxcc/cli/cli.h"
#include <iostream>

int main(int argc, char* argv[]) {
    return onnxcc::cli::execute(argc, argv, std::cout, std::cerr);
}
