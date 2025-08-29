#include <cstdlib>
#include <iostream>
#include <vector>

#include "argument_parser.hpp"

int main(int argc, const char *const argv[], const char *const envp[])
{
    try
    {
        // Create ArgumentParser with description
        argument_parser::ArgumentParser parser(argc, argv,
                                               "A simple example demonstrating C++ ArgumentParser functionality");

        // Add positional argument
        parser.add_argument("filename", "store", "", "", "",
                            "Input filename to process");

        // Add optional arguments
        parser.add_argument(std::vector<std::string>{"-v", "--verbose"},
                            "store_true", "", "", "false",
                            "Enable verbose output");

        parser.add_argument(std::vector<std::string>{"-o", "--output"},
                            "store", "", "", "output.txt",
                            "Output filename", "FILE");

        parser.add_argument(std::vector<std::string>{"-n", "--count"},
                            "store", "", "", "1",
                            "Number of iterations", "N");

        parser.add_argument(std::vector<std::string>{"-m", "--mode"},
                            "store", "", "", "auto",
                            "Processing mode", "MODE", false,
                            std::vector<std::string>{"auto", "manual", "debug"});

        parser.add_argument(std::vector<std::string>{"--version"},
                            "version", "", "ArgumentParser Example v1.0", "",
                            "Show version information");

        // Parse arguments
        auto args = parser.parse_args();

        // Use parsed arguments
        std::cout << "Parsed Arguments:" << std::endl;
        std::cout << "  filename: " << args.get<std::string>("filename") << std::endl;
        std::cout << "  verbose: " << (args.get<bool>("verbose") ? "true" : "false") << std::endl;
        std::cout << "  output: " << args.get<std::string>("output") << std::endl;
        std::cout << "  count: " << args.get<int>("count") << std::endl;
        std::cout << "  mode: " << args.get<std::string>("mode") << std::endl;

        // Demonstrate functionality
        if (args.get<bool>("verbose"))
        {
            std::cout << "\nVerbose mode enabled!" << std::endl;
            std::cout << "Processing file: " << args.get<std::string>("filename") << std::endl;
            std::cout << "Output will be written to: " << args.get<std::string>("output") << std::endl;
            std::cout << "Running " << args.get<int>("count") << " iterations" << std::endl;
            std::cout << "Mode: " << args.get<std::string>("mode") << std::endl;
        }

        std::cout << "\nProgram completed successfully!" << std::endl;
    }
    catch (const argument_parser::ArgumentError &exception)
    {
        std::cerr << "Argument error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
