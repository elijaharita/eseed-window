// Copyright (c) 2020 Elijah Seed Arita
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.

#include <vulkan/vulkan.hpp>
#include <eseed/window/vulkanwindow.hpp>
#include <fstream>
#include <iostream>

using esd::wnd::Key;

// Create shader module with .spv code at path
vk::ShaderModule loadShaderModule(vk::Device device, std::string path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Could not open spv");

    std::vector<uint32_t> code;

    while (true) {
        
        uint32_t i = 0;
        file.read((char*)&i, 4);

        if (file.eof()) break;

        code.push_back(i);
    }

    file.close();

    return device.createShaderModule(
        vk::ShaderModuleCreateInfo()
            .setCodeSize(code.size() * 4)
            .setPCode(code.data())
    );
}

int main() {
    esd::wnd::VulkanWindow window("ウィンドウ", { 1366, 768 });

    window.keyHandler = [&](esd::wnd::KeyEvent e) {
        // Close window when Escape is pressed
        if (e.key == Key::Esc) window.setCloseRequested(true);

        // Toggle fullscreen when F11 is pressed
        if (e.key == Key::F11) window.setFullscreen(!window.isFullscreen());
    };

    // CREATE INSTANCE

    std::vector<const char*> instanceExtensions = window.getRequiredSurfaceInstanceExtensions();
    std::vector<const char*> instanceLayers = { "VK_LAYER_KHRONOS_validation" };

    auto availableInstanceExtensions = 
        vk::enumerateInstanceExtensionProperties();

    auto instanceCi = vk::InstanceCreateInfo()
        .setEnabledExtensionCount((uint32_t)instanceExtensions.size())
        .setPpEnabledExtensionNames(instanceExtensions.data())
        .setEnabledLayerCount((uint32_t)instanceLayers.size())
        .setPpEnabledLayerNames(instanceLayers.data());
        
    auto instance = vk::createInstance(instanceCi);

    // CREATE SURFACE

    auto surface = window.createSurface(instance);

    // CHOOSE PHYSICAL DEVICE
    // This is a simple test so just choose the first one

    auto physicalDevices = instance.enumeratePhysicalDevices();
    auto physicalDevice = physicalDevices[0];

    // FIND QUEUE FAMILY INDICES

    uint32_t graphicsQueueFamilyIndex;
    bool graphicsQueueFamilyIndexFound = false;

    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
            graphicsQueueFamilyIndex = i;
            graphicsQueueFamilyIndexFound = true;
            break;
        }
    }

    if (!graphicsQueueFamilyIndexFound)
        throw std::runtime_error("Could not find a graphics and present queue family");

    // CREATE DEVICE

    std::vector<const char*> deviceExtensions = { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME 
    };

    std::vector<vk::DeviceQueueCreateInfo> queueCis;

    // -- graphics queue
    float graphicsQueuePriorities[] = { 1.f };
    queueCis.push_back(
        vk::DeviceQueueCreateInfo()
            .setQueueCount(1)
            .setQueueFamilyIndex(graphicsQueueFamilyIndex)
            .setPQueuePriorities(graphicsQueuePriorities)
    );
    
    auto device = physicalDevice.createDevice(
        vk::DeviceCreateInfo()
            .setEnabledExtensionCount((uint32_t)deviceExtensions.size())
            .setPpEnabledExtensionNames(deviceExtensions.data())
            .setQueueCreateInfoCount((uint32_t)queueCis.size())
            .setPQueueCreateInfos(queueCis.data())        
    );

    // CREATE SWAPCHAIN
    auto surfaceFormat = physicalDevice.getSurfaceFormatsKHR(surface)[0];
    auto swapchain = device.createSwapchainKHR(
        vk::SwapchainCreateInfoKHR()
            .setClipped(true)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setImageArrayLayers(1)
            .setImageColorSpace(surfaceFormat.colorSpace)
            .setImageExtent({ (uint32_t)window.getSize().w, (uint32_t)window.getSize().h })
            .setImageFormat(surfaceFormat.format)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setMinImageCount(2)
            .setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(&graphicsQueueFamilyIndex)
            .setPresentMode(vk::PresentModeKHR::eFifo)
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setSurface(surface)
    );

    // CREATE RENDER PASS

    std::vector<vk::AttachmentDescription> renderPassAttachments;
    renderPassAttachments.push_back( // Color
        vk::AttachmentDescription()
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
            .setFormat(vk::Format::eB8G8R8A8Unorm)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
    );

    std::vector<vk::SubpassDescription> renderPassSubpasses;

    std::vector<vk::AttachmentReference> mainSubpassColorAttachments;
    mainSubpassColorAttachments.push_back(
        vk::AttachmentReference()
            .setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
    );
    
    renderPassSubpasses.push_back(
        vk::SubpassDescription()
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachmentCount((uint32_t)mainSubpassColorAttachments.size())
            .setPColorAttachments(mainSubpassColorAttachments.data())
    );
    
    auto renderPass = device.createRenderPass(
        vk::RenderPassCreateInfo()
            .setAttachmentCount((uint32_t)renderPassAttachments.size())
            .setPAttachments(renderPassAttachments.data())
            .setSubpassCount((uint32_t)renderPassSubpasses.size())
            .setPSubpasses(renderPassSubpasses.data())
    );
    
    // CREATE SHADER STAGES

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    auto vertShaderModule = loadShaderModule(device, "examples/vulkan/shaders/vulkan.vert.spv");
    shaderStages.push_back(
        vk::PipelineShaderStageCreateInfo()
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertShaderModule)
    );

    auto fragShaderModule = loadShaderModule(device, "examples/vulkan/shaders/vulkan.frag.spv");
    shaderStages.push_back(
        vk::PipelineShaderStageCreateInfo()
            .setPName("main")
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragShaderModule)
    );

    // CREATE GRAPHICS PIPELINE LAYOUT

    auto pipelineLayout = device.createPipelineLayout(
        vk::PipelineLayoutCreateInfo()
    );

    // CREATE GRAPHICS PIPELINE

    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.push_back(vk::PipelineColorBlendAttachmentState()
        .setBlendEnable(false)
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
        )
    );

    auto pipeline = device.createGraphicsPipeline(
        nullptr,
        vk::GraphicsPipelineCreateInfo()
            .setRenderPass(renderPass)
            .setSubpass(0)
            .setLayout(pipelineLayout)
            .setPColorBlendState(&vk::PipelineColorBlendStateCreateInfo()
                .setAttachmentCount((uint32_t)colorBlendAttachments.size())
                .setPAttachments(colorBlendAttachments.data())
            )
            .setPInputAssemblyState(&vk::PipelineInputAssemblyStateCreateInfo()
                .setPrimitiveRestartEnable(false)
                .setTopology(vk::PrimitiveTopology::eTriangleList)
            )
            .setPMultisampleState(&vk::PipelineMultisampleStateCreateInfo()
                .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            )
            .setPRasterizationState(&vk::PipelineRasterizationStateCreateInfo()
                .setLineWidth(1)
                .setFrontFace(vk::FrontFace::eClockwise)
                .setCullMode(vk::CullModeFlagBits::eBack)
                .setPolygonMode(vk::PolygonMode::eFill)
            )
            .setStageCount((uint32_t)shaderStages.size())
            .setPStages(shaderStages.data())
            .setPVertexInputState(&vk::PipelineVertexInputStateCreateInfo()
                .setVertexAttributeDescriptionCount(0)
                .setVertexBindingDescriptionCount(0)
            )
            .setPViewportState(&vk::PipelineViewportStateCreateInfo()
                .setScissorCount(1)
                .setPScissors(&vk::Rect2D()
                    .setExtent({
                        (uint32_t)window.getSize().w, 
                        (uint32_t)window.getSize().h 
                    })
                )
                .setViewportCount(1)
                .setPViewports(&vk::Viewport()
                    .setWidth((float)window.getSize().w)
                    .setHeight((float)window.getSize().h)
                    .setMinDepth(0)
                    .setMaxDepth(1)
                )
            )
    );

    // CREATE IMAGE VIEWS

    auto swapchainImages = device.getSwapchainImagesKHR(swapchain);
    
    std::vector<vk::ImageView> swapchainImageViews(swapchainImages.size());
    for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        swapchainImageViews[i] = device.createImageView(
            vk::ImageViewCreateInfo()
                .setComponents({
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                })
                .setFormat(vk::Format::eB8G8R8A8Unorm)
                .setImage(swapchainImages[i])
                .setSubresourceRange(vk::ImageSubresourceRange()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setBaseMipLevel(0)
                    .setLayerCount(1)
                    .setLevelCount(1)
                )
                .setViewType(vk::ImageViewType::e2D)
        );
    }

    // CREATE FRAMEBUFFERS

    std::vector<vk::Framebuffer> framebuffers(swapchainImageViews.size());
    for (size_t i = 0; i < framebuffers.size(); i++) {
        framebuffers[i] = device.createFramebuffer(
            vk::FramebufferCreateInfo()
                .setAttachmentCount(1)
                .setPAttachments(&swapchainImageViews[i])
                .setWidth(window.getSize().w)
                .setHeight(window.getSize().h)
                .setLayers(1)
                .setRenderPass(renderPass)
        );
    }

    // CREATE COMMAND POOL AND BUFFERS

    auto commandPool = device.createCommandPool(vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(graphicsQueueFamilyIndex)
    );

    std::vector<vk::CommandBuffer> commandBuffers = 
        device.allocateCommandBuffers(
            vk::CommandBufferAllocateInfo()
                .setCommandBufferCount((uint32_t)swapchainImageViews.size())
                .setCommandPool(commandPool)
                .setLevel(vk::CommandBufferLevel::ePrimary)
        );
        
    for (size_t i = 0; i < commandBuffers.size(); i++) {
        commandBuffers[i].begin(vk::CommandBufferBeginInfo());
        commandBuffers[i].beginRenderPass(
            vk::RenderPassBeginInfo()
                .setClearValueCount(1)
                .setPClearValues(&vk::ClearValue()
                    .setColor(vk::ClearColorValue()
                        .setFloat32({0.f, 0.f, 0.f, 1.f})
                    )
                )
                .setRenderArea(vk::Rect2D()
                    .setExtent({ 
                        (uint32_t)window.getSize().w, 
                        (uint32_t)window.getSize().h 
                    })
                )
                .setRenderPass(renderPass)
                .setFramebuffer(framebuffers[i]),
            vk::SubpassContents::eInline
        );
        commandBuffers[i].bindPipeline(
            vk::PipelineBindPoint::eGraphics, 
            pipeline
        );
        commandBuffers[i].draw(3, 1, 0, 0);
        commandBuffers[i].endRenderPass();
        commandBuffers[i].end();
    }    

    // SEMAPHORES

    auto swapchainImageReady = device.createSemaphore(
        vk::SemaphoreCreateInfo()
    );
    auto renderDone = device.createSemaphore(vk::SemaphoreCreateInfo());

    // WINDOW LOOP

    vk::Queue graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
    while (!window.isCloseRequested()) {

        // Render

        uint32_t swapchainImageIndex = device.acquireNextImageKHR(
            swapchain, 
            UINT64_MAX, 
            swapchainImageReady, 
            nullptr
        ).value;
        
        vk::PipelineStageFlags stageMasks = 
            vk::PipelineStageFlagBits::eTopOfPipe;

        graphicsQueue.submit(
            {
                vk::SubmitInfo()
                    .setCommandBufferCount(1)
                    .setPCommandBuffers(&commandBuffers[swapchainImageIndex])
                    .setSignalSemaphoreCount(1)
                    .setPSignalSemaphores(&renderDone)
                    .setWaitSemaphoreCount(1)
                    .setPWaitSemaphores(&swapchainImageReady)
                    .setPWaitDstStageMask(&stageMasks)
            },
            nullptr
        );

        graphicsQueue.presentKHR(
            vk::PresentInfoKHR()
                .setSwapchainCount(1)
                .setPSwapchains(&swapchain)
                .setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&renderDone)
                .setPImageIndices(&swapchainImageIndex)
        );

        graphicsQueue.waitIdle();
        
        // Poll for window events
        
        window.poll();
    }

    device.waitIdle();

    device.destroySemaphore(renderDone);
    device.destroySemaphore(swapchainImageReady);
    device.freeCommandBuffers(commandPool, commandBuffers);
    device.destroyCommandPool(commandPool);
    for (const auto& it : framebuffers) device.destroyFramebuffer(it);
    for (const auto& it : swapchainImageViews) device.destroyImageView(it);
    device.destroyPipeline(pipeline);
    device.destroyShaderModule(vertShaderModule);
    device.destroyShaderModule(fragShaderModule);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);
    device.destroySwapchainKHR(swapchain);
    device.destroy();
    instance.destroySurfaceKHR(surface);

    // Close the window after destroying the surface but before destroying the
    // instance
    window.close();
    
    instance.destroy();
}