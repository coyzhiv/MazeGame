#include "VulkanContext.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <array>
#include <cstring>
#include <cmath>

QueueFamilyIndices VulkanContext::findQueueFamilies()
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        nullptr
    );

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice,
        &queueFamilyCount,
        queueFamilies.data()
    );

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        const auto& queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice,
            i,
            surface,
            &presentSupport
        );

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }
    }

    return indices;
}

void VulkanContext::initialize(GLFWwindow* window)
{
    this->window = window;

    if (maze != nullptr)
    {
        float mazeCenterX =
            (maze->getWidth() - 1) / 2.0f;

        float mazeCenterZ =
            (maze->getHeight() - 1) / 2.0f;

        float startX =
            static_cast<float>(maze->getStartX()) - mazeCenterX;

        float startZ =
            static_cast<float>(maze->getStartY()) - mazeCenterZ;

        camera.setPosition(
            glm::vec3(startX, 0.6f, startZ)
        );
    }

    glfwSetWindowUserPointer(
        window,
        this
    );

    glfwSetCursorPosCallback(
        window,
        VulkanContext::mouseCallback
    );

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "MazeGame";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "MazeGame Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;

    const char** glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    VkResult result = vkCreateInstance(
        &createInfo,
        nullptr,
        &instance
    );

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    if (glfwCreateWindowSurface(
            instance,
            window,
            nullptr,
            &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }

    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createDepthResources();
    createRenderPass();
    createFramebuffers();
    createCommandPool();

    createTextureImage(
        "assets/floor.png",
        floorTextureImage,
        floorTextureImageMemory
    );
    createTextureImage(
        "assets/wall.png",
        wallTextureImage,
        wallTextureImageMemory
    );
    createTextureImage(
        "assets/ceiling.png",
        ceilingTextureImage,
        ceilingTextureImageMemory
    );

    createTextureImageView(
        floorTextureImage,
        floorTextureImageView
    );
    createTextureImageView(
        wallTextureImage,
        wallTextureImageView
    );
    createTextureImageView(
        ceilingTextureImage,
        ceilingTextureImageView
    );

    createTextureSampler(
        floorTextureSampler
    );
    createTextureSampler(
        wallTextureSampler
    );
    createTextureSampler(
        ceilingTextureSampler
    );

    createDescriptorSetLayout();
    createCameraUniformBuffer();
    createDescriptorPool();
    createDescriptorSet();
    createGraphicsPipeline();
    createVertexBuffer();
    createCommandBuffers();
    recordCommandBuffers();
    createSyncObjects();
    
    
}

void VulkanContext::cleanup()
{
    if (device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device);
    }
    
    if (device != VK_NULL_HANDLE)
    {
        // Floor texture
        vkDestroySampler(
            device,
            floorTextureSampler,
            nullptr
        );

        vkDestroyImageView(
            device,
            floorTextureImageView,
            nullptr
        );

        vkDestroyImage(
            device,
            floorTextureImage,
            nullptr
        );

        vkFreeMemory(
            device,
            floorTextureImageMemory,
            nullptr
        );

        // Wall texture
        vkDestroySampler(
            device,
            wallTextureSampler,
            nullptr
        );

        vkDestroyImageView(
            device,
            wallTextureImageView,
            nullptr
        );

        vkDestroyImage(
            device,
            wallTextureImage,
            nullptr
        );

        vkFreeMemory(
            device,
            wallTextureImageMemory,
            nullptr
        );

        // Ceiling texture
        vkDestroySampler(
            device,
            ceilingTextureSampler,
            nullptr
        );

        vkDestroyImageView(
            device,
            ceilingTextureImageView,
            nullptr
        );

        vkDestroyImage(
            device,
            ceilingTextureImage,
            nullptr
        );

        vkFreeMemory(
            device,
            ceilingTextureImageMemory,
            nullptr
        );

        vkDestroyDevice(
            device,
            nullptr
        );

        device = VK_NULL_HANDLE;
    }

    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(
            instance,
            surface,
            nullptr
        );

        surface = VK_NULL_HANDLE;
    }

    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(
            instance,
            nullptr
        );

        instance = VK_NULL_HANDLE;
    }
}

void VulkanContext::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        nullptr
    );

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find a GPU with Vulkan support");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        devices.data()
    );

    physicalDevice = devices[0];

    QueueFamilyIndices indices = findQueueFamilies();

    SwapchainSupportDetails swapchainSupport =
        querySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapchainSupport.formats);

    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapchainSupport.presentModes);

    std::cout << "Selected present mode: "
            << presentMode
            << '\n';

    std::cout << "Selected surface format: "
            << surfaceFormat.format
            << '\n';

    std::cout << "Surface formats: "
            << swapchainSupport.formats.size()
            << '\n';

    std::cout << "Present modes: "
            << swapchainSupport.presentModes.size()
            << '\n';

    std::cout << "Min image count: "
            << swapchainSupport.capabilities.minImageCount
            << '\n';

    std::cout << "Max image count: "
            << swapchainSupport.capabilities.maxImageCount
            << '\n';

        if (!indices.isComplete())
        {
            throw std::runtime_error(
                "Failed to find required queue families"
            );
        }

        std::cout << "Graphics queue family: "
                << indices.graphicsFamily.value()
                << '\n';

        std::cout << "Present queue family: "
                << indices.presentFamily.value()
                << '\n';
    
    VkPhysicalDeviceProperties properties{};

    vkGetPhysicalDeviceProperties(
        physicalDevice,
        &properties
    );

    std::cout << "GPU: " << properties.deviceName << '\n';
}

