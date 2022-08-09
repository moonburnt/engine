#include "platform_macos.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <fmt/format.h>

std::string PlatformMacos::get_resource_dir() {
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef resource_dir = CFBundleCopyResourcesDirectoryURL(bundle);
    CFURLRef path = CFURLCopyAbsoluteURL(resource_dir);
    CFStringRef string = CFURLCopyFileSystemPath(path, kCFURLPOSIXPathStyle);
    char bundle_str [4096] = { 0 };
    CFStringGetCString(string, bundle_str, sizeof(bundle_str), kCFStringEncodingUTF8);
    CFRelease(string);
    CFRelease(path);
    CFRelease(resource_dir);
    return std::string(bundle_str) + "/";
}

std::string PlatformMacos::get_sprites_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_sounds_dir() {
    return get_resource_dir();
}

std::string PlatformMacos::get_settings_dir() {
    return get_resource_dir();
}
