#include "platform.hpp"

#if defined(PLATFORM_MACOS)
#include "macos/platform_macos.h"
#elif defined(PLATFORM_LINUX)
#include "linux/platform_linux.hpp"
#elif defined(PLATFORM_WINDOWS)
#include "windows/platform_windows.hpp"
#else
#error "Platform is not defined or could not be detected"
#endif

std::unique_ptr<Platform> Platform::make_platform() {
#if defined(PLATFORM_MACOS)
    return std::make_unique<PlatformMacos>();
#elif defined(PLATFORM_LINUX)
    return std::make_unique<PlatformLinux>();
#elif defined(PLATFORM_WINDOWS)
    return std::make_unique<PlatformWindows>();
#else
#error "Platform is not defined or could not be detected"
#endif
}