void VulkanContext::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies();

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(
            physicalDevice,
            &createInfo,
            nullptr,
            &device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    vkGetDeviceQueue(
        device,
        indices.graphicsFamily.value(),
        0,
        &graphicsQueue
    );

    vkGetDeviceQueue(
        device,
        indices.presentFamily.value(),
        0,
        &presentQueue
    );
}

SwapchainSupportDetails VulkanContext::querySwapchainSupport()
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice,
        surface,
        &details.capabilities
    );

    uint32_t formatCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &formatCount,
        nullptr
    );

    if (formatCount > 0)
    {
        details.formats.resize(formatCount);

        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice,
            surface,
            &formatCount,
            details.formats.data()
        );
    }

    uint32_t presentModeCount = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physicalDevice,
        surface,
        &presentModeCount,
        nullptr
    );

    if (presentModeCount > 0)
    {
        details.presentModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice,
            surface,
            &presentModeCount,
            details.presentModes.data()
        );
    }

    return details;
}

VkSurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR VulkanContext::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& presentModes)
{
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanContext::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    int width;
    int height;

    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent{
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );

    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return actualExtent;
}

void VulkanContext::createSwapchain()
{
    SwapchainSupportDetails swapchainSupport =
        querySwapchainSupport();

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapchainSupport.formats);

    VkPresentModeKHR presentMode =
        chooseSwapPresentMode(swapchainSupport.presentModes);

    VkExtent2D extent =
        chooseSwapExtent(swapchainSupport.capabilities);

    uint32_t imageCount =
        swapchainSupport.capabilities.minImageCount + 1;

    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapchainSupport.capabilities.maxImageCount)
    {
        imageCount =
            swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType =
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;

    createInfo.imageUsage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices =
        findQueueFamilies();

    uint32_t queueFamilyIndices[] = {
        indices.graphicsFamily.value(),
        indices.presentFamily.value()
    };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode =
            VK_SHARING_MODE_CONCURRENT;

        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices =
            queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode =
            VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform =
        swapchainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha =
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(
            device,
            &createInfo,
            nullptr,
            &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create swapchain"
        );
    }

    vkGetSwapchainImagesKHR(
        device,
        swapchain,
        &imageCount,
        nullptr
    );

    swapchainImages.resize(imageCount);

    vkGetSwapchainImagesKHR(
        device,
        swapchain,
        &imageCount,
        swapchainImages.data()
    );

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    std::cout << "Swapchain created. Images: "
              << swapchainImages.size()
              << '\n';
}

void VulkanContext::createImageViews()
{
    swapchainImageViews.resize(swapchainImages.size());

    for (size_t i = 0; i < swapchainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType =
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        createInfo.image = swapchainImages[i];

        createInfo.viewType =
            VK_IMAGE_VIEW_TYPE_2D;

        createInfo.format =
            swapchainImageFormat;

        createInfo.components.r =
            VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g =
            VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b =
            VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a =
            VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;

        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;

        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(
                device,
                &createInfo,
                nullptr,
                &swapchainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create image view"
            );
        }
    }

    

    std::cout << "Image views created: "
              << swapchainImageViews.size()
              << '\n';
}

void VulkanContext::createRenderPass()
{
    // Color attachment
    VkAttachmentDescription colorAttachment{};

    colorAttachment.format =
        swapchainImageFormat;

    colorAttachment.samples =
        VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp =
        VK_ATTACHMENT_LOAD_OP_CLEAR;

    colorAttachment.storeOp =
        VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    colorAttachment.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    colorAttachment.finalLayout =
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    // Depth attachment
    VkAttachmentDescription depthAttachment{};

    depthAttachment.format =
        findDepthFormat();

    depthAttachment.samples =
        VK_SAMPLE_COUNT_1_BIT;

    depthAttachment.loadOp =
        VK_ATTACHMENT_LOAD_OP_CLEAR;

    depthAttachment.storeOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachment.stencilLoadOp =
        VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    depthAttachment.stencilStoreOp =
        VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depthAttachment.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    depthAttachment.finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // Color attachment reference
    VkAttachmentReference colorAttachmentRef{};

    colorAttachmentRef.attachment = 0;

    colorAttachmentRef.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    // Depth attachment reference
    VkAttachmentReference depthAttachmentRef{};

    depthAttachmentRef.attachment = 1;

    depthAttachmentRef.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // Subpass
    VkSubpassDescription subpass{};

    subpass.pipelineBindPoint =
        VK_PIPELINE_BIND_POINT_GRAPHICS;

    subpass.colorAttachmentCount = 1;

    subpass.pColorAttachments =
        &colorAttachmentRef;

    subpass.pDepthStencilAttachment =
        &depthAttachmentRef;


    // Attachments array
    VkAttachmentDescription attachments[] = {
        colorAttachment,
        depthAttachment
    };


    // Render pass
    VkRenderPassCreateInfo renderPassInfo{};

    renderPassInfo.sType =
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderPassInfo.attachmentCount = 2;

    renderPassInfo.pAttachments =
        attachments;

    renderPassInfo.subpassCount = 1;

    renderPassInfo.pSubpasses =
        &subpass;


    if (vkCreateRenderPass(
            device,
            &renderPassInfo,
            nullptr,
            &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create render pass"
        );
    }

    std::cout << "Render pass created." << '\n';
}


void VulkanContext::createFramebuffers()
{
    swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
            swapchainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};

        framebufferInfo.sType =
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        framebufferInfo.renderPass = renderPass;

        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;

        framebufferInfo.width = swapchainExtent.width;
        framebufferInfo.height = swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
                device,
                &framebufferInfo,
                nullptr,
                &swapchainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to create framebuffer"
            );
        }
    }

    std::cout << "Framebuffers created: "
              << swapchainFramebuffers.size()
              << '\n';
}

