#include "argument_parser.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <cstdlib>

namespace argument_parser
{

    // Action implementations
    std::string Action::format_usage() const
    {
        if (is_optional())
        {
            std::string usage = "[";
            if (!option_strings.empty())
            {
                usage += option_strings[0];
            }
            if (nargs != "0" && !metavar.empty())
            {
                usage += " " + metavar;
            }
            else if (nargs != "0" && metavar.empty() && !dest.empty())
            {
                std::string dest_upper = dest;
                std::transform(dest_upper.begin(), dest_upper.end(), dest_upper.begin(), ::toupper);
                usage += " " + dest_upper;
            }
            usage += "]";
            return usage;
        }
        else
        {
            if (!metavar.empty())
            {
                return metavar;
            }
            else
            {
                std::string dest_upper = dest;
                std::transform(dest_upper.begin(), dest_upper.end(), dest_upper.begin(), ::toupper);
                return dest_upper;
            }
        }
    }

    void StoreAction::call(ArgumentParser &parser, Namespace &namespace_obj,
                           const std::vector<std::string> &values,
                           const std::string &option_string)
    {
        if (values.empty())
        {
            throw ArgumentError("Expected at least one argument for " + option_string);
        }

        // Validate choices if provided
        if (!choices.empty())
        {
            for (const auto &value : values)
            {
                parser.validate_choices(value, choices);
            }
        }

        if (nargs == "" || nargs == "1")
        {
            namespace_obj.set(dest, values[0]);
        }
        else if (nargs == "*" || nargs == "+")
        {
            // For multiple values, join with spaces (or could store as list)
            std::string combined;
            for (size_t i = 0; i < values.size(); ++i)
            {
                if (i > 0)
                    combined += " ";
                combined += values[i];
            }
            namespace_obj.set(dest, combined);
        }
        else if (nargs == "?")
        {
            if (!values.empty())
            {
                namespace_obj.set(dest, values[0]);
            }
            else if (!const_value.empty())
            {
                namespace_obj.set(dest, const_value);
            }
            else if (!default_value.empty())
            {
                namespace_obj.set(dest, default_value);
            }
        }
    }

    void StoreConstAction::call(ArgumentParser &parser, Namespace &namespace_obj,
                                const std::vector<std::string> &values,
                                const std::string &option_string)
    {
        namespace_obj.set(dest, const_value);
    }

    void HelpAction::call(ArgumentParser &parser, Namespace &namespace_obj,
                          const std::vector<std::string> &values,
                          const std::string &option_string)
    {
        parser.print_help();
        std::exit(0);
    }

    void VersionAction::call(ArgumentParser &parser, Namespace &namespace_obj,
                             const std::vector<std::string> &values,
                             const std::string &option_string)
    {
        std::cout << version_ << std::endl;
        std::exit(0);
    }

    // ArgumentParser implementations
    ArgumentParser::ArgumentParser(const std::string &prog,
                                   const std::string &description,
                                   const std::string &epilog,
                                   bool add_help)
        : prog_(prog.empty() ? "program" : prog),
          description_(description),
          epilog_(epilog),
          add_help_(add_help)
    {

        if (add_help_)
        {
            auto help_action = std::make_unique<HelpAction>();
            option_string_actions_["-h"] = help_action.get();
            option_string_actions_["--help"] = help_action.get();
            actions_.push_back(std::move(help_action));
        }
    }

    ArgumentParser::ArgumentParser(int argc, const char *const argv[],
                                   const std::string &description,
                                   const std::string &epilog,
                                   bool add_help)
        : ArgumentParser(argc > 0 ? argv[0] : "program", description, epilog, add_help)
    {

        for (int i = 1; i < argc; ++i)
        {
            args_.push_back(argv[i]);
        }
    }

    Action &ArgumentParser::add_argument(const std::string &name_or_flags,
                                         const std::string &action,
                                         const std::string &nargs,
                                         const std::string &const_value,
                                         const std::string &default_value,
                                         const std::string &help,
                                         const std::string &metavar,
                                         bool required,
                                         const std::vector<std::string> &choices)
    {
        return add_argument(std::vector<std::string>{name_or_flags}, action, nargs,
                            const_value, default_value, help, metavar, required, choices);
    }

