#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class ImageSampler {
  public:
    ImageSampler(const Device& device);

    static auto getBindingDescription(const uint32_t binding) -> vk::DescriptorSetLayoutBinding;
    auto getSampler() const -> const vk::raii::Sampler&;

  private:
    static auto createSampler(const Device& device) -> vk::raii::Sampler;

    vk::raii::Sampler sampler;
};

}  // namespace global_ao