void VulkanContext::createCommandPool()
{
    QueueFamilyIndices queueFamilyIndices =
        findQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};

    poolInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    poolInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    poolInfo.queueFamilyIndex =
        queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(
            device,
            &poolInfo,
            nullptr,
            &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create command pool"
        );
    }

    std::cout << "Command pool created." << '\n';
}

void VulkanContext::createCommandBuffers()
{
    commandBuffers.resize(swapchainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    allocInfo.commandPool = commandPool;

    allocInfo.level =
        VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandBufferCount =
        static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(
            device,
            &allocInfo,
            commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate command buffers"
        );
    }

    std::cout << "Command buffers created: "
              << commandBuffers.size()
              << '\n';
}

void VulkanContext::recordCommandBuffers()
{
    

    
    
    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        
        
        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType =
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(
                commandBuffers[i],
                &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to begin recording command buffer"
            );
        }

        VkClearValue clearValues[2]{};

        clearValues[0].color = {
            0.05f,
            0.05f,
            0.08f,
            1.0f
        };

        clearValues[1].depthStencil = {
            1.0f,
            0
        };

        VkRenderPassBeginInfo renderPassInfo{};

        renderPassInfo.clearValueCount = 2;

renderPassInfo.pClearValues = clearValues;

        renderPassInfo.sType =
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        renderPassInfo.renderPass = renderPass;

        renderPassInfo.framebuffer =
            swapchainFramebuffers[i];

        renderPassInfo.renderArea.offset = {0, 0};

        renderPassInfo.renderArea.extent =
            swapchainExtent;

        renderPassInfo.clearValueCount = 2;

        renderPassInfo.pClearValues =
            clearValues;

        vkCmdBeginRenderPass(
            commandBuffers[i],
            &renderPassInfo,
            VK_SUBPASS_CONTENTS_INLINE
        );

        vkCmdBindPipeline(
            commandBuffers[i],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphicsPipeline
        );

        VkBuffer vertexBuffers[] = {
            vertexBuffer
        };

        VkDeviceSize offsets[] = {
            0
        };

        vkCmdBindVertexBuffers(
            commandBuffers[i],
            0,
            1,
            vertexBuffers,
            offsets
        );


        vkCmdBindDescriptorSets(
            commandBuffers[i],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &descriptorSet,
            0,
            nullptr
        );

        vkCmdDraw(
            commandBuffers[i],
            static_cast<uint32_t>(vertices.size()),
            1,
            0,
            0
        );

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(
                commandBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to record command buffer"
            );
        }
    }

    std::cout << "Command buffers recorded." << '\n';
}

void VulkanContext::createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};

    semaphoreInfo.sType =
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};

    fenceInfo.sType =
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    fenceInfo.flags =
        VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(
            device,
            &semaphoreInfo,
            nullptr,
            &imageAvailableSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create image available semaphore"
        );
    }

    if (vkCreateSemaphore(
            device,
            &semaphoreInfo,
            nullptr,
            &renderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create render finished semaphore"
        );
    }

    if (vkCreateFence(
            device,
            &fenceInfo,
            nullptr,
            &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create in-flight fence"
        );
    }

    std::cout << "Synchronization objects created."
              << '\n';
}

void VulkanContext::drawFrame()
{
    vkWaitForFences(
        device,
        1,
        &inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    updateCameraUniformBuffer();

    vkResetFences(
        device,
        1,
        &inFlightFence
    );

    uint32_t imageIndex = 0;

    VkResult result = vkAcquireNextImageKHR(
        device,
        swapchain,
        UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error(
            "Failed to acquire swapchain image"
        );
    }

    VkSemaphore waitSemaphores[] = {
        imageAvailableSemaphore
    };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSubmitInfo submitInfo{};

    submitInfo.sType =
        VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.waitSemaphoreCount = 1;

    submitInfo.pWaitSemaphores =
        waitSemaphores;

    submitInfo.pWaitDstStageMask =
        waitStages;

    submitInfo.commandBufferCount = 1;

    submitInfo.pCommandBuffers =
        &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {
        renderFinishedSemaphore
    };

    submitInfo.signalSemaphoreCount = 1;

    submitInfo.pSignalSemaphores =
        signalSemaphores;

    if (vkQueueSubmit(
            graphicsQueue,
            1,
            &submitInfo,
            inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to submit draw command buffer"
        );
    }

    VkPresentInfoKHR presentInfo{};

    presentInfo.sType =
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pWaitSemaphores =
        signalSemaphores;

    VkSwapchainKHR swapchains[] = {
        swapchain
    };

    presentInfo.swapchainCount = 1;

    presentInfo.pSwapchains =
        swapchains;

    presentInfo.pImageIndices =
        &imageIndex;

    result = vkQueuePresentKHR(
        presentQueue,
        &presentInfo
    );

    if (result != VK_SUCCESS &&
        result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error(
            "Failed to present swapchain image"
        );
    }
}

std::vector<char> VulkanContext::readFile(
    const std::string& filename)
{
    std::ifstream file(
        filename,
        std::ios::ate | std::ios::binary
    );

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open shader file: " + filename
        );
    }

    size_t fileSize =
        static_cast<size_t>(file.tellg());

    std::vector<char> buffer(fileSize);

    file.seekg(0);

    file.read(
        buffer.data(),
        fileSize
    );

    file.close();

    return buffer;
}