    Action &ArgumentParser::add_argument(const std::vector<std::string> &name_or_flags,
                                         const std::string &action,
                                         const std::string &nargs,
                                         const std::string &const_value,
                                         const std::string &default_value,
                                         const std::string &help,
                                         const std::string &metavar,
                                         bool required,
                                         const std::vector<std::string> &choices)
    {

        std::string dest = get_dest(name_or_flags);

        auto action_ptr = create_action(action, name_or_flags, dest, nargs,
                                        const_value, default_value, help, metavar,
                                        required, choices);

        Action *action_ref = action_ptr.get();

        // Register option strings for optional arguments
        for (const auto &option_string : name_or_flags)
        {
            if (is_optional_string(option_string))
            {
                option_string_actions_[option_string] = action_ref;
            }
        }

        // Add to positional actions if it's a positional argument
        if (action_ref->is_positional())
        {
            positional_actions_.push_back(action_ref);
        }

        actions_.push_back(std::move(action_ptr));
        return *action_ref;
    }

    std::unique_ptr<Action> ArgumentParser::create_action(const std::string &action_type,
                                                          const std::vector<std::string> &option_strings,
                                                          const std::string &dest,
                                                          const std::string &nargs,
                                                          const std::string &const_value,
                                                          const std::string &default_value,
                                                          const std::string &help,
                                                          const std::string &metavar,
                                                          bool required,
                                                          const std::vector<std::string> &choices)
    {
        if (action_type == "store")
        {
            return std::make_unique<StoreAction>(option_strings, dest, nargs, const_value,
                                                 default_value, help, metavar, required, choices);
        }
        else if (action_type == "store_const")
        {
            return std::make_unique<StoreConstAction>(option_strings, dest, const_value,
                                                      default_value, help, required);
        }
        else if (action_type == "store_true")
        {
            return std::make_unique<StoreTrueAction>(option_strings, dest, help);
        }
        else if (action_type == "store_false")
        {
            return std::make_unique<StoreFalseAction>(option_strings, dest, help);
        }
        else if (action_type == "help")
        {
            return std::make_unique<HelpAction>(option_strings);
        }
        else if (action_type == "version")
        {
            return std::make_unique<VersionAction>(option_strings, const_value, help);
        }
        else
        {
            throw ArgumentError("Unknown action: " + action_type);
        }
    }

    std::string ArgumentParser::get_dest(const std::vector<std::string> &option_strings)
    {
        for (const auto &option_string : option_strings)
        {
            if (option_string.substr(0, 2) == "--")
            {
                std::string dest = option_string.substr(2);
                std::replace(dest.begin(), dest.end(), '-', '_');
                return dest;
            }
        }

        for (const auto &option_string : option_strings)
        {
            if (option_string.substr(0, 1) == "-" && option_string.length() > 1)
            {
                return option_string.substr(1);
            }
        }

        // For positional arguments
        if (!option_strings.empty())
        {
            return option_strings[0];
        }

        return "";
    }

    bool ArgumentParser::is_optional_string(const std::string &arg)
    {
        return !arg.empty() && arg[0] == '-';
    }

    void ArgumentParser::validate_choices(const std::string &value, const std::vector<std::string> &choices)
    {
        if (std::find(choices.begin(), choices.end(), value) == choices.end())
        {
            std::ostringstream oss;
            oss << "Invalid choice: '" << value << "' (choose from ";
            for (size_t i = 0; i < choices.size(); ++i)
            {
                if (i > 0)
                    oss << ", ";
                oss << "'" << choices[i] << "'";
            }
            oss << ")";
            throw ArgumentError(oss.str());
        }
    }

