//
//  main.cpp
//  DigitalVox5
//
//  Created by 杨丰 on 2022/4/11.
//

#include "logging.h"
#include "platform/platform.h"
//#include "plugins/plugins.h"

#include "hdr.h"

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
#    include "platform/android/android_platform.h"
void android_main(android_app *state)
{
    vox::AndroidPlatform platform{state};
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
#    include "platform/windows/windows_platform.h"
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     PSTR lpCmdLine, INT nCmdShow)
{
    vox::WindowsPlatform platform{hInstance, hPrevInstance,
                                  lpCmdLine, nCmdShow};
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
#    include "platform/unix/unix_d2d_platform.h"
int main(int argc, char *argv[])
{
    vox::UnixD2DPlatform platform{argc, argv};
#else

#    include "platform/unix/unix_platform.h"

int main(int argc, char *argv[]) {
#    if defined(VK_USE_PLATFORM_METAL_EXT)
    vox::UnixPlatform platform{vox::UnixType::Mac, argc, argv};
#    elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
    vox::UnixPlatform platform{vox::UnixType::Linux, argc, argv};
#    endif
#endif

    std::vector<vox::Plugin *> plugins{};
    auto code = platform.initialize(plugins);
    if (code == vox::ExitCode::Success) {
        platform.set_app(std::make_unique<vox::HDR>());
        code = platform.main_loop();
    }
    platform.terminate(code);

#ifndef VK_USE_PLATFORM_ANDROID_KHR
    return EXIT_SUCCESS;
#endif
}