VkShaderModule VulkanContext::createShaderModule(
    const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};

    createInfo.sType =
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    createInfo.codeSize =
        code.size();

    createInfo.pCode =
        reinterpret_cast<const uint32_t*>(
            code.data()
        );

    VkShaderModule shaderModule;

    if (vkCreateShaderModule(
            device,
            &createInfo,
            nullptr,
            &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create shader module"
        );
    }

    return shaderModule;
}

void VulkanContext::createGraphicsPipeline()
{
    auto vertShaderCode =
        readFile("shaders/compiled/basic.vert.spv");

    auto fragShaderCode =
        readFile("shaders/compiled/basic.frag.spv");

    VkShaderModule vertShaderModule =
        createShaderModule(vertShaderCode);

    VkShaderModule fragShaderModule =
        createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};

    vertShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    vertShaderStageInfo.stage =
        VK_SHADER_STAGE_VERTEX_BIT;

    vertShaderStageInfo.module =
        vertShaderModule;

    vertShaderStageInfo.pName =
        "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};

    fragShaderStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    fragShaderStageInfo.stage =
        VK_SHADER_STAGE_FRAGMENT_BIT;

    fragShaderStageInfo.module =
        fragShaderModule;

    fragShaderStageInfo.pName =
        "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };


    // Vertex input

    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[4]{};

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format =
        VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset =
        offsetof(Vertex, pos);

    // UV
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format =
        VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset =
        offsetof(Vertex, uv);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format =
        VK_FORMAT_R32_SFLOAT;
    attributeDescriptions[2].offset =
        offsetof(Vertex, textureIndex);

    // Normal
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format =
        VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset =
        offsetof(Vertex, normal);


    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

    vertexInputInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = 1;

    vertexInputInfo.pVertexBindingDescriptions =
        &bindingDescription;

    vertexInputInfo.vertexAttributeDescriptionCount = 4;
    vertexInputInfo.pVertexAttributeDescriptions =
        attributeDescriptions;


    // Input assembly

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

    inputAssembly.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    inputAssembly.topology =
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    inputAssembly.primitiveRestartEnable =
        VK_FALSE;

    VkViewport viewport{};

    viewport.x = 0.0f;
    viewport.y = 0.0f;

    viewport.width =
        static_cast<float>(swapchainExtent.width);

    viewport.height =
        static_cast<float>(swapchainExtent.height);

    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};

    scissor.offset = {0, 0};

    scissor.extent =
        swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};

    viewportState.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    viewportState.viewportCount = 1;

    viewportState.pViewports =
        &viewport;

    viewportState.scissorCount = 1;

    viewportState.pScissors =
        &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};

    rasterizer.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    rasterizer.depthClampEnable =
        VK_FALSE;

    rasterizer.rasterizerDiscardEnable =
        VK_FALSE;

    rasterizer.polygonMode =
        VK_POLYGON_MODE_FILL;

    rasterizer.lineWidth =
        1.0f;

    rasterizer.cullMode = VK_CULL_MODE_NONE;
    
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable =
        VK_FALSE;
    
    VkPipelineMultisampleStateCreateInfo multisampling{};

    multisampling.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    multisampling.sampleShadingEnable =
        VK_FALSE;

    multisampling.rasterizationSamples =
        VK_SAMPLE_COUNT_1_BIT;
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};

    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;

    colorBlendAttachment.blendEnable =
        VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};

    colorBlending.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    colorBlending.logicOpEnable =
        VK_FALSE;

    colorBlending.logicOp =
        VK_LOGIC_OP_COPY;

    colorBlending.attachmentCount = 1;

    colorBlending.pAttachments =
        &colorBlendAttachment;

    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

    pipelineLayoutInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipelineLayoutInfo.setLayoutCount = 1;

    pipelineLayoutInfo.pSetLayouts =
        &descriptorSetLayout;

    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(
            device,
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create pipeline layout"
        );
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};

    depthStencil.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    depthStencil.depthTestEnable = VK_TRUE;

    depthStencil.depthWriteEnable = VK_TRUE;

    depthStencil.depthCompareOp =
        VK_COMPARE_OP_LESS;

    depthStencil.depthBoundsTestEnable = VK_FALSE;

    depthStencil.stencilTestEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.sType =
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = 2;

    pipelineInfo.pStages =
        shaderStages;

    pipelineInfo.pVertexInputState =
        &vertexInputInfo;

    pipelineInfo.pInputAssemblyState =
        &inputAssembly;

    pipelineInfo.pViewportState =
        &viewportState;

    pipelineInfo.pRasterizationState =
        &rasterizer;

    pipelineInfo.pMultisampleState =
        &multisampling;

    pipelineInfo.pDepthStencilState = 
        &depthStencil;

    pipelineInfo.pColorBlendState =
        &colorBlending;

    pipelineInfo.pDynamicState =
        nullptr;

    pipelineInfo.layout =
        pipelineLayout;

    pipelineInfo.renderPass =
        renderPass;

    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle =
        VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create graphics pipeline"
        );
    }

    std::cout << "Graphics pipeline created." << '\n';

    vkDestroyShaderModule(
        device,
        fragShaderModule,
        nullptr
    );

    vkDestroyShaderModule(
        device,
        vertShaderModule,
        nullptr
    );
};

uint32_t VulkanContext::findMemoryType(
    uint32_t typeFilter,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;

    vkGetPhysicalDeviceMemoryProperties(
        physicalDevice,
        &memoryProperties
    );

    for (uint32_t i = 0;
         i < memoryProperties.memoryTypeCount;
         i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties)
                == properties)
        {
            return i;
        }
    }

    throw std::runtime_error(
        "Failed to find suitable memory type"
    );
}