    Namespace ArgumentParser::parse_args(const std::vector<std::string> &args)
    {
        Namespace namespace_obj;
        std::vector<std::string> args_to_parse = args.empty() ? args_ : args;

        // Set default values
        for (const auto &action : actions_)
        {
            if (!action->default_value.empty())
            {
                namespace_obj.set(action->dest, action->default_value);
            }
        }

        size_t positional_index = 0;
        for (size_t i = 0; i < args_to_parse.size(); ++i)
        {
            const std::string &arg = args_to_parse[i];

            if (is_optional_string(arg))
            {
                // Handle optional arguments
                auto it = option_string_actions_.find(arg);
                if (it == option_string_actions_.end())
                {
                    error("Unrecognized argument: " + arg);
                }

                Action *action = it->second;
                std::vector<std::string> values;

                // Handle different nargs values
                if (action->nargs == "0" ||
                    dynamic_cast<StoreTrueAction *>(action) ||
                    dynamic_cast<StoreFalseAction *>(action) ||
                    dynamic_cast<HelpAction *>(action) ||
                    dynamic_cast<VersionAction *>(action))
                {
                    // No arguments needed
                }
                else if (action->nargs == "?" || action->nargs == "" || action->nargs == "1")
                {
                    if (i + 1 < args_to_parse.size() && !is_optional_string(args_to_parse[i + 1]))
                    {
                        values.push_back(args_to_parse[++i]);
                    }
                }
                else if (action->nargs == "*")
                {
                    while (i + 1 < args_to_parse.size() && !is_optional_string(args_to_parse[i + 1]))
                    {
                        values.push_back(args_to_parse[++i]);
                    }
                }
                else if (action->nargs == "+")
                {
                    if (i + 1 >= args_to_parse.size() || is_optional_string(args_to_parse[i + 1]))
                    {
                        error("Argument " + arg + " expected at least one argument");
                    }
                    while (i + 1 < args_to_parse.size() && !is_optional_string(args_to_parse[i + 1]))
                    {
                        values.push_back(args_to_parse[++i]);
                    }
                }

                action->call(*this, namespace_obj, values, arg);
            }
            else
            {
                // Handle positional arguments
                if (positional_index >= positional_actions_.size())
                {
                    error("Too many positional arguments: " + arg);
                }

                Action *action = positional_actions_[positional_index++];
                action->call(*this, namespace_obj, {arg});
            }
        }

        // Check required arguments
        for (const auto &action : actions_)
        {
            if (action->required && !namespace_obj.has(action->dest))
            {
                error("Argument " + action->dest + " is required");
            }
        }

        // Check required positional arguments
        if (positional_index < positional_actions_.size())
        {
            for (size_t i = positional_index; i < positional_actions_.size(); ++i)
            {
                error("The following arguments are required: " + positional_actions_[i]->dest);
            }
        }

        return namespace_obj;
    }

    Namespace ArgumentParser::parse_known_args(const std::vector<std::string> &args)
    {
        // For simplicity, this implementation is the same as parse_args
        // In a full implementation, this would return unknown args separately
        return parse_args(args);
    }

    std::string ArgumentParser::format_usage() const
    {
        std::ostringstream oss;
        oss << "usage: " << prog_;

        // Add optional arguments
        for (const auto &action : actions_)
        {
            if (action->is_optional() && !dynamic_cast<HelpAction *>(action.get()))
            {
                oss << " " << action->format_usage();
            }
        }

        // Add positional arguments
        for (const auto &action : positional_actions_)
        {
            oss << " " << action->format_usage();
        }

        return oss.str();
    }

    std::string ArgumentParser::format_help() const
    {
        std::ostringstream oss;

        // Usage line
        oss << format_usage() << "\n\n";

        // Description
        if (!description_.empty())
        {
            oss << description_ << "\n\n";
        }

        // Positional arguments
        bool has_positional = false;
        for (const auto &action : positional_actions_)
        {
            if (!has_positional)
            {
                oss << "positional arguments:\n";
                has_positional = true;
            }
            oss << "  " << std::left << std::setw(20) << action->dest;
            if (!action->help.empty())
            {
                oss << action->help;
            }
            oss << "\n";
        }

        if (has_positional)
        {
            oss << "\n";
        }

        // Optional arguments
        bool has_optional = false;
        for (const auto &action : actions_)
        {
            if (action->is_optional())
            {
                if (!has_optional)
                {
                    oss << "optional arguments:\n";
                    has_optional = true;
                }

                std::string option_string;
                for (size_t i = 0; i < action->option_strings.size(); ++i)
                {
                    if (i > 0)
                        option_string += ", ";
                    option_string += action->option_strings[i];
                }

                oss << "  " << std::left << std::setw(20) << option_string;
                if (!action->help.empty())
                {
                    oss << action->help;
                }
                oss << "\n";
            }
        }

        // Epilog
        if (!epilog_.empty())
        {
            oss << "\n"
                << epilog_;
        }

        return oss.str();
    }

    void ArgumentParser::print_usage() const
    {
        std::cout << format_usage() << std::endl;
    }

    void ArgumentParser::print_help() const
    {
        std::cout << format_help() << std::endl;
    }

    void ArgumentParser::error(const std::string &message)
    {
        std::cerr << prog_ << ": error: " << message << std::endl;
        print_usage();
        std::exit(2);
    }

} // namespace argument_parser
