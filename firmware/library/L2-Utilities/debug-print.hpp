#ifndef __DEBUG_PRINT_HPP_
#define __DEBUG_PRINT_HPP_

#ifndef DEBUG
    #define DEBUG 3
#endif

#define DEBUG_TEXT_COLOR  "\e[1;32m"
#define DEBUG_COLOR_RESET "\e[0m"
#define DEBUG_FILE_COLOR  "\e[0;91m"
#define DEBUG_LINE_COLOR  "\e[0;94m"
#define USE_COLOR 1

#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, \
    DEBUG_TEXT_COLOR "[DEBUG]" DEBUG_COLOR_RESET \
     " %s:%s:" \
    DEBUG_FILE_COLOR "%d" DEBUG_COLOR_RESET "> "\
    fmt "\n", \
    __FILE__, \
    __PRETTY_FUNCTION__, \
    __LINE__, \
    ##args)
#else
 #define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif