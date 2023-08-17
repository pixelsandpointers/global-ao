#include <lib/renderer/textures/texture-sampler.hxx>

namespace global_ao {
TextureSampler::TextureSampler(const Device& device)
  : sampler { createSampler(device) } {
}

auto TextureSampler::createSampler(const Device& device) -> vk::raii::Sampler {
    const auto& physicalDevice = device.getPhysicalDevice();
    const auto& physicalDeviceProperties = physicalDevice.getProperties();

    const auto samplerCreateInfo = vk::SamplerCreateInfo {
        .magFilter = vk::Filter::eLinear,              // magnification filter (linear interpolation)
        .minFilter = vk::Filter::eLinear,              // minification filter (linear interpolation)
        .mipmapMode = vk::SamplerMipmapMode::eLinear,  // mipmap interpolation mode
        // address mode to specify how texture coordinates outside the [0, 1] range are handled
        .addressModeU = vk::SamplerAddressMode::eRepeat,  // just repeat the texture
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0F,                                                     // level of detail bias for mipmap level
        .anisotropyEnable = VK_TRUE,                                            // enable anisotropic filtering
        .maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy,  // just use the maximum anisotropy
        .compareEnable = VK_FALSE,            // enable comparison against another value when doing texture lookups
        .compareOp = vk::CompareOp::eAlways,  // comparison operation to use
        .minLod = 0.0F,                       // minimum level of detail to pick mip level
        .maxLod = 0.0F,                       // maximum level of detail to pick mip level
        .borderColor = vk::BorderColor::eIntOpaqueBlack,  // border color to use if we were using border clamp
        .unnormalizedCoordinates = VK_FALSE,  // false means we're using normalized coordinates (between 0 and 1), true
                                              // means we're using actual texel coordinates (between 0 and texture size)
                                              // basically the resolution of the texture doesn't matter for the sampler
    };

    return vk::raii::Sampler { device.getLogicalDevice(), samplerCreateInfo };
}

auto TextureSampler::getBindingDescription() -> vk::DescriptorSetLayoutBinding {
    return {
        .binding = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eFragment,
        .pImmutableSamplers = nullptr,
    };
}

auto TextureSampler::getSampler() const -> const vk::raii::Sampler& {
    return sampler;
}
}  // namespace global_ao