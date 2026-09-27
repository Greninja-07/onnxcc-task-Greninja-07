#ifndef ONNXCC_CLI_CLI_H
#define ONNXCC_CLI_CLI_H

#include <iostream>
#include <string>

namespace onnxcc::cli {

enum class Subcommand {
    None,
    Dump,
    Run,
    Compile,
    Benchmark,
    Unknown
};

struct DumpOptions {
    std::string model_path;
    bool show_graph{false};
    bool verbose{false};
};

// Dispatch and run CLI logic with specified output and error streams.
// Returns process exit code (0 for success, non-zero for failure).
int execute(int argc, const char* const* argv, std::ostream& out = std::cout, std::ostream& err = std::cerr);

// Handlers for individual subcommands
int handle_dump(const DumpOptions& options, std::ostream& out, std::ostream& err);

// Usage formatting functions
void print_top_level_usage(std::ostream& os);
void print_dump_usage(std::ostream& os);

} // namespace onnxcc::cli

#endif // ONNXCC_CLI_CLI_H
