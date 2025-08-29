#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>

namespace argument_parser
{

    // Exception classes
    class ArgumentError : public std::runtime_error
    {
    public:
        explicit ArgumentError(const std::string &message) : std::runtime_error(message) {}
    };

    class ArgumentTypeError : public std::runtime_error
    {
    public:
        explicit ArgumentTypeError(const std::string &message) : std::runtime_error(message) {}
    };

    // Forward declarations
    class Action;
    class ArgumentParser;

    // Namespace to hold parsed arguments
    class Namespace
    {
    private:
        std::map<std::string, std::string> values_;

    public:
        void set(const std::string &key, const std::string &value)
        {
            values_[key] = value;
        }

        template <typename T>
        T get(const std::string &key) const
        {
            auto it = values_.find(key);
            if (it == values_.end())
            {
                throw ArgumentError("Argument '" + key + "' not found");
            }
            return convert_to<T>(it->second);
        }

        bool has(const std::string &key) const
        {
            return values_.find(key) != values_.end();
        }

        const std::map<std::string, std::string> &get_all() const
        {
            return values_;
        }

    private:
        template <typename T>
        T convert_to(const std::string &value) const
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                return value;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return std::stoi(value);
            }
            else if constexpr (std::is_same_v<T, long>)
            {
                return std::stol(value);
            }
            else if constexpr (std::is_same_v<T, float>)
            {
                return std::stof(value);
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                return std::stod(value);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                std::string lower = value;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
            }
            else
            {
                static_assert(std::is_same_v<T, void>, "Unsupported type for argument conversion");
            }
        }
    };

    // Base Action class
    class Action
    {
    public:
        std::vector<std::string> option_strings;
        std::string dest;
        std::string nargs;
        std::string const_value;
        std::string default_value;
        std::string help;
        std::string metavar;
        bool required;
        std::vector<std::string> choices;

        Action(const std::vector<std::string> &option_strings,
               const std::string &dest,
               const std::string &nargs = "",
               const std::string &const_value = "",
               const std::string &default_value = "",
               const std::string &help = "",
               const std::string &metavar = "",
               bool required = false,
               const std::vector<std::string> &choices = {})
            : option_strings(option_strings), dest(dest), nargs(nargs),
              const_value(const_value), default_value(default_value),
              help(help), metavar(metavar), required(required), choices(choices) {}

        virtual ~Action() = default;

        virtual void call(ArgumentParser &parser, Namespace &namespace_obj,
                          const std::vector<std::string> &values,
                          const std::string &option_string = "") = 0;

        virtual std::string format_usage() const;
        bool is_optional() const { return !option_strings.empty() && option_strings[0][0] == '-'; }
        bool is_positional() const { return !is_optional(); }
    };

    // Specific Action implementations
    class StoreAction : public Action
    {
    public:
        StoreAction(const std::vector<std::string> &option_strings,
                    const std::string &dest,
                    const std::string &nargs = "",
                    const std::string &const_value = "",
                    const std::string &default_value = "",
                    const std::string &help = "",
                    const std::string &metavar = "",
                    bool required = false,
                    const std::vector<std::string> &choices = {})
            : Action(option_strings, dest, nargs, const_value, default_value, help, metavar, required, choices) {}

        void call(ArgumentParser &parser, Namespace &namespace_obj,
                  const std::vector<std::string> &values,
                  const std::string &option_string = "") override;
    };

    class StoreConstAction : public Action
    {
    public:
        StoreConstAction(const std::vector<std::string> &option_strings,
                         const std::string &dest,
                         const std::string &const_value,
                         const std::string &default_value = "",
                         const std::string &help = "",
                         bool required = false)
            : Action(option_strings, dest, "", const_value, default_value, help, "", required) {}

        void call(ArgumentParser &parser, Namespace &namespace_obj,
                  const std::vector<std::string> &values,
                  const std::string &option_string = "") override;
    };

    class StoreTrueAction : public StoreConstAction
    {
    public:
        StoreTrueAction(const std::vector<std::string> &option_strings,
                        const std::string &dest,
                        const std::string &help = "")
            : StoreConstAction(option_strings, dest, "true", "false", help) {}
    };

    class StoreFalseAction : public StoreConstAction
    {
    public:
        StoreFalseAction(const std::vector<std::string> &option_strings,
                         const std::string &dest,
                         const std::string &help = "")
            : StoreConstAction(option_strings, dest, "false", "true", help) {}
    };

    class HelpAction : public Action
    {
    public:
        HelpAction(const std::vector<std::string> &option_strings = {"-h", "--help"})
            : Action(option_strings, "help", "", "", "", "show this help message and exit") {}

        void call(ArgumentParser &parser, Namespace &namespace_obj,
                  const std::vector<std::string> &values,
                  const std::string &option_string = "") override;
    };

    class VersionAction : public Action
    {
    private:
        std::string version_;

    public:
        VersionAction(const std::vector<std::string> &option_strings,
                      const std::string &version,
                      const std::string &help = "show program's version number and exit")
            : Action(option_strings, "version", "", "", "", help), version_(version) {}

        void call(ArgumentParser &parser, Namespace &namespace_obj,
                  const std::vector<std::string> &values,
                  const std::string &option_string = "") override;
    };

    // Main ArgumentParser class
    class ArgumentParser
    {
    private:
        std::string prog_;
        std::string description_;
        std::string epilog_;
        std::vector<std::unique_ptr<Action>> actions_;
        std::map<std::string, Action *> option_string_actions_;
        std::vector<Action *> positional_actions_;
        std::vector<std::string> args_;
        bool add_help_;

    public:
        ArgumentParser(const std::string &prog = "",
                       const std::string &description = "",
                       const std::string &epilog = "",
                       bool add_help = true);

        ArgumentParser(int argc, const char *const argv[],
                       const std::string &description = "",
                       const std::string &epilog = "",
                       bool add_help = true);

        // Add argument methods
        Action &add_argument(const std::string &name_or_flags,
                             const std::string &action = "store",
                             const std::string &nargs = "",
                             const std::string &const_value = "",
                             const std::string &default_value = "",
                             const std::string &help = "",
                             const std::string &metavar = "",
                             bool required = false,
                             const std::vector<std::string> &choices = {});

        Action &add_argument(const std::vector<std::string> &name_or_flags,
                             const std::string &action = "store",
                             const std::string &nargs = "",
                             const std::string &const_value = "",
                             const std::string &default_value = "",
                             const std::string &help = "",
                             const std::string &metavar = "",
                             bool required = false,
                             const std::vector<std::string> &choices = {});

        // Parse methods
        Namespace parse_args(const std::vector<std::string> &args = {});
        Namespace parse_known_args(const std::vector<std::string> &args = {});

        // Help and usage
        std::string format_usage() const;
        std::string format_help() const;
        void print_usage() const;
        void print_help() const;
        void error(const std::string &message);

        // Getters
        const std::string &get_prog() const { return prog_; }
        const std::string &get_description() const { return description_; }

        // Validation methods (public for Action classes)
        void validate_choices(const std::string &value, const std::vector<std::string> &choices);

    private:
        std::unique_ptr<Action> create_action(const std::string &action_type,
                                              const std::vector<std::string> &option_strings,
                                              const std::string &dest,
                                              const std::string &nargs,
                                              const std::string &const_value,
                                              const std::string &default_value,
                                              const std::string &help,
                                              const std::string &metavar,
                                              bool required,
                                              const std::vector<std::string> &choices);

        std::string get_dest(const std::vector<std::string> &option_strings);
        std::vector<std::string> split_args(const std::string &args_string);
        bool is_optional_string(const std::string &arg);
    };

} // namespace argument_parser
