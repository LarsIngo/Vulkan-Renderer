#pragma once

/* GLFW_INCLUDE_VULKAN makes the GLFW header include the Vulkan vulkan/vulkan.h header instead of the regular OpenGL header. */
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace Manovreraren 
{
    class ShaderProgram;
    class Model;
    class Image;

    class Renderer
    {
    public:
        Renderer( const char* window_title = "Manovreraren", 
            const bool& windowed = true, 
            const bool& auto_full_resolution = true, 
            const bool& borderless = true,
            const unsigned int& window_width = 640, 
            const unsigned int& window_height = 480 );
        ~Renderer();

        bool Run();
        void CloseWindow();

        //void Bind( ShaderProgram& shader_program );
        void Render( ShaderProgram& shader_program );
        void Present();
        void PresentImage( Image& image );

        void SetWindowTitle( const char* window_title );
        //void ResizeWindow( const unsigned int& window_width, const unsigned int& window_height );
        const unsigned int& GetWindowWidth();
        const unsigned int& GetWindowHeight();

        const VkInstance& GetVkInstance();
        const VkDevice& GetVkDevice();
        const VkPhysicalDevice& GetVkPhysicalDevice();
        const VkQueue& GetVkGraphicsQueue();
        const uint32_t& GetGraphicsFamilyIndex();
        const VkExtent2D& GetExtent();
        const VkFormat& GetFormat();
        const VkCommandPool& GetCommandPool();
        const std::vector<VkImageView>& GetImageViewList();

    private:
        void m_InitGFLWWindow( const char* window_title,
            const bool& windowed,
            const bool& auto_full_resolution,
            const bool& borderless,
            const unsigned int& window_width, 
            const unsigned int& window_height );
        void m_DeInitGLFWWindow();
        //static void m_OnWindowResized( GLFWwindow* window, int width, int height );

        void m_SetupLayersAndExtensions();

        void m_InitInstance();
        void m_DeInitInstance();

        void m_SetupDebugCallback();
        void m_InitDebug();
        void m_DeInitDebug();

        void m_InitDevice();
        void m_DeInitDevice();

        void m_InitSurface();
        void m_DeInitSurface();

        void m_InitSwapchain();
        void m_DeInitSwapchain();
        //void m_ReInitSwapchain();

        void m_InitSwapchainImageViews();
        void m_DeInitSwapchainImageViews();

        //void m_InitRenderPass();
        //void m_DeInitRenderPass();

        //void m_InitGraphicsPipeline();
        //void m_DeInitGraphicsPipeline();

        void m_InitCommandPool();
        void m_DeInitCommandPool();

        //void m_InitSwapchainFramebuffers();
        //void m_DeInitSwapchainFramebuffers();

        //void m_InitSwapchainCommmandBuffers();
        //void m_DeInitSwapchainCommandBuffers();

        void m_InitSemaphores();
        void m_DeInitSemaphores();

        //void m_InitFences();
        //void m_DeInitFences();

        void m_StartNewFrame();
        void m_EndFrame();

        GLFWwindow* m_window = nullptr;
        bool m_should_close_window = false;
        //unsigned int m_window_width;
        //unsigned int m_window_height;

        VkInstance m_instance = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;        
        VkPhysicalDevice m_gpu = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties m_physical_device_proterties = {};
        uint32_t m_graphics_family_index = 0;
        uint32_t m_present_family_index = 0;

        std::vector<const char*> m_instance_layer_list;
        std::vector<const char*> m_instance_extension_list;
        std::vector<const char*> m_device_layer_list;
        std::vector<const char*> m_device_extension_list;
        VkDebugReportCallbackEXT m_debug_callback;
        VkDebugReportCallbackCreateInfoEXT m_debug_callback_create_info = {};

        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSurfaceCapabilitiesKHR m_surface_capabilities = {};
        VkSurfaceFormatKHR m_surface_format = {};
        VkExtent2D m_swapchain_extent = {};
        uint32_t m_swapchain_image_count = 0;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> m_swapchain_image_list;
        std::vector<VkImageView> m_swapchain_image_view_list;
        //std::vector<VkFramebuffer> m_swapchain_framebuffer_list;
        //std::vector<VkCommandBuffer> m_swapchain_command_buffer_list;

        //VkRenderPass m_render_pass = VK_NULL_HANDLE;

        VkCommandPool m_command_pool = VK_NULL_HANDLE;

        VkQueue m_graphics_queue = VK_NULL_HANDLE;
        VkQueue m_present_queue = VK_NULL_HANDLE;

        //VkShaderModule m_vert_shader_module = VK_NULL_HANDLE;
        //VkShaderModule m_frag_shader_module = VK_NULL_HANDLE;
        //VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
        //VkPipeline m_graphics_pipeline = VK_NULL_HANDLE;

        // Semaphores
        // Used to coordinate operations within the graphics queue and ensure correct command ordering
        VkSemaphore m_present_complete_semaphore = VK_NULL_HANDLE;
        VkSemaphore m_render_complete_semaphore = VK_NULL_HANDLE;

        uint32_t m_render_swapchain_image_index = 0;

        //ShaderProgram* m_p_shader_program = nullptr;

        //VkFence m_fence_wait = VK_NULL_HANDLE;

    };
}
