#include <lib/renderer/sync-objects-handler.hxx>

namespace global_ao {
SyncObjectsHandler::SyncObjectsHandler(const Device& device)
  : device { device },
    syncObjects { createSyncObjects() } {
}

auto SyncObjectsHandler::getSyncObjects() const -> const SyncObjectsHandler::SyncObjects& {
    return syncObjects;
}

auto SyncObjectsHandler::createSyncObjects() -> SyncObjectsHandler::SyncObjects {
    const auto& _device = device.getLogicalDevice();
    return {
        .imageAvailableSemaphore = _device.createSemaphore(vk::SemaphoreCreateInfo {}),
        .renderFinishedSemaphore = _device.createSemaphore(vk::SemaphoreCreateInfo {}),
        .inFlightFence = _device.createFence(vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled })
    };  // set to signaled so that we don't wait indefinitely on the first frame
}


}  // namespace global_ao