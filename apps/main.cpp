//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "logging.h"
#include "platform/platform.h"
#include "plugins/plugins.h"

#include "primitive_app.h"
#include "physx_app.h"
#include "physx_dynamic_app.h"
#include "skybox_app.h"
#include "assimp_app.h"
#include "gui_app.h"
#include "pbr_app.h"
#include "irradiance_app.h"
#include "ibl_app.h"
#include "shadowmap_app.h"
#include "cascade_shadowmap_app.h"
#include "atomic_compute_app.h"

//MARK: - Entry
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
    vox::UnixPlatform platform{vox::UnixType::MAC, argc, argv};
#    elif defined(VK_USE_PLATFORM_XCB_KHR) || defined(VK_USE_PLATFORM_XLIB_KHR) || defined(VK_USE_PLATFORM_WAYLAND_KHR)
    vox::UnixPlatform platform{vox::UnixType::Linux, argc, argv};
#    endif
#endif

    auto code = platform.initialize(plugins::get_all());
    if (code == vox::ExitCode::SUCCESS) {
        platform.set_app(std::make_unique<vox::ShadowMapApp>());
        code = platform.main_loop();
    }
    platform.terminate(code);

#ifndef VK_USE_PLATFORM_ANDROID_KHR
    return EXIT_SUCCESS;
#endif
}

