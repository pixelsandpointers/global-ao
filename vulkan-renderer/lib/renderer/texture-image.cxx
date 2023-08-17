#include <filesystem>
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/texture-image.hxx>
#include <lib/renderer/utilities.hxx>
#include <stb_image.h>

namespace global_ao {
TextureImage::TextureImage(
    const Device& device,
    const std::filesystem::path& texturePath,
    const vk::raii::Queue& graphicsQueue,
    const CommandPool& commandPool)
  : stagingBuffer { std::make_unique<StagingBuffer<stbi_uc>>(createStagingBuffer(device, loadImageData(texturePath))) },
    textureImage { createImage(device) },
    textureImageMemory {
        Utilities::createDeviceMemory(textureImage, device, vk::MemoryPropertyFlagBits::eDeviceLocal)
    } {
    loadTexture(graphicsQueue, commandPool);
}

auto TextureImage::loadImageData(const std::filesystem::path& texturePath)
    -> std::unique_ptr<stbi_uc, void (*)(void*)> {
    auto* imageData = stbi_load(texturePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (imageData == nullptr) {
        throw std::runtime_error("Failed to load texture image!");
    }

    imageSize = width * height * 4;
    return { imageData, stbi_image_free };
}

auto TextureImage::createStagingBuffer(const Device& device, const std::unique_ptr<stbi_uc, void (*)(void*)>& imageData)
    const -> StagingBuffer<stbi_uc> {
    return { device, imageData.get(), imageSize };
}

auto TextureImage::createImage(const Device& device) -> vk::raii::Image {
    const auto imageCreateInfo = vk::ImageCreateInfo {
        .imageType = vk::ImageType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .extent = vk::Extent3D {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        .sharingMode = vk::SharingMode::eExclusive,
        .initialLayout = initialLayout,
    };
    return device.getLogicalDevice().createImage(imageCreateInfo);
}

auto TextureImage::loadTexture(const vk::raii::Queue& graphicsQueue, const CommandPool& commandPool) -> void {
    const auto commandBuffers = commandPool.createCommandBuffers(3);

    transitionImageLayout(
        graphicsQueue,
        commandBuffers.getCommandBuffer(0),
        initialLayout,
        vk::ImageLayout::eTransferDstOptimal);

    copyBufferToImage(graphicsQueue, commandBuffers.getCommandBuffer(1));

    // prepare for shader access
    transitionImageLayout(
        graphicsQueue,
        commandBuffers.getCommandBuffer(2),
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal);
}

auto TextureImage::transitionImageLayout(
    const vk::raii::Queue& graphicsQueue,
    const vk::raii::CommandBuffer& commandBuffer,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout) -> void {
    recordCommandBufferToTransitionImageLayout(commandBuffer, oldLayout, newLayout);

    auto submitInfo = vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

auto TextureImage::copyBufferToImage(const vk::raii::Queue& graphicsQueue, const vk::raii::CommandBuffer& commandBuffer)
    -> void {
    recordCommandBufferToCopyBufferToImage(commandBuffer);

    auto submitInfo = vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

auto TextureImage::recordCommandBufferToTransitionImageLayout(
    const vk::raii::CommandBuffer& commandBuffer,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout) -> void {
    const auto beginInfo = vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);


    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        const auto sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        const auto destinationStage = vk::PipelineStageFlagBits::eTransfer;

        auto imageBarrier = vk::ImageMemoryBarrier {
            .srcAccessMask = {},
            .dstAccessMask = vk::AccessFlagBits::eTransferWrite,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = *textureImage,
            .subresourceRange = vk::ImageSubresourceRange { .aspectMask = vk::ImageAspectFlagBits::eColor,
                              .baseMipLevel = 0,
                              .levelCount = 1,
                              .baseArrayLayer = 0,
                              .layerCount = 1 },
        };

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, nullptr, nullptr, imageBarrier);

    } else if (
        oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        const auto sourceStage = vk::PipelineStageFlagBits::eTransfer;
        const auto destinationStage = vk::PipelineStageFlagBits::eFragmentShader;

        auto imageBarrier = vk::ImageMemoryBarrier {
            .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
            .dstAccessMask = vk::AccessFlagBits::eShaderRead,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = *textureImage,
            .subresourceRange = vk::ImageSubresourceRange {.aspectMask = vk::ImageAspectFlagBits::eColor,
                                                           .baseMipLevel = 0,
                                                           .levelCount = 1,
                                                           .baseArrayLayer = 0,
                                                           .layerCount = 1},
        };

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, nullptr, nullptr, imageBarrier);
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    commandBuffer.end();
}

auto TextureImage::recordCommandBufferToCopyBufferToImage(const vk::raii::CommandBuffer& commandBuffer) -> void {
    const auto beginInfo = vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    commandBuffer.begin(beginInfo);
    const auto region = vk::BufferImageCopy {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = vk::ImageSubresourceLayers { .aspectMask = vk::ImageAspectFlagBits::eColor,
                                                        .mipLevel = 0,
                                                        .baseArrayLayer = 0,
                                                        .layerCount = 1 },
        .imageOffset = vk::Offset3D { 0, 0, 0 },
        .imageExtent = vk::Extent3D { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 },
    };
    commandBuffer.copyBufferToImage(
        *stagingBuffer->getBuffer().getBuffer(),
        *textureImage,
        vk::ImageLayout::eTransferDstOptimal,
        { region });

    commandBuffer.end();
}

}  // namespace global_ao