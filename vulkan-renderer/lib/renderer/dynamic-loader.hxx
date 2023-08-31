#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

// This class handles some of the Vulkan dynamic loading magic.
// It needs to be instantiated before any Vulkan objects are created,
// and it needs to be destroyed after all Vulkan objects are destroyed.
class DynamicLoader {
  public:
    DynamicLoader();

  private:
    vk::DynamicLoader dynamicLoader;
};

}  // namespace global_ao