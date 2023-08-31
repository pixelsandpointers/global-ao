#include <lib/renderer/utilities.hxx>

namespace global_ao {
auto Utilities::findSuitableMemoryTypeIndex(
    const global_ao::Device& device,
    uint32_t typeFilter,
    vk::MemoryPropertyFlags requestedProperties) -> uint32_t {
    const auto& physicalDevice = device.getPhysicalDevice();
    const auto memoryProperties = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if (((typeFilter & (1 << i)) != 0U)
            && (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

auto Utilities::findSupportedImageFormat(
    const Device& device,
    const std::vector<vk::Format>& candidates,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlags features) -> vk::Format {
    const auto& physicalDevice = device.getPhysicalDevice();

    for (const auto& format : candidates) {
        const auto formatProperties = physicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (formatProperties.linearTilingFeatures & features) == features) {
            return format;
        } else if (
            tiling == vk::ImageTiling::eOptimal
            && (formatProperties.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

}  // namespace global_ao