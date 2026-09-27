#include <gtest/gtest.h>
#include "onnxcc/cli/cli.h"
#include <sstream>
#include <string>

// Row 1: onnxcc dump --model path/to/file.onnx -> Exit code 0
TEST(CLIContractTest, DumpValidModel) {
    const char* argv[] = {"onnxcc", "dump", "--model", "path/to/file.onnx"};
    int argc = 4;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());
}

// Row 2: onnxcc dump --model f.onnx --show-graph -> Exit 0; --show-graph is a boolean flag
TEST(CLIContractTest, DumpWithShowGraph) {
    const char* argv[] = {"onnxcc", "dump", "--model", "f.onnx", "--show-graph"};
    int argc = 5;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());
}

// Row 3: onnxcc dump --model f.onnx --verbose -> Exit 0; --verbose is a boolean flag
TEST(CLIContractTest, DumpWithVerbose) {
    const char* argv[] = {"onnxcc", "dump", "--model", "f.onnx", "--verbose"};
    int argc = 5;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());
    EXPECT_NE(out.str().find("f.onnx"), std::string::npos);
}

// Additional test: both --show-graph and --verbose simultaneously
TEST(CLIContractTest, DumpWithShowGraphAndVerbose) {
    const char* argv[] = {"onnxcc", "dump", "--model", "f.onnx", "--show-graph", "--verbose"};
    int argc = 6;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());
    std::string out_str = out.str();
    EXPECT_NE(out_str.find("Show graph: true"), std::string::npos);
    EXPECT_NE(out_str.find("Verbose: true"), std::string::npos);
}

// Row 4: onnxcc dump --help -> Prints usage for dump listing its three options; exit 0
TEST(CLIContractTest, DumpHelpListingThreeOptions) {
    const char* argv[] = {"onnxcc", "dump", "--help"};
    int argc = 3;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());

    std::string help_output = out.str();
    EXPECT_NE(help_output.find("model"), std::string::npos);
    EXPECT_NE(help_output.find("show-graph"), std::string::npos);
    EXPECT_NE(help_output.find("verbose"), std::string::npos);
}

// Row 4 short form: onnxcc dump -h -> Prints usage for dump; exit 0
TEST(CLIContractTest, DumpShortHelpListingThreeOptions) {
    const char* argv[] = {"onnxcc", "dump", "-h"};
    int argc = 3;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());

    std::string help_output = out.str();
    EXPECT_NE(help_output.find("model"), std::string::npos);
    EXPECT_NE(help_output.find("show-graph"), std::string::npos);
    EXPECT_NE(help_output.find("verbose"), std::string::npos);
}

// Row 5: onnxcc --help -> Prints top-level usage listing the available subcommands; exit 0
TEST(CLIContractTest, TopLevelHelpListingSubcommands) {
    const char* argv[] = {"onnxcc", "--help"};
    int argc = 2;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());

    std::string help_output = out.str();
    EXPECT_NE(help_output.find("Available subcommands:"), std::string::npos);
    EXPECT_NE(help_output.find("dump"), std::string::npos);
}

// Row 5 short form: onnxcc -h -> Prints top-level usage; exit 0
TEST(CLIContractTest, TopLevelShortHelpListingSubcommands) {
    const char* argv[] = {"onnxcc", "-h"};
    int argc = 2;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_EQ(exit_code, 0);
    EXPECT_TRUE(err.str().empty());

    std::string help_output = out.str();
    EXPECT_NE(help_output.find("Available subcommands:"), std::string::npos);
    EXPECT_NE(help_output.find("dump"), std::string::npos);
}

// Row 6: onnxcc dump (no --model) -> Exit non-zero; readable error on stderr
TEST(CLIContractTest, DumpMissingModelFailsOnStderr) {
    const char* argv[] = {"onnxcc", "dump"};
    int argc = 2;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_NE(exit_code, 0);
    std::string err_output = err.str();
    EXPECT_FALSE(err_output.empty());
    EXPECT_NE(err_output.find("--model"), std::string::npos);
}

// Row 6 variant: onnxcc dump with flags but no --model
TEST(CLIContractTest, DumpFlagsWithoutModelFailsOnStderr) {
    const char* argv[] = {"onnxcc", "dump", "--show-graph", "--verbose"};
    int argc = 4;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_NE(exit_code, 0);
    std::string err_output = err.str();
    EXPECT_FALSE(err_output.empty());
    EXPECT_NE(err_output.find("--model"), std::string::npos);
}

// Row 7: onnxcc bogus -> Exit non-zero; readable error on stderr naming the bad subcommand
TEST(CLIContractTest, BogusSubcommandNamesBadSubcommandOnStderr) {
    const char* argv[] = {"onnxcc", "bogus"};
    int argc = 2;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_NE(exit_code, 0);
    std::string err_output = err.str();
    EXPECT_FALSE(err_output.empty());
    EXPECT_NE(err_output.find("bogus"), std::string::npos);
}

// Row 8: onnxcc (no arguments) -> Exit non-zero; usage on stderr
TEST(CLIContractTest, NoArgumentsPrintsUsageOnStderr) {
    const char* argv[] = {"onnxcc"};
    int argc = 1;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_NE(exit_code, 0);
    std::string err_output = err.str();
    EXPECT_FALSE(err_output.empty());
    EXPECT_NE(err_output.find("Usage:"), std::string::npos);
}

// Unrecognized option to dump reports error on stderr and exits non-zero
TEST(CLIContractTest, DumpUnrecognizedOptionFailsOnStderr) {
    const char* argv[] = {"onnxcc", "dump", "--model", "f.onnx", "--unrecognized-flag"};
    int argc = 5;
    std::ostringstream out;
    std::ostringstream err;

    int exit_code = onnxcc::cli::execute(argc, argv, out, err);

    EXPECT_NE(exit_code, 0);
    std::string err_output = err.str();
    EXPECT_FALSE(err_output.empty());
}