void VulkanContext::createVertexBuffer()
{
    VkDeviceSize bufferSize =
        sizeof(vertices[0]) * vertices.size();

    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType =
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferInfo.size =
        bufferSize;

    bufferInfo.usage =
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    bufferInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(
            device,
            &bufferInfo,
            nullptr,
            &vertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create vertex buffer"
        );
    }

    VkMemoryRequirements memoryRequirements;

    vkGetBufferMemoryRequirements(
        device,
        vertexBuffer,
        &memoryRequirements
    );

    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocInfo.allocationSize =
        memoryRequirements.size;

    allocInfo.memoryTypeIndex =
        findMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    if (vkAllocateMemory(
            device,
            &allocInfo,
            nullptr,
            &vertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate vertex buffer memory"
        );
    }

    vkBindBufferMemory(
        device,
        vertexBuffer,
        vertexBufferMemory,
        0
    );

    std::cout << "Vertex buffer created." << '\n';

    void* data;

    vkMapMemory(
        device,
        vertexBufferMemory,
        0,
        bufferSize,
        0,
        &data
    );

    memcpy(
        data,
        vertices.data(),
        static_cast<size_t>(bufferSize)
    );

    vkUnmapMemory(
        device,
        vertexBufferMemory
    );

    std::cout << "Vertex data uploaded." << '\n';
}

VkFormat VulkanContext::findDepthFormat()
{
        return VK_FORMAT_D32_SFLOAT;
}

void VulkanContext::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    VkImageCreateInfo imageInfo{};

    imageInfo.sType =
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    imageInfo.imageType =
        VK_IMAGE_TYPE_2D;

    imageInfo.format =
        depthFormat;

    imageInfo.extent.width =
        swapchainExtent.width;

    imageInfo.extent.height =
        swapchainExtent.height;

    imageInfo.extent.depth =
        1;

    imageInfo.mipLevels = 1;

    imageInfo.arrayLayers = 1;

    imageInfo.samples =
        VK_SAMPLE_COUNT_1_BIT;

    imageInfo.tiling =
        VK_IMAGE_TILING_OPTIMAL;

    imageInfo.usage =
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    imageInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(
            device,
            &imageInfo,
            nullptr,
            &depthImage) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create depth image"
        );
    }

    VkMemoryRequirements memoryRequirements;

    vkGetImageMemoryRequirements(
        device,
        depthImage,
        &memoryRequirements
    );

    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocInfo.allocationSize =
        memoryRequirements.size;

    allocInfo.memoryTypeIndex =
        findMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        );

    if (vkAllocateMemory(
            device,
            &allocInfo,
            nullptr,
            &depthImageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate depth image memory"
        );
    }

    vkBindImageMemory(
        device,
        depthImage,
        depthImageMemory,
        0
    );

    VkImageViewCreateInfo viewInfo{};

    viewInfo.sType =
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    viewInfo.image =
        depthImage;

    viewInfo.viewType =
        VK_IMAGE_VIEW_TYPE_2D;

    viewInfo.format =
        depthFormat;

    viewInfo.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;

    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
            device,
            &viewInfo,
            nullptr,
            &depthImageView) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create depth image view"
        );
    }

    std::cout << "Depth resources created." << '\n';
}

void VulkanContext::createImage(
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkImage& image,
    VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};

    imageInfo.sType =
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    imageInfo.imageType =
        VK_IMAGE_TYPE_2D;

    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;

    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;

    imageInfo.tiling = tiling;

    imageInfo.initialLayout =
        VK_IMAGE_LAYOUT_UNDEFINED;

    imageInfo.usage = usage;

    imageInfo.samples =
        VK_SAMPLE_COUNT_1_BIT;

    imageInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(
            device,
            &imageInfo,
            nullptr,
            &image) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create image"
        );
    }

    VkMemoryRequirements memoryRequirements;

    vkGetImageMemoryRequirements(
        device,
        image,
        &memoryRequirements
    );

    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocInfo.allocationSize =
        memoryRequirements.size;

    allocInfo.memoryTypeIndex =
        findMemoryType(
            memoryRequirements.memoryTypeBits,
            properties
        );

    if (vkAllocateMemory(
            device,
            &allocInfo,
            nullptr,
            &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate image memory"
        );
    }

    vkBindImageMemory(
        device,
        image,
        imageMemory,
        0
    );
}

