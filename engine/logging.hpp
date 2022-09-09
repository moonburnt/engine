// Simple logging solution, inspired by python's logging module
// Header-only (at least for now), --optionally supports fmt--
// (Maybe it wont actually support fmt, but use cpp 20's <format>?
// Or maybe use this or that or throw error if neither are available?

#include <deque>
#include <string>
#include <cstdio>
#include <iostream>

#include  "fmt/format.h"

// TODO
// #if __has_include("fmt/format.h")
// #include "fmt/format.h"
// using namespace fmt;
// #elif __cplusplus => 202002L
// #include <format>
// using namespace std;
// #else
// #error "Logging library require fmt or cpp20+ to work"
// #endif

enum class LogLevel {
    trace,
    debug,
    info,
    warn,
    error,
    critical
};

class Logger {
protected:
    LogLevel log_level;
    std::string formatter = "[%H:%M:%S][%l] %v";

    std::deque<std::pair<LogLevel, std::string>> queue;

public:
    Logger()
        : log_level(LogLevel::info) {}

    void set_format(const std::string& _formatter) {
        formatter = _formatter;
    }

    void msg(const std::string& txt, LogLevel lvl) {
        // This may be suboptimal
        if (lvl <= log_level) {
            queue.push_back({lvl, txt});
        }
    }

    void trace(const std::string& txt) {
        msg(txt, LogLevel::trace);
    }
    void debug(const std::string& txt) {
        msg(txt, LogLevel::debug);
    }
    void info(const std::string& txt) {
        msg(txt, LogLevel::info);
    }
    void warn(const std::string& txt) {
        msg(txt, LogLevel::warn);
    }
    void error(const std::string& txt) {
        // TODO: make use of cerr for warnings, errors and critical msges
        msg(txt, LogLevel::error);
    }
    void critical(const std::string& txt) {
        msg(txt, LogLevel::critical);
    }

    void update() {
        while (!queue.empty()) {
            // This does not work, coz our pair has no formatter yet
            std::cout << fmt::format(formatter, queue.front()) << "\n";
            queue.pop_front();
        }
    }

    // Globally accessible instance of logger
    static Logger log;
};
