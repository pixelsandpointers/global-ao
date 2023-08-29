#include <filesystem>
#include <fstream>
#include <lib/renderer/shader-module.hxx>
#include <vector>

namespace global_ao {
ShaderModule::ShaderModule(const std::filesystem::path& shaderPath, const Device& device)
  : device { device },
    code { readCode(shaderPath) },
    shaderModule { createShaderModule() } {
}

auto ShaderModule::readCode(const std::filesystem::path& shaderPath) -> std::vector<char> {
    auto file = std::ifstream { shaderPath, std::ios::ate | std::ios::binary };

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    const auto fileSize = static_cast<size_t>(file.tellg());

    auto buffer = std::vector<char> {};
    buffer.resize(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

auto ShaderModule::createShaderModule() -> vk::raii::ShaderModule {
    const auto& _device = device.getLogicalDevice();

    const auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo {
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data()),
    };

    auto _shaderModule = vk::raii::ShaderModule { _device, shaderModuleCreateInfo };

    return _shaderModule;
}
}  // namespace global_ao