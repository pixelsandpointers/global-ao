#include <lib/renderer/dynamic-loader.hxx>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;  // use dynamic dispatch loader

namespace global_ao {
DynamicLoader::DynamicLoader() {
    // some magic from https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
    auto vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}
}  // namespace global_ao