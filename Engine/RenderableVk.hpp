/*
MIT License

Copyright (c) 2018 Ben Brown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <glm/common.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

#include "Renderable.hpp"

class DriverVk;
class RenderableVk : public Renderable {
public:
    RenderableVk(DriverVk* pDriver);
    bool attachShader(const char* filename, ShaderStage stage) override;
    bool execute() override;
    bool setIndexBuffer(std::vector<uint16_t> indices) override;
    bool setVertexBuffer(std::vector<uint32_t> vertices) override;
private:
    DriverVk* m_pDriver;
    vk::UniqueCommandBuffer m_pCommandBuffer;
    vk::UniquePipeline m_Pipeline;
    vk::UniqueRenderPass m_RenderPass;
    std::vector<vk::PipelineShaderStageCreateInfo> m_ShaderStages;
};