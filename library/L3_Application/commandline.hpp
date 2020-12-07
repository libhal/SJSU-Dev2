#pragma once

#include <cstdio>
#include <cstring>
#include <iterator>

#include "newlib/newlib.hpp"
#include "third_party/microrl/microrl.h"
#include "utility/allocator.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// CommandInterface is the set of methods that every command must support
class CommandInterface
{
 public:
  /// Return the command's name which is used to call this command from a
  /// command line handler.
  virtual const char * GetName() = 0;

  /// Returns a summary/help of what the command can do and how to use it.
  virtual const char * GetDescription() = 0;

  /// The implementing class will have this method run when its command has
  /// been called
  virtual int Program(int argc, const char * const argv[]) = 0;

  /// AutoComplete works by giving the user a set of options that they
  /// can use to complete their command. What options they are given is
  /// specific to the command implementation and at what stage the comment
  /// is at.
  /// For example lets say we have a command called "version" and this
  /// command can take one argument that could either be "bootloader" or
  /// "application". If you have typed "version boot" and then press the
  /// `tab` key, the command line will jump to show "version bootloader".
  /// If you had only typed "version" and pressed `tab`, then you will see
  /// "version `newline` bootloader application" as options. In order to
  /// make this happen, you need to load string pointers into the
  /// completion_array by evaluating the argc and argv parameters. Be sure
  /// not to exceed the length of the completion_array which is given by
  /// the kCompletionArrayLength.
  ///
  /// @param argc - Number of arguments in argv
  /// @param argv - list of strings containing argument parameter text
  /// @param completion - a modifable array of char pointers. For every auto
  ///        complete option that is valid given the supplied argc + argv
  ///        parameters, return the results into this array.
  /// @param completion_length - the number of char pointers that the completion
  ///        array can hold. When the max is hit, the overriding AutoComplete
  ///        should stop writing into the buffer and return.
  /// @return returns the number of completion strings written into the
  ///         completion_array.
  virtual int AutoComplete(int argc,
                           const char * const argv[],
                           const char * completion[],
                           size_t completion_length)
  {
    (void)argc;
    (void)argv;
    (void)completion;
    (void)completion_length;
    return 0;
  }
};

/// The common function signature for all commands.
/// Every command must take an interger that represents the number of arguments
/// passed, argc, and an array of command arguments as character strings, argv.
///
/// A command must return a return code as an interger when the command exits.
/// For more information about what the return codes mean, see:
///    http://tldp.org/LDP/abs/html/exitcodes.html
/// As a rule of thumb though, return of 0 means success, a return of 1 presents
/// an error return code.
using CommandSignature = int (*)(int argc, const char * const argv[]);

/// A generic command class that takes a name, description and a function
/// pointer.
class Command : public CommandInterface
{
 public:
  // Usage:
  //
  // int NewCommandFunction(int argc, const char * const argv[])
  // {
  //   /* does some stuff */
  // }
  // Command new_command("new", "I'm a new command", NewCommandFunction);
  //

  /// @param name the name that the user will type to call this command
  /// @param description the description the user will see when they type list
  ///        try to keep this as shorter than 80 characters. If you want to
  ///        describe the command futher, the "program" should check for a
  ///        "help" argument in argv[1] and give a long description there.
  /// @param program the function that will be called when the user types in
  ///        this commands name
  constexpr Command(const char * name,
                    const char * description,
                    CommandSignature program = DoNothingCommand)
      : name_(name), description_(description), program_(program)
  {
  }

  const char * GetName() override
  {
    return name_;
  }

  const char * GetDescription() override
  {
    return description_;
  }

  /// This class method will execute the function that was passed to this object
  /// via the constructor.
  int Program(int argc, const char * const argv[]) override
  {
    return program_(argc, argv);
  }

 private:
  /// This function acts as a filler function for to be used for functions that
  /// have not yet been defined.
  static int DoNothingCommand(int, const char * const[])
  {
    sjsu::LogInfo("This command hasn't been implemented yet.");
    return 0;
  }

  const char * name_;
  const char * description_;
  CommandSignature program_;
};

/// In order to construct a CommandLine object, you must provide a command list
/// structure with static storage duration (must be a global variable).
///
/// @see{https://en.cppreference.com/w/cpp/language/storage_duration}
///
/// @tparam kNumberOfCommands is the number of commands you would like your
///         CommandLIne object to hold.
template <size_t kNumberOfCommands>
struct CommandList_t
{
  /// Default constructor of the command list that reserves the space in the
  /// command list vector.
  CommandList_t() : buffer(), commands(&buffer)
  {
    commands.reserve(kNumberOfCommands);
  }

  /// Returns the number of commands currently held in the command list.
  size_t size()  // NOLINT
  {
    return commands.size();
  }

  /// Returns the total number of commands that can be held in the command list.
  constexpr size_t max_size()  // NOLINT
  {
    return kNumberOfCommands;
  }

  /// Fixed sized std::vector that holds pointers to a list of command line
  /// commands.
  static constexpr size_t kCommandListBufferSize =
      sizeof(CommandInterface *) * kNumberOfCommands;
  StaticAllocator<kCommandListBufferSize> buffer;
  std::pmr::vector<CommandInterface *> commands;
};