void VulkanContext::createTextureImage(
    const std::string& filename,
    VkImage& image,
    VkDeviceMemory& imageMemory)
{
    TextureData texture =
        loadTexture(filename);

    uint32_t textureWidth =
        static_cast<uint32_t>(texture.width);

    uint32_t textureHeight =
        static_cast<uint32_t>(texture.height);

    VkDeviceSize imageSize =
        static_cast<VkDeviceSize>(
            textureWidth
        ) *
        static_cast<VkDeviceSize>(
            textureHeight
        ) *
        4;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory =
        VK_NULL_HANDLE;

    createBuffer(
        imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data = nullptr;

    vkMapMemory(
        device,
        stagingBufferMemory,
        0,
        imageSize,
        0,
        &data
    );

    memcpy(
        data,
        texture.pixels,
        static_cast<size_t>(imageSize)
    );

    vkUnmapMemory(
        device,
        stagingBufferMemory
    );

    createImage(
        textureWidth,
        textureHeight,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        image,
        imageMemory
    );

    freeTexture(texture);

    transitionImageLayout(
        image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    );

    copyBufferToImage(
        stagingBuffer,
        image,
        textureWidth,
        textureHeight
    );

    transitionImageLayout(
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    vkDestroyBuffer(
        device,
        stagingBuffer,
        nullptr
    );

    vkFreeMemory(
        device,
        stagingBufferMemory,
        nullptr
    );

    std::cout
        << "Texture image created: "
        << filename
        << '\n';
}

void VulkanContext::transitionImageLayout(
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout)
{
    VkCommandBufferAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    allocInfo.level =
        VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandPool =
        commandPool;

    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;

    vkAllocateCommandBuffers(
        device,
        &allocInfo,
        &commandBuffer
    );

    VkCommandBufferBeginInfo beginInfo{};

    beginInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(
        commandBuffer,
        &beginInfo
    );

    VkImageMemoryBarrier barrier{};

    barrier.sType =
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex =
        VK_QUEUE_FAMILY_IGNORED;

    barrier.dstQueueFamilyIndex =
        VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;

    barrier.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;

    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;

        barrier.dstAccessMask =
            VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage =
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        destinationStage =
            VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (
        oldLayout ==
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        newLayout ==
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask =
            VK_ACCESS_TRANSFER_WRITE_BIT;

        barrier.dstAccessMask =
            VK_ACCESS_SHADER_READ_BIT;

        sourceStage =
            VK_PIPELINE_STAGE_TRANSFER_BIT;

        destinationStage =
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::runtime_error(
            "Unsupported image layout transition"
        );
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};

    submitInfo.sType =
        VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;

    submitInfo.pCommandBuffers =
        &commandBuffer;

    vkQueueSubmit(
        graphicsQueue,
        1,
        &submitInfo,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(
        device,
        commandPool,
        1,
        &commandBuffer
    );
}

void VulkanContext::copyBufferToImage(
    VkBuffer buffer,
    VkImage image,
    uint32_t width,
    uint32_t height)
{
    VkCommandBufferAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    allocInfo.level =
        VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandPool =
        commandPool;

    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;

    vkAllocateCommandBuffers(
        device,
        &allocInfo,
        &commandBuffer
    );

    VkCommandBufferBeginInfo beginInfo{};

    beginInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(
        commandBuffer,
        &beginInfo
    );

    VkBufferImageCopy region{};

    region.bufferOffset = 0;

    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;

    region.imageSubresource.mipLevel = 0;

    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};

    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};

    submitInfo.sType =
        VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submitInfo.commandBufferCount = 1;

    submitInfo.pCommandBuffers =
        &commandBuffer;

    vkQueueSubmit(
        graphicsQueue,
        1,
        &submitInfo,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(
        device,
        commandPool,
        1,
        &commandBuffer
    );
}

void VulkanContext::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType =
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferInfo.size = size;

    bufferInfo.usage = usage;

    bufferInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(
            device,
            &bufferInfo,
            nullptr,
            &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create buffer"
        );
    }

    VkMemoryRequirements memoryRequirements;

    vkGetBufferMemoryRequirements(
        device,
        buffer,
        &memoryRequirements
    );

    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocInfo.allocationSize =
        memoryRequirements.size;

    allocInfo.memoryTypeIndex =
        findMemoryType(
            memoryRequirements.memoryTypeBits,
            properties
        );

    if (vkAllocateMemory(
            device,
            &allocInfo,
            nullptr,
            &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate buffer memory"
        );
    }

    vkBindBufferMemory(
        device,
        buffer,
        bufferMemory,
        0
    );
}

void VulkanContext::createTextureImageView(
    VkImage image,
    VkImageView& imageView)
{
    VkImageViewCreateInfo viewInfo{};

    viewInfo.sType =
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    viewInfo.image =
        image;

    viewInfo.viewType =
        VK_IMAGE_VIEW_TYPE_2D;

    viewInfo.format =
        VK_FORMAT_R8G8B8A8_UNORM;

    viewInfo.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_COLOR_BIT;

    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;

    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
            device,
            &viewInfo,
            nullptr,
            &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create texture image view"
        );
    }
}

void VulkanContext::createTextureSampler(
    VkSampler& sampler)
{
    VkSamplerCreateInfo samplerInfo{};

    samplerInfo.sType =
        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

    samplerInfo.magFilter =
        VK_FILTER_NEAREST;

    samplerInfo.minFilter =
        VK_FILTER_NEAREST;

    samplerInfo.addressModeU =
        VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.addressModeV =
        VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.addressModeW =
        VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable =
        VK_TRUE;

    VkPhysicalDeviceProperties properties{};

    vkGetPhysicalDeviceProperties(
        physicalDevice,
        &properties
    );

    samplerInfo.maxAnisotropy =
        properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor =
        VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates =
        VK_FALSE;

    samplerInfo.compareEnable =
        VK_FALSE;

    samplerInfo.compareOp =
        VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode =
        VK_SAMPLER_MIPMAP_MODE_NEAREST;

    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(
            device,
            &samplerInfo,
            nullptr,
            &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create texture sampler"
        );
    }
}

void VulkanContext::createDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};

    samplerLayoutBinding.binding = 0;

    samplerLayoutBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    samplerLayoutBinding.descriptorCount = 3;

    samplerLayoutBinding.stageFlags =
        VK_SHADER_STAGE_FRAGMENT_BIT;

    samplerLayoutBinding.pImmutableSamplers =
        nullptr;

    VkDescriptorSetLayoutBinding cameraLayoutBinding{};

    cameraLayoutBinding.binding = 1;

    cameraLayoutBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    cameraLayoutBinding.descriptorCount = 1;

    cameraLayoutBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT;

    cameraLayoutBinding.pImmutableSamplers =
        nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        samplerLayoutBinding,
        cameraLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};

    layoutInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    layoutInfo.bindingCount =
        static_cast<uint32_t>(bindings.size());

    layoutInfo.pBindings =
        bindings.data();

    if (vkCreateDescriptorSetLayout(
            device,
            &layoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create descriptor set layout"
        );
    }

    std::cout
        << "Descriptor set layout created."
        << '\n';
}

