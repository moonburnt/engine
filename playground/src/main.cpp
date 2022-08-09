#include "app.hpp"

#include <spdlog/spdlog.h>

int main(int argc, char* const* argv) {
    // Processing launch arguments.
    // For now there is just one - to toggle on debug messages.
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (std::strcmp(argv[i], "--debug") == 0) {
                spdlog::set_level(spdlog::level::debug);
            }
        }
    }

    App app;
    app.run();

    return 0;
}
