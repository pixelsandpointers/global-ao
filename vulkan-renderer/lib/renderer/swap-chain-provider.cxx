#include <iostream>
#include <lib/renderer/swap-chain-provider.hxx>
#include <lib/window/window.hxx>
#include <limits>
#include <set>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {
SwapChainHandler::SwapChainHandler(const Device& device, const Surface& surface, const Window& window)
  : surface { surface },
    device { device },
    surfaceFormat { selectSurfaceFormat() },
    swapChainExtent { selectSwapExtent(window) },
    swapChain { createSwapChain() },
    swapChainImages { swapChain.getImages() },
    imageViews { createImageViews() } {
}

auto SwapChainHandler::selectSurfaceFormat() -> vk::SurfaceFormatKHR {
    const auto& physicalDevice = device.getPhysicalDevice();
    const auto& _surface = surface.getVulkanInternalSurface();
    const auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*_surface);

    if (surfaceFormats.empty()) {
        throw std::runtime_error("No surface formats found");
    }

    // find a format that supports srgb color space
    auto surfaceFormatIt =
        std::find_if(surfaceFormats.begin(), surfaceFormats.end(), [](const vk::SurfaceFormatKHR& _surfaceFormat) {
            return _surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
                   && _surfaceFormat.format == vk::Format::eB8G8R8A8Srgb;
        });

    if (surfaceFormatIt == surfaceFormats.end()) {
        // if no srgb color space is found, just use the first one
        surfaceFormatIt = surfaceFormats.begin();
    }

    auto _surfaceFormat = *surfaceFormatIt;
    return _surfaceFormat;
}

auto SwapChainHandler::selectSwapExtent(const Window& window) -> vk::Extent2D {
    auto surfaceCapabilities =
        device.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface.getVulkanInternalSurface());
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return surfaceCapabilities.currentExtent;
    }
    const auto [width, height] = window.getFramebufferSize();
    auto actualExtent = vk::Extent2D { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    actualExtent.width = std::clamp(
        actualExtent.width,
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height,
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);

    return actualExtent;
}

auto SwapChainHandler::selectSwapPresentMode() -> vk::PresentModeKHR {
    // get the supported present modes
    const auto& physicalDevice = device.getPhysicalDevice();
    const auto& _surface = surface.getVulkanInternalSurface();
    auto presentModes = physicalDevice.getSurfacePresentModesKHR(*_surface);

    // check if the mailbox present mode is supported
    auto presentModeIt = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eMailbox);
    if (presentModeIt != presentModes.end()) {
        // mailbox is available, so use it
        return vk::PresentModeKHR::eMailbox;
    }
    // if mailbox is not available try fifo relaxed
    presentModeIt = std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eFifoRelaxed);
    if (presentModeIt != presentModes.end()) {
        // fifo relaxed is available, so use it
        return vk::PresentModeKHR::eFifoRelaxed;
    }
    // the FIFO present mode is guaranteed by the spec to be supported,
    // so we use that as a fallback
    return vk::PresentModeKHR::eFifo;
}

vk::raii::SwapchainKHR SwapChainHandler::createSwapChain() {
    auto swapChainPresentMode = selectSwapPresentMode();

    // get the supported surface capabilities
    auto surfaceCapabilities =
        device.getPhysicalDevice().getSurfaceCapabilitiesKHR(*surface.getVulkanInternalSurface());

    auto swapChainCreateInfo = vk::SwapchainCreateInfoKHR {
        .flags = vk::SwapchainCreateFlagsKHR(),
        .surface = *surface.getVulkanInternalSurface(),
        .minImageCount = std::max(
            surfaceCapabilities.minImageCount + 1,
            surfaceCapabilities
                .minImageCount),  // it's recommended to request at least one more image than the minimum available
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,      // we don't want any transformations
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,  // ignore alpha channel
        .presentMode = swapChainPresentMode,
        .clipped = VK_TRUE,                                        // we don't care about pixels that are obscured
    };

    // check if graphics and present queues are the same
    auto queueFamilyIndices = std::array { device.getGraphicsQueueFamilyIndex(), device.getPresentQueueFamilyIndex() };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between the queues,
        // or we have to create the swapchain with imageSharingMode as VK_SHARING_MODE_CONCURRENT
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapChainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }

    return device.getLogicalDevice().createSwapchainKHR(swapChainCreateInfo);
}

auto SwapChainHandler::createImageViews() -> std::vector<vk::raii::ImageView> {
    const auto& _device = device.getLogicalDevice();

    std::vector<vk::raii::ImageView> _imageViews;
    _imageViews.reserve(swapChainImages.size());

    for (const auto& image : swapChainImages) {
        auto imageViewCreateInfo = vk::ImageViewCreateInfo {
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = surfaceFormat.format,
            .components = vk::ComponentMapping { .r = vk::ComponentSwizzle::eIdentity,
                                                .g = vk::ComponentSwizzle::eIdentity,
                                                .b = vk::ComponentSwizzle::eIdentity,
                                                .a = vk::ComponentSwizzle::eIdentity },
            .subresourceRange = vk::ImageSubresourceRange { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                                .baseMipLevel = 0,
                                                .levelCount = 1,
                                                .baseArrayLayer = 0,
                                                .layerCount = 1 }
        };
        imageViews.push_back(_device.createImageView(imageViewCreateInfo));
    }
    return _imageViews;
}

}  // namespace global_ao