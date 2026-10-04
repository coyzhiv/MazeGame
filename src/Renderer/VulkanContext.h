#pragma once

#include "Vertex.h"
#include "../Camera/Camera.h"
#include "../Camera/CameraUBO.h"
#include "../Maze/Maze.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include <string>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() &&
               presentFamily.has_value();
    }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;

    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct PushConstants
{
    glm::mat4 transform;
};

class VulkanContext
{
public:
    void printDebugMap() const;
    void initialize(GLFWwindow* window);
    void cleanup();
    void drawFrame();
    void update(float deltaTime);
    void setMaze(const Maze* maze);

    void createVertexBuffer();
    void setVertices(
        const std::vector<Vertex>& newVertices
    );

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkImage floorTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory floorTextureImageMemory = VK_NULL_HANDLE;
    VkImageView floorTextureImageView = VK_NULL_HANDLE;
    VkSampler floorTextureSampler = VK_NULL_HANDLE;

    VkImage wallTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory wallTextureImageMemory = VK_NULL_HANDLE;
    VkImageView wallTextureImageView = VK_NULL_HANDLE;
    VkSampler wallTextureSampler = VK_NULL_HANDLE;

    VkImage ceilingTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory ceilingTextureImageMemory = VK_NULL_HANDLE;
    VkImageView ceilingTextureImageView = VK_NULL_HANDLE;
    VkSampler ceilingTextureSampler = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkBuffer cameraUniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory cameraUniformBufferMemory = VK_NULL_HANDLE;
    std::vector<Vertex> vertices;

private:
    
    void pickPhysicalDevice();
    QueueFamilyIndices findQueueFamilies();
    void createLogicalDevice();
    void createSwapchain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffers();
    void createSyncObjects();
    void createGraphicsPipeline();
    void createDepthResources();
    void createTextureImage(
        const std::string& filename,
        VkImage& image,
        VkDeviceMemory& imageMemory
    );
    void createTextureImageView(
        VkImage image,
        VkImageView& imageView
    );

    void createTextureSampler(
        VkSampler& sampler
    );
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSet();
    void createCameraUniformBuffer();
    void updateCameraUniformBuffer();
    const Maze* maze = nullptr;
    
    static void mouseCallback(
        GLFWwindow* window,
        double xpos,
        double ypos
    );

    void processMouseMovement(
        double xpos,
        double ypos
    );

    bool firstMouse = true;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    void createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory
    );

    void transitionImageLayout(
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout
    );

    void copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height
    );

    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory
    );

    uint32_t findMemoryType(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties
    );

    std::vector<char> readFile(const std::string& filename);

    VkShaderModule createShaderModule(
        const std::vector<char>& code
    );

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& formats
    );

    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& presentModes
    );

    VkExtent2D chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR& capabilities
    );

    

    SwapchainSupportDetails querySwapchainSupport();

    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        
    VkCommandPool commandPool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> commandBuffers;

    VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;

    VkFence inFlightFence = VK_NULL_HANDLE;

    GLFWwindow* window = nullptr;
    
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

    VkFormat findDepthFormat();

    Camera camera;

    bool victory = false;

    bool debugMapVisible = false;
    float victoryTime = 0.0f;
};