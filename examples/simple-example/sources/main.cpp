#include <cstdlib>
#include <iostream>
#include <vector>

#include "argument_parser.hpp"
#include "python/object.hpp"

int main(int argc, const char *const argv[], const char *const envp[])
{
    std::shared_ptr<argument_parser::ArgumentParser> argument_parser = nullptr;
    std::shared_ptr<argument_parser::Namespace> arguement_namespace = nullptr;
    std::unique_ptr<python::Object> python_object = nullptr;

    try
    {
        // Create ArgumentParser with description
        argument_parser = std::make_shared<argument_parser::ArgumentParser>(argc, argv,
                                                                            "A simple example demonstrating the component-engine functionality");
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

    try
    {
        arguement_namespace = std::make_shared<argument_parser::Namespace>(argument_parser->parse_args());
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        python_object = std::make_unique<python::Object>();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
