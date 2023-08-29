#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class TextureSampler {
  public:
    TextureSampler(const Device& device);

    static auto getBindingDescription() -> vk::DescriptorSetLayoutBinding;
    auto getSampler() const -> const vk::raii::Sampler&;

  private:
    static auto createSampler(const Device& device) -> vk::raii::Sampler;

    vk::raii::Sampler sampler;
};

}  // namespace global_ao