void VulkanContext::createDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    poolSizes[0].type =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    poolSizes[0].descriptorCount = 3;

    poolSizes[1].type =
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};

    poolInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    poolInfo.poolSizeCount =
        static_cast<uint32_t>(poolSizes.size());

    poolInfo.pPoolSizes =
        poolSizes.data();

    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(
            device,
            &poolInfo,
            nullptr,
            &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create descriptor pool"
        );
    }

    std::cout
        << "Descriptor pool created."
        << '\n';
}

void VulkanContext::createDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocInfo{};

    allocInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    allocInfo.descriptorPool =
        descriptorPool;

    allocInfo.descriptorSetCount = 1;

    allocInfo.pSetLayouts =
        &descriptorSetLayout;

    if (vkAllocateDescriptorSets(
            device,
            &allocInfo,
            &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate descriptor set"
        );
    }


    // Texture descriptor

    std::array<VkDescriptorImageInfo, 3> imageInfos{};

    imageInfos[0].imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    imageInfos[0].imageView =
        floorTextureImageView;

    imageInfos[0].sampler =
        floorTextureSampler;


    imageInfos[1].imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    imageInfos[1].imageView =
        wallTextureImageView;

    imageInfos[1].sampler =
        wallTextureSampler;


    imageInfos[2].imageLayout =
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    imageInfos[2].imageView =
        ceilingTextureImageView;

    imageInfos[2].sampler =
        ceilingTextureSampler;


    // Camera UBO descriptor

    VkDescriptorBufferInfo cameraBufferInfo{};

    cameraBufferInfo.buffer =
        cameraUniformBuffer;

    cameraBufferInfo.offset = 0;

    cameraBufferInfo.range =
        sizeof(CameraUBO);


    // Descriptor writes

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};


    descriptorWrites[0].sType =
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    descriptorWrites[0].dstSet =
        descriptorSet;

    descriptorWrites[0].dstBinding = 0;

    descriptorWrites[0].dstArrayElement = 0;

    descriptorWrites[0].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    descriptorWrites[0].descriptorCount = 3;

    descriptorWrites[0].pImageInfo =
        imageInfos.data();


    descriptorWrites[1].sType =
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

    descriptorWrites[1].dstSet =
        descriptorSet;

    descriptorWrites[1].dstBinding = 1;

    descriptorWrites[1].dstArrayElement = 0;

    descriptorWrites[1].descriptorType =
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    descriptorWrites[1].descriptorCount = 1;

    descriptorWrites[1].pBufferInfo =
        &cameraBufferInfo;


    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(),
        0,
        nullptr
    );


    std::cout
        << "Descriptor set created."
        << '\n';
}

void VulkanContext::setVertices(
    const std::vector<Vertex>& newVertices)
{
    vertices = newVertices;
}

void VulkanContext::update(float deltaTime)
{
    static bool mWasPressed = false;

    bool mPressed =
        glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;

    if (mPressed && !mWasPressed)
    {
        debugMapVisible = !debugMapVisible;

        if (debugMapVisible)
        {
            printDebugMap();
        }
    }

    mWasPressed = mPressed;

    float forwardInput = 0.0f;
    float rightInput = 0.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        forwardInput += 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        forwardInput -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        rightInput += 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        rightInput -= 1.0f;
    }

    camera.setMovementInput(
        forwardInput,
        rightInput
    );

    glm::vec3 oldPosition =
        camera.getPosition();

    camera.update(deltaTime);

    glm::vec3 newPosition =
        camera.getPosition();

    const float playerRadius = 0.3f;

    if (maze == nullptr)
    {
        return;
    }

    const float mazeCenterX =
        static_cast<float>(
            maze->getWidth() - 1
        ) / 2.0f;

    const float mazeCenterZ =
        static_cast<float>(
            maze->getHeight() - 1
        ) / 2.0f;

    auto isPositionValid =
        [&](const glm::vec3& position)
        {
            for (uint32_t z = 0;
                 z < maze->getHeight();
                 ++z)
            {
                for (uint32_t x = 0;
                     x < maze->getWidth();
                     ++x)
                {
                    if (maze->isWalkable(x, z))
                    {
                        continue;
                    }

                    float wallCenterX =
                        static_cast<float>(x) -
                        mazeCenterX;

                    float wallCenterZ =
                        static_cast<float>(z) -
                        mazeCenterZ;

                    float wallMinX =
                        wallCenterX - 0.5f;

                    float wallMaxX =
                        wallCenterX + 0.5f;

                    float wallMinZ =
                        wallCenterZ - 0.5f;

                    float wallMaxZ =
                        wallCenterZ + 0.5f;

                    float closestX =
                        glm::clamp(
                            position.x,
                            wallMinX,
                            wallMaxX
                        );

                    float closestZ =
                        glm::clamp(
                            position.z,
                            wallMinZ,
                            wallMaxZ
                        );

                    float distanceX =
                        position.x - closestX;

                    float distanceZ =
                        position.z - closestZ;

                    float distanceSquared =
                        distanceX * distanceX +
                        distanceZ * distanceZ;

                    if (distanceSquared <
                        playerRadius * playerRadius)
                    {
                        return false;
                    }
                }
            }

            return true;
        };

    // Try X movement
    glm::vec3 testPosition =
        oldPosition;

    testPosition.x =
        newPosition.x;

    if (!isPositionValid(testPosition))
    {
        newPosition.x =
            oldPosition.x;
    }

    // Try Z movement
    testPosition =
        oldPosition;

    testPosition.z =
        newPosition.z;

    if (!isPositionValid(testPosition))
    {
        newPosition.z =
            oldPosition.z;
    }

    newPosition.y =
        oldPosition.y;

    camera.setPosition(
        newPosition
    );

    if (!victory && maze != nullptr)
    {
        float mazeCenterX =
            (maze->getWidth() - 1) / 2.0f;

        float mazeCenterZ =
            (maze->getHeight() - 1) / 2.0f;

        float exitX =
            static_cast<float>(maze->getExitX()) - mazeCenterX;

        float exitZ =
            static_cast<float>(maze->getExitY()) - mazeCenterZ;

        float dx = camera.getPosition().x - exitX;
        float dz = camera.getPosition().z - exitZ;

        float distanceSquared =
            dx * dx + dz * dz;

        if (distanceSquared < 0.5f * 0.5f)
        {
            if (!victory)
            {
                victory = true;
                victoryTime = static_cast<float>(glfwGetTime());

                std::cout << "VICTORY!" << std::endl;
            }
        }
    }
}

