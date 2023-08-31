#pragma once

#include <lib/renderer/buffer-objects/uniform-buffer-object.hxx>

namespace global_ao {

struct OcclusionPipelineUniformBufferFragmentObject {
    float nSamples;

    static auto getBindingDescription() -> vk::DescriptorSetLayoutBinding;
};
}  // namespace global_ao
