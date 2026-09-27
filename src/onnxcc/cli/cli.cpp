#include "onnxcc/cli/cli.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "onnxcc/third_party/cxxopts.hpp"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <vector>
#include <string_view>

namespace onnxcc::cli {

namespace {

cxxopts::Options create_dump_parser() {
    cxxopts::Options options("onnxcc dump", "Inspect and dump ONNX model details");
    options.custom_help("[options]");
    options.add_options()
        ("m,model", "Path to the ONNX model file (required)", cxxopts::value<std::string>())
        ("show-graph", "Display computational graph structure", cxxopts::value<bool>()->default_value("false"))
        ("v,verbose", "Enable verbose diagnostic output", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print dump usage and options");
    return options;
}

} // namespace

void print_top_level_usage(std::ostream& os) {
    os << "Usage: onnxcc <subcommand> [options]\n\n"
       << "Available subcommands:\n"
       << "  dump       Inspect and dump ONNX model details\n"
       << "  run        Execute an ONNX model (coming soon)\n"
       << "  compile    Compile an ONNX model to executable code (coming soon)\n"
       << "  benchmark  Benchmark model performance (coming soon)\n\n"
       << "Options:\n"
       << "  -h, --help Print top-level usage\n";
}

void print_dump_usage(std::ostream& os) {
    auto parser = create_dump_parser();
    os << parser.help() << "\n";
}

int handle_dump(const DumpOptions& options, std::ostream& out, std::ostream& /*err*/) {
    // Constraint 3: dump does not have to actually open the ONNX file.
    // It accepts and validates the arguments and reports clearly when they're wrong.
    if (options.verbose) {
        out << "Model file: " << options.model_path << "\n";
        out << "Show graph: " << (options.show_graph ? "true" : "false") << "\n";
        out << "Verbose: true\n";
    }
    return 0;
}

int execute(int argc, const char* const* argv, std::ostream& out, std::ostream& err) {
    if (argc <= 1) {
        // Row 8 of CLI table: onnxcc (no arguments) -> Exit non-zero; usage on stderr
        print_top_level_usage(err);
        return 1;
    }

    std::string_view first_arg(argv[1]);

    // Row 5 of CLI table: onnxcc --help -> Prints top-level usage listing the available subcommands; exit 0
    if (first_arg == "--help" || first_arg == "-h") {
        print_top_level_usage(out);
        return 0;
    }

    if (first_arg == "dump") {
        try {
            auto parser = create_dump_parser();

            // Prepare arguments for subcommand parsing: replace argv[0..1] with 'onnxcc dump'
            std::vector<const char*> sub_argv;
            sub_argv.reserve(static_cast<size_t>(argc));
            std::string subcmd_name = "onnxcc dump";
            sub_argv.push_back(subcmd_name.c_str());
            for (int i = 2; i < argc; ++i) {
                sub_argv.push_back(argv[i]);
            }
            int sub_argc = static_cast<int>(sub_argv.size());

            auto result = parser.parse(sub_argc, sub_argv.data());

            // Row 4 of CLI table: onnxcc dump --help -> Prints usage for dump listing its three options; exit 0
            if (result.count("help") > 0) {
                out << parser.help() << "\n";
                return 0;
            }

            // Row 6 of CLI table: onnxcc dump (no --model) -> Exit non-zero; readable error on stderr
            if (result.count("model") == 0 || result["model"].as<std::string>().empty()) {
                err << "Error: Missing required option '--model'.\n";
                return 1;
            }

            DumpOptions dump_options;
            dump_options.model_path = result["model"].as<std::string>();
            dump_options.show_graph = result["show-graph"].as<bool>();
            dump_options.verbose = result["verbose"].as<bool>();

            return handle_dump(dump_options, out, err);
        } catch (const cxxopts::exceptions::exception& e) {
            err << "Error: " << e.what() << "\n";
            return 1;
        }
    }

    // Row 7 of CLI table: onnxcc bogus -> Exit non-zero; readable error on stderr naming the bad subcommand
    err << "Error: Unknown subcommand '" << first_arg << "'.\n";
    print_top_level_usage(err);
    return 1;
}

} // namespace onnxcc::cli