void VulkanContext::createCameraUniformBuffer()
{
    VkDeviceSize bufferSize =
        sizeof(CameraUBO);

    VkBufferCreateInfo bufferInfo{};

    bufferInfo.sType =
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferInfo.size =
        bufferSize;

    bufferInfo.usage =
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    bufferInfo.sharingMode =
        VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(
            device,
            &bufferInfo,
            nullptr,
            &cameraUniformBuffer
        ) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to create camera uniform buffer"
        );
    }

    VkMemoryRequirements memoryRequirements{};

    vkGetBufferMemoryRequirements(
        device,
        cameraUniformBuffer,
        &memoryRequirements
    );

    VkMemoryAllocateInfo allocateInfo{};

    allocateInfo.sType =
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    allocateInfo.allocationSize =
        memoryRequirements.size;

    allocateInfo.memoryTypeIndex =
        findMemoryType(
            memoryRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    if (vkAllocateMemory(
            device,
            &allocateInfo,
            nullptr,
            &cameraUniformBufferMemory
        ) != VK_SUCCESS)
    {
        throw std::runtime_error(
            "Failed to allocate camera uniform buffer memory"
        );
    }

    vkBindBufferMemory(
        device,
        cameraUniformBuffer,
        cameraUniformBufferMemory,
        0
    );
}

void VulkanContext::updateCameraUniformBuffer()
{
    CameraUBO cameraUBO{};

    cameraUBO.view =
        camera.getViewMatrix();

    cameraUBO.projection =
        glm::perspective(
            glm::radians(60.0f),
            static_cast<float>(swapchainExtent.width) /
            static_cast<float>(swapchainExtent.height),
            0.1f,
            100.0f
        );
    
    cameraUBO.time = static_cast<float>(glfwGetTime());
    cameraUBO.victoryTime =
    victory
        ? static_cast<float>(glfwGetTime()) - victoryTime
        : 0.0f;
    

    cameraUBO.projection[1][1] *= -1;

    void* data = nullptr;

    vkMapMemory(
        device,
        cameraUniformBufferMemory,
        0,
        sizeof(CameraUBO),
        0,
        &data
    );

    memcpy(
        data,
        &cameraUBO,
        sizeof(CameraUBO)
    );

    vkUnmapMemory(
        device,
        cameraUniformBufferMemory
    );
}

void VulkanContext::mouseCallback(
    GLFWwindow* window,
    double xpos,
    double ypos)
{
    VulkanContext* context =
        static_cast<VulkanContext*>(
            glfwGetWindowUserPointer(window)
        );

    if (context == nullptr)
    {
        return;
    }

    context->processMouseMovement(
        xpos,
        ypos
    );
}

void VulkanContext::processMouseMovement(
    double xpos,
    double ypos)
{
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;

        firstMouse = false;

        return;
    }

    float xOffset =
        static_cast<float>(
            xpos - lastMouseX
        );

    float yOffset =
        static_cast<float>(
            lastMouseY - ypos
        );

    lastMouseX = xpos;
    lastMouseY = ypos;

    camera.processMouseMovement(
        xOffset,
        yOffset
    );
}

void VulkanContext::setMaze(const Maze* newMaze)
{
    maze = newMaze;
}

void VulkanContext::printDebugMap() const
{
    if (maze == nullptr)
        return;

    const uint32_t width = maze->getWidth();
    const uint32_t height = maze->getHeight();

    const float mazeCenterX =
        (width - 1) / 2.0f;

    const float mazeCenterZ =
        (height - 1) / 2.0f;

    const glm::vec3 playerPosition =
        camera.getPosition();

    int playerX =
        static_cast<int>(
            std::round(playerPosition.x + mazeCenterX)
        );

    int playerY =
        static_cast<int>(
            std::round(playerPosition.z + mazeCenterZ)
        );

    // ANSI colors
    const char* blue = "\033[94m";
    const char* yellow = "\033[93m";
    const char* reset = "\033[0m";

    std::cout << "\n";

    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            if (static_cast<int>(x) == playerX &&
                static_cast<int>(y) == playerY)
            {
                std::cout << blue << "P " << reset;
            }
            else if (x == maze->getExitX() &&
                     y == maze->getExitY())
            {
                std::cout << yellow << "E " << reset;
            }
            else if (maze->isWalkable(x, y))
            {
                std::cout << ". ";
            }
            else
            {
                std::cout << "##";
            }
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}