namespace command
{
/// Limit to the number of arguments that can be auto completed
static constexpr size_t kAutoCompleteOptions = 32;
/// A standard print callback used by the microrl library to print to stdout
inline void Print(const char * str)
{
  printf("%s", str);
}
}  // namespace command
/// The CommandLine class is used to create an interactive command line for the
/// user to use. Usage:
///
///     Command command_one("one", "first command", SomeFunction);
///     CommandList_t<16> command_list; // Can hold 16 commands total, must be
///                                   // global/static storage.
///     int main()
///     {
///       CommandLine<command_list> command_line; // Create CommandLine Object
///       command_line.AddCommand(&command_one); // Add a command
///       sjsu::LogInfo("Press Enter To Start Command Line"); // Prompt the user
///       command_line.WaitForInput(); // Start interactive command line
///     }
///
/// @tparam command_list The CommandLine class must have a CommandList_t
///         structure passed to it. The CommandList_t is used to hold the list
///         of available commands. Must be greater than 2.
template <auto /* CommandList_t<#count> */ & command_list>
class CommandLine
{
 public:
  /// Limit to the number of arguments that can be auto completed
  static constexpr size_t kAutoCompleteOptions = 32;

  static_assert(command_list.max_size() > 2,
                "Command list must be of size greater than 2");

  CommandLine()
      : rl_{},
        initialized_(false),
        quit_("quit", "Quit from commandline", Quit),
        list_("list", "List all available commands", List)
  {
    command_list.commands.clear();
  }
  /// Call this to initialie the command line
  void Initialize()
  {
    // Call init with ptr to microrl instance and print callback
    microrl_init(&rl_, command::Print);
    // Set callback for execute
    microrl_set_execute_callback(&rl_, ExecuteCommand);
    // Set callback for completion
    microrl_set_complete_callback(&rl_, AutoCompleteHandler);
    // Adding list and quit to list of commands
    AddCommand(&list_);
    AddCommand(&quit_);
    newlib::StdinEchoBack(false);
    initialized_ = true;
  }
  /// Adds a command to the command list
  ///
  /// @param command the address of a command object to store in the command
  ///        list.
  void AddCommand(CommandInterface * command)
  {
    command_list.commands.push_back(command);
  }
  /// Starts the command line and waits for input from the user. This will put
  /// the system in an infinite loop. To exit this loop, the `quit` command must
  /// be used.
  /// The `Initialized()` of this class method must be called before running
  /// this method.
  void WaitForInput()
  {
    SJ2_ASSERT_FATAL(initialized_,
                     "This commandline has not been initalized yet!");
    is_commandline_running = true;
    List(0, nullptr);

    printf(SJ2_BOLD_WHITE
           "Press Enter to Start Command Line!\n" SJ2_COLOR_RESET);
    while (is_commandline_running)
    {
      // Get char from stdin and send to microrl lib
      microrl_insert_char(&rl_, getchar());
    }
  }

 private:
  /// AutoCompleteHandler is passed to the microrl object and used to handle
  /// <tab> completion.
  ///
  /// The handler first if the command is complete, which is found by checking
  /// if argc <= 1. If it is not, then the set of commands added to this
  /// commandline is checked, and all commands that include the starting
  /// substring of the entered text will be returned.
  ///
  /// If argc > 1, then the auto complete function finds which command has been
  /// entered and run's that command objects AutoComplete handler method.
  static char ** AutoCompleteHandler(int argc, const char * const argv[])
  {
    size_t position                = 0;
    autocomplete_options[position] = nullptr;
    // If the first command hasn't been completed yet
    if (argc <= 1)
    {
      // Iterate through our available token and match it
      for (const auto & command : command_list.commands)
      {
        // If token is matched (text is part of our token starting from 0 char)
        if (strstr(command->GetName(), argv[0]) == command->GetName())
        {
          // Add it to completion set
          autocomplete_options[position++] = command->GetName();
        }
      }
    }
    else
    {
      for (const auto & command : command_list.commands)
      {
        // If token is matched (text is part of our token starting from 0 char)
        if (std::strcmp(command->GetName(), argv[0]) == 0)
        {
          position = command->AutoComplete(argc,
                                           argv,
                                           autocomplete_options,
                                           std::size(autocomplete_options));
          break;
        }
      }
    }
    autocomplete_options[position] = nullptr;
    return const_cast<char **>(autocomplete_options);
  }

  /// ExecuteCommand is passed to the microrl object and used to execute a
  /// command when the <enter> key is pressed.
  /// ExecuteCommand will compare argv[0] (name of the argument) with the list
  /// of command names and if one of them is found, that command will be
  /// executed.
  ///
  /// Note: Microrl will not run this if there is not at least a letter
  /// entered into the terminal, so no need to check if argc == 0.
  static int ExecuteCommand(int argc, const char * const argv[])
  {
    // Result of 0x7F means that the command was not found. See
    int result               = 0x7F;
    bool command_was_handled = false;
    for (const auto & command : command_list.commands)
    {
      if (strcmp(argv[0], command->GetName()) == 0)
      {
        result              = command->Program(argc, argv);
        command_was_handled = true;
        break;
      }
    }
    if (!command_was_handled)
    {
      sjsu::LogWarning("Command: '%s' Not found", argv[0]);
    }
    return result;
  }

  /// Quits this command line
  static int Quit(int, const char * const[])
  {
    is_commandline_running = false;
    printf("\n");
    return 0;
  }

  /// Lists all commands and their descriptions in the command list
  static int List(int, const char * const[])
  {
    printf("List of commands:\n------------------\n\n");
    for (size_t i = 0; command_list.commands[i] != nullptr; i++)
    {
      printf("%*s - %s\n",
             10,
             command_list.commands[i]->GetName(),
             command_list.commands[i]->GetDescription());
    }
    printf("\n");
    return 0;
  }

  static inline bool is_commandline_running = false;
  static inline const char *
      autocomplete_options[command::kAutoCompleteOptions] = { nullptr };

  microrl_t rl_;
  bool initialized_;
  Command quit_;
  Command list_;
};
}  // namespace sjsu
