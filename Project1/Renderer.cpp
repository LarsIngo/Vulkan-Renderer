#include "Renderer.hpp"

#include "BUILD_OPTIONS.hpp"
#include "RendererTools.hpp"
#include "Image.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <sstream>

using namespace Manovreraren;

Renderer::Renderer( const char* window_title,
    const bool& windowed,
    const bool& auto_full_resolution,
    const bool& borderless,
    const unsigned int& window_width,
    const unsigned int& window_height )
{
    m_SetupDebugCallback();
    m_InitGFLWWindow( window_title, windowed, auto_full_resolution, borderless, window_width, window_height );
    m_SetupLayersAndExtensions();
    m_InitInstance();
    m_InitDebug();
    m_InitDevice();
    m_InitSurface();
    m_InitSwapchain();
    m_InitSwapchainImageViews();
    //m_InitRenderPass();
    //m_InitGraphicsPipeline();
    m_InitCommandPool();
    //m_InitSwapchainFramebuffers();
    //m_InitSwapchainCommmandBuffers();
    m_InitSemaphores();
    //m_InitFences();
}


Renderer::~Renderer()
{
    vkDeviceWaitIdle( m_device );

    //m_DeInitFences();
    m_DeInitSemaphores();
    //m_DeInitSwapchainCommandBuffers();
    //m_DeInitSwapchainFramebuffers();
    m_DeInitCommandPool();
    //m_DeInitGraphicsPipeline();
    //m_DeInitRenderPass();
    m_DeInitSwapchainImageViews();
    m_DeInitSwapchain();
    m_DeInitSurface();
    m_DeInitDevice();
    m_DeInitDebug();
    m_DeInitInstance();
    m_DeInitGLFWWindow();
}


//void Renderer::m_ReInitSwapchain()
//{
//    vkDeviceWaitIdle( m_device );
//
//    //m_DeInitSwapchainCommandBuffers();
//    //m_DeInitSwapchainFramebuffers();
//    //m_DeInitGraphicsPipeline();
//    //m_DeInitRenderPass();
//    m_DeInitSwapchainImageViews();
//    m_DeInitSwapchain();
//    //m_DeInitSurface();
//
//    //glfwSetWindowSize( m_window, m_window_width, m_window_height );
//
//    //m_InitSurface();
//    m_InitSwapchain();
//    m_InitSwapchainImageViews();
//    //m_InitRenderPass();
//    //m_InitGraphicsPipeline();
//    //m_InitSwapchainFramebuffers();
//    //m_InitSwapchainCommmandBuffers();
//}


void Renderer::m_SetupLayersAndExtensions()
{
    uint32_t glfw_extension_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    for (unsigned int i = 0; i < glfw_extension_count; i++)
        m_instance_extension_list.push_back(glfw_extensions[i]);

    m_device_extension_list.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
}


//void Renderer::m_OnWindowResized( GLFWwindow* window, int width, int height ) {
//    if (width == 0 || height == 0) return;
//
//    Renderer* renderer = reinterpret_cast<Renderer*>( glfwGetWindowUserPointer( window ) );
//    renderer->ResizeWindow( width, height );
//}


void Renderer::m_InitGFLWWindow( const char* window_title,
    const bool& windowed,
    const bool& auto_full_resolution,
    const bool& borderless,
    const unsigned int& window_width,
    const unsigned int& window_height )
{
    //m_window_width = window_width;
    //m_window_height = window_height;

    /* Initialize the library */
    if ( !glfwInit() ) {
        assert( 0 && "GLFWERROR: Initialize the library." );
        std::exit( -1 );
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode( monitor );
    int width = window_width;
    int height = window_height;

    glfwWindowHint( GLFW_RED_BITS, mode->redBits );
    glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
    glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
    glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    if ( borderless )
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    if ( windowed )
        monitor = NULL;
    if ( auto_full_resolution ) {
        width = mode->width;
        height = mode->height;
    }

    /* Create window */
    m_window = glfwCreateWindow( width, height, window_title, monitor, NULL );
    
    if ( !m_window ) {
        glfwTerminate();
        assert( 0 && "GLFWERROR: Create window." );
        std::exit( -1 );
    }

    //glfwSetWindowUserPointer( m_window, this );
    //glfwSetWindowSizeCallback( m_window, Renderer::m_OnWindowResized );

    /* Make the window's context current */
    glfwMakeContextCurrent( m_window );
}


void Renderer::m_DeInitGLFWWindow()
{
    if ( m_window != nullptr ) {
        glfwTerminate();
        m_window = nullptr;
    }
}


void Renderer::m_InitInstance()
{
    VkApplicationInfo application_info = {};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.apiVersion = VK_MAKE_VERSION( 1, 0, 3 );
    application_info.pApplicationName = "Vulkan Renderer";
    application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    application_info.pEngineName = "No Engine";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledExtensionCount = static_cast<std::uint32_t>(m_instance_extension_list.size());
    instance_create_info.ppEnabledExtensionNames = m_instance_extension_list.data();
    instance_create_info.enabledLayerCount = static_cast<std::uint32_t>(m_instance_layer_list.size());
    instance_create_info.ppEnabledLayerNames = m_instance_layer_list.data();
    instance_create_info.pNext = &m_debug_callback_create_info;

    Tools::VkErrorCheck( vkCreateInstance( &instance_create_info, nullptr, &m_instance ) );
}


void Renderer::m_DeInitInstance()
{
    vkDestroyInstance( m_instance, nullptr );
    m_instance = VK_NULL_HANDLE;
}


void Renderer::m_InitDevice()
{
    {
        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices( m_instance, &gpu_count, nullptr );
        std::vector<VkPhysicalDevice> gpu_list( gpu_count );
        vkEnumeratePhysicalDevices( m_instance, &gpu_count, gpu_list.data() );
        m_gpu = gpu_list[ 0 ];
        vkGetPhysicalDeviceProperties( m_gpu, &m_physical_device_proterties );
    }

    m_graphics_family_index = Tools::FindGraphicsFamilyIndex( m_gpu );

    float queue_priorities{ 1.f };
    VkDeviceQueueCreateInfo device_queue_info = {};
    device_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_info.queueCount = 1;
    device_queue_info.queueFamilyIndex = m_graphics_family_index;
    device_queue_info.pQueuePriorities = &queue_priorities;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_info;
    device_create_info.enabledLayerCount = static_cast<uint32_t>(m_device_extension_list.size());
    device_create_info.ppEnabledLayerNames = m_device_extension_list.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extension_list.size());
    device_create_info.ppEnabledExtensionNames = m_device_extension_list.data();

    Tools::VkErrorCheck( vkCreateDevice( m_gpu, &device_create_info, nullptr, &m_device ) );

    vkGetDeviceQueue( m_device, m_graphics_family_index, 0, &m_graphics_queue );
}


void Renderer::m_DeInitDevice()
{
    vkDestroyDevice( m_device, nullptr );
    m_device = VK_NULL_HANDLE;
}


void Renderer::m_InitSurface()
{
    Tools::VkErrorCheck( glfwCreateWindowSurface( m_instance, m_window, nullptr, &m_surface ) );

    m_present_family_index = Tools::FindPresentFamilyIndex( m_gpu, m_surface );

    //vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_gpu, m_surface, &m_surface_capabilities );
    //if ( m_surface_capabilities.currentExtent.width < UINT32_MAX ) {
    //    m_swapchain_extent.width = m_surface_capabilities.currentExtent.width;
    //    m_swapchain_extent.height = m_surface_capabilities.currentExtent.height;
    //}

    {
        uint32_t surface_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR( m_gpu, m_surface, &surface_count, nullptr );
        if ( surface_count == 0 ) {
            assert( 0 && "Surface format missing." );
            std::exit( -1 );
        }
        std::vector<VkSurfaceFormatKHR> surface_formats( surface_count );
        vkGetPhysicalDeviceSurfaceFormatsKHR( m_gpu, m_surface, &surface_count, surface_formats.data() );
        if ( surface_formats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
            m_surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
            m_surface_format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        } else {
            m_surface_format = surface_formats[ 0 ];
        }
    }
}


void Renderer::m_DeInitSurface()
{
    vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
    m_surface = VK_NULL_HANDLE;
}


void Renderer::m_InitSwapchain() 
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_gpu, m_surface, &m_surface_capabilities );
    if (m_surface_capabilities.currentExtent.width < UINT32_MAX) {
        m_swapchain_extent.width = m_surface_capabilities.currentExtent.width;
        m_swapchain_extent.height = m_surface_capabilities.currentExtent.height;
    }

    if ( m_swapchain_image_count < m_surface_capabilities.minImageCount + 1 )
        m_swapchain_image_count = m_surface_capabilities.minImageCount + 1;
    if ( m_surface_capabilities.maxImageCount > 0 )
        if ( m_swapchain_image_count > m_surface_capabilities.maxImageCount ) 
            m_swapchain_image_count = m_surface_capabilities.maxImageCount;

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // VK_PRESENT_MODE_MAILBOX_KHR is guaranteed to work, VK_PRESENT_MODE_MAILBOX_KHR wanted for games (v-sync)
    {
        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR( m_gpu, m_surface, &present_mode_count, nullptr );
        std::vector<VkPresentModeKHR> present_mode_list( present_mode_count );
        vkGetPhysicalDeviceSurfacePresentModesKHR( m_gpu, m_surface, &present_mode_count, present_mode_list.data() );
        for ( auto m : present_mode_list )
            if ( m == VK_PRESENT_MODE_MAILBOX_KHR )
                present_mode = m;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info;
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = m_surface;
    swapchain_create_info.minImageCount = m_swapchain_image_count;
    swapchain_create_info.imageFormat = m_surface_format.format;
    swapchain_create_info.imageColorSpace = m_surface_format.colorSpace;
    swapchain_create_info.imageExtent.width = m_swapchain_extent.width;
    swapchain_create_info.imageExtent.height = m_swapchain_extent.height;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT when only copy and not render to swapchain image
    
    uint32_t queue_family_indices[] = { m_graphics_family_index, m_present_family_index };
    if ( m_present_family_index != m_graphics_family_index ) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // (SLI), several queue families render to same swap chain OR have one queue render and another present 
        swapchain_create_info.queueFamilyIndexCount = 0; // Not used if VK_SHARING_MODE_EXCLUSIVE
        swapchain_create_info.pQueueFamilyIndices = nullptr; // Not used if VK_SHARING_MODE_EXCLUSIVE
    }
    
    swapchain_create_info.preTransform = m_surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;

    VkSwapchainKHR old_swapchain = m_swapchain;
    swapchain_create_info.oldSwapchain = old_swapchain;

    VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
    Tools::VkErrorCheck( vkCreateSwapchainKHR( m_device, &swapchain_create_info, nullptr, &new_swapchain ) );

    *&m_swapchain = new_swapchain;

    Tools::VkErrorCheck( vkGetSwapchainImagesKHR( m_device, m_swapchain, &m_swapchain_image_count, nullptr ) );

    vkGetDeviceQueue( m_device, m_present_family_index, 0, &m_present_queue );
}


void Renderer::m_DeInitSwapchain()
{
    vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );
    m_swapchain = VK_NULL_HANDLE;
}


void Renderer::m_InitSwapchainImageViews()
{
    m_swapchain_image_list.resize( m_swapchain_image_count );
    m_swapchain_image_view_list.resize( m_swapchain_image_count );

    Tools::VkErrorCheck( vkGetSwapchainImagesKHR( m_device, m_swapchain, &m_swapchain_image_count, m_swapchain_image_list.data() ) );

    for (uint32_t i = 0; i < m_swapchain_image_count; ++i ) {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = m_swapchain_image_list[ i ];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = m_surface_format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        Tools::VkErrorCheck( vkCreateImageView( m_device, &image_view_create_info, nullptr, &m_swapchain_image_view_list[ i ] ) );
    }
}


void Renderer::m_DeInitSwapchainImageViews()
{
    for (auto& image_view : m_swapchain_image_view_list) {
        vkDestroyImageView(m_device, image_view, nullptr);
        image_view = VK_NULL_HANDLE;
    }
}


//void Renderer::m_InitRenderPass()
//{
//    Tools::CreateRenderPass( m_device, m_surface_format.format, m_render_pass );
//}


//void Renderer::m_DeInitRenderPass()
//{
//    vkDestroyRenderPass( m_device, m_render_pass, nullptr );
//    m_render_pass = VK_NULL_HANDLE;
//}


//void Renderer::m_InitGraphicsPipeline()
//{
//    Tools::CreateShaderModule( m_device, "shaders/vert.spv", m_vert_shader_module);
//    Tools::CreateShaderModule( m_device, "shaders/frag.spv", m_frag_shader_module);
//
//    Tools::CreatePipelineLayout( m_device, m_pipeline_layout );
//
//    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_list;
//    shader_stage_list.push_back( Tools::CreatePipelineShaderStageCreateInfo(m_device, m_vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT, "main" ) );
//    shader_stage_list.push_back( Tools::CreatePipelineShaderStageCreateInfo(m_device, m_frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT, "main" ) );
//    Tools::CreateGraphicsPipeline( m_device, m_swapchain_extent, shader_stage_list, m_render_pass, m_pipeline_layout, m_graphics_pipeline );
//}


//void Renderer::m_DeInitGraphicsPipeline()
//{
//    vkDestroyPipelineLayout( m_device, m_pipeline_layout, nullptr );
//    vkDestroyPipeline( m_device, m_graphics_pipeline, nullptr );
//    vkDestroyShaderModule( m_device, m_vert_shader_module, nullptr );
//    vkDestroyShaderModule( m_device, m_frag_shader_module, nullptr );
//}


void Renderer::m_InitCommandPool()
{
    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.queueFamilyIndex = m_graphics_family_index;

    Tools::VkErrorCheck( vkCreateCommandPool( m_device, &command_pool_create_info, nullptr, &m_command_pool ) );
}

void Renderer::m_DeInitCommandPool()
{
    vkDestroyCommandPool( m_device, m_command_pool, nullptr );
    m_command_pool = VK_NULL_HANDLE;
}


//void Renderer::m_InitSwapchainFramebuffers() 
//{
//    m_swapchain_framebuffer_list.resize( m_swapchain_image_count );
//
//    for ( uint32_t i = 0; i < m_swapchain_image_count; ++i )
//        Tools::CreateFramebuffer( m_device, m_swapchain_extent, m_render_pass, m_swapchain_image_view_list[ i ], m_swapchain_framebuffer_list[ i ] );
//}


//void Renderer::m_DeInitSwapchainFramebuffers()
//{
//    for ( auto& swapchain_frame_buffer : m_swapchain_framebuffer_list ) {
//        vkDestroyFramebuffer( m_device, swapchain_frame_buffer, nullptr );
//        swapchain_frame_buffer = VK_NULL_HANDLE;
//    }
//}


//void Renderer::m_InitSwapchainCommmandBuffers()
//{
//    m_swapchain_command_buffer_list.resize( m_swapchain_image_count );
//
//    Tools::CreateCommandBuffers( m_device, m_command_pool, m_swapchain_image_count, m_swapchain_command_buffer_list.data() );
//
//    for ( size_t i = 0; i < m_swapchain_command_buffer_list.size(); ++i ) {
//        VkCommandBufferBeginInfo command_buffer_begin_info = {};
//        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//
//        Tools::VkErrorCheck( vkBeginCommandBuffer( m_swapchain_command_buffer_list[ i ], &command_buffer_begin_info ) );
//
//        VkRenderPassBeginInfo render_pass_begin_info = {};
//        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        render_pass_begin_info.renderPass = m_render_pass;
//        render_pass_begin_info.framebuffer = m_swapchain_framebuffer_list[ i ];
//        render_pass_begin_info.renderArea.offset = { 0, 0 };
//        render_pass_begin_info.renderArea.extent = m_swapchain_extent;
//
//        VkClearValue clear_color = { 0.f, 0.f, 0.f, 1.f };
//        render_pass_begin_info.clearValueCount = 1;
//        render_pass_begin_info.pClearValues = &clear_color;
//
//        vkCmdBeginRenderPass( m_swapchain_command_buffer_list[ i ], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
//
//        vkCmdBindPipeline( m_swapchain_command_buffer_list[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline );
//
//        vkCmdDraw( m_swapchain_command_buffer_list[ i ], 3, 1, 0, 0 );
//
//        vkCmdEndRenderPass( m_swapchain_command_buffer_list[ i ] );
//
//        Tools::VkErrorCheck( vkEndCommandBuffer( m_swapchain_command_buffer_list[ i ] ) );
//    }
//}


//void Renderer::m_DeInitSwapchainCommandBuffers()
//{
//    vkFreeCommandBuffers( m_device, m_command_pool, static_cast<uint32_t>( m_swapchain_command_buffer_list.size() ), m_swapchain_command_buffer_list.data() );
//}


void Renderer::m_InitSemaphores()
{
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    Tools::VkErrorCheck( vkCreateSemaphore( m_device, &semaphore_create_info, nullptr, &m_render_complete_semaphore ) );
    Tools::VkErrorCheck( vkCreateSemaphore( m_device, &semaphore_create_info, nullptr, &m_present_complete_semaphore ) );
}


void Renderer::m_DeInitSemaphores()
{
    vkDestroySemaphore( m_device, m_render_complete_semaphore, nullptr );
    m_render_complete_semaphore = VK_NULL_HANDLE;
    vkDestroySemaphore( m_device, m_present_complete_semaphore, nullptr );
    m_present_complete_semaphore = VK_NULL_HANDLE;
}


//void Renderer::m_InitFences()
//{
//    VkFenceCreateInfo fence_create_info = {};
//    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//
//    Tools::VkErrorCheck( vkCreateFence( m_device, &fence_create_info, nullptr, &m_fence_wait ) );
//}
//
//
//void Renderer::m_DeInitFences()
//{
//    vkDestroyFence( m_device, m_fence_wait, nullptr );
//    m_fence_wait = VK_NULL_HANDLE;
//}



#if BUILD_ENABLE_VULKAN_DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanDebugCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  obj_type,
    uint64_t                    src_obj,
    size_t                      location,
    int32_t                     msg_code,
    const char*                 layer_prefix,
    const char*                 msg,
    void*                       user_data
    )
{
    std::ostringstream stream;
    stream << "VKDBG: ";
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        stream << "INFO" ;
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        stream << "WARNING";
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        stream << "PERFORMANCE_WARNING";
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        stream << "ERROR";
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        stream << "DEBUG";
    
    stream << "@[" << layer_prefix << "]: ";
    stream << "Code: " << msg_code << " : " <<msg << std::endl;

    std::cout << stream.str();
    
    #ifdef _WIN32
    //if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    MessageBox( NULL, stream.str().c_str(), "Vulkan Error!", 0 );
    #endif
    return VK_FALSE;
}

void Renderer::m_SetupDebugCallback() 
{
    m_debug_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    m_debug_callback_create_info.pfnCallback = (PFN_vkDebugReportCallbackEXT)VulkanDebugCallback;
    m_debug_callback_create_info.flags =
        //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        //VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        0;

    //{
    //    uint32_t layer_property_count = 0;
    //    Tools::VkErrorCheck( vkEnumerateInstanceLayerProperties( &layer_property_count, nullptr ) );
    //    std::vector<VkLayerProperties> layer_property_list( layer_property_count );
    //    Tools::VkErrorCheck( vkEnumerateInstanceLayerProperties( &layer_property_count, layer_property_list.data() ) );
    //    std::cout << "Instance Layers: " << std::endl;
    //    for (auto &i : layer_property_list)
    //        std::cout << "   " << i.layerName << "\t\t | " << i.description << std::endl;
    //    std::cout << std::endl;
    //}
    //{
    //    uint32_t layer_property_count = 0;
    //    Tools::VkErrorCheck( vkEnumerateDeviceLayerProperties( m_gpu, &layer_property_count, nullptr ) ) ;
    //    std::vector<VkLayerProperties> layer_property_list( layer_property_count );
    //    Tools::VkErrorCheck( vkEnumerateDeviceLayerProperties( m_gpu, &layer_property_count, layer_property_list.data() ) );
    //    std::cout << "Device Layers: " << std::endl;
    //    for ( auto &i : layer_property_list )
    //        std::cout << "   " << i.layerName << "\t\t | " << i.description << std::endl;
    //    std::cout << std::endl;
    //}

    m_instance_layer_list.push_back ( "VK_LAYER_LUNARG_standard_validation" );
    m_instance_extension_list.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
    m_device_layer_list.push_back( "VK_LAYER_LUNARG_standard_validation" );
}


PFN_vkCreateDebugReportCallbackEXT fvkCreateDebugReportCallbackEXT = VK_NULL_HANDLE;
PFN_vkDestroyDebugReportCallbackEXT fvkDestroyDebugReportCallbackEXT = VK_NULL_HANDLE;
PFN_vkDebugReportMessageEXT fvkDebugReportMessageEXT = VK_NULL_HANDLE;

void Renderer::m_InitDebug() 
{
    fvkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instance, "vkCreateDebugReportCallbackEXT" ) );
    fvkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>( vkGetInstanceProcAddr( m_instance, "vkDestroyDebugReportCallbackEXT" ) );
    fvkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>( vkGetInstanceProcAddr( m_instance, "vkDebugReportMessageEXT" ) );

    if (fvkCreateDebugReportCallbackEXT == VK_NULL_HANDLE || fvkDestroyDebugReportCallbackEXT == VK_NULL_HANDLE) {
        assert( 0 && "VKERROR: Could not fetch debug function pointers." );
        std::exit( -1 );
    }

    Tools::VkErrorCheck( fvkCreateDebugReportCallbackEXT( m_instance, &m_debug_callback_create_info, nullptr, &m_debug_callback ) );
}

void Renderer::m_DeInitDebug() 
{   
    fvkDestroyDebugReportCallbackEXT( m_instance, m_debug_callback, nullptr );
    m_debug_callback = VK_NULL_HANDLE;
}

#else

void Renderer::m_SetupDebugCallback() {};
void Renderer::m_InitDebug() {};
void Renderer::m_DeInitDebug() {};

#endif //BUILD_ENABLE_VULKAN_DEBUG

bool Renderer::Run()
{
    /* Poll for and process events */
    glfwPollEvents();

    if ( glfwWindowShouldClose(m_window) )
        m_should_close_window = true;

    return !m_should_close_window;
}


void Renderer::CloseWindow()
{
    m_should_close_window = true;
}


//void Renderer::Bind( ShaderProgram& shader_program)
//{
//    m_p_shader_program = &shader_program;
//}


void Renderer::m_StartNewFrame()
{
    // GET NEXT IMAGE FROM SWAPCHAIN
    Tools::VkErrorCheck( vkAcquireNextImageKHR( m_device, m_swapchain, (std::numeric_limits<uint64_t>::max)(), m_present_complete_semaphore, VK_NULL_HANDLE, &m_render_swapchain_image_index ) );

    //// BEGIN NEW RENDER PASS
    //{
    //    VkCommandBuffer command_buffer = Tools::BeginSingleTimeCommand(m_device, m_command_pool);
    //    VkRenderPassBeginInfo render_pass_begin_info = {};
    //    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //    render_pass_begin_info.renderPass = m_p_shader_program->GetRenderPass();
    //    render_pass_begin_info.framebuffer = m_p_shader_program->GetFramebufferList()[ m_render_swapchain_image_index ];
    //    render_pass_begin_info.renderArea.offset = { 0, 0 };
    //    render_pass_begin_info.renderArea.extent = m_p_shader_program->GetExtent();

    //    VkClearValue clear_color = { 0.f, 0.f, 0.f, 1.f };
    //    render_pass_begin_info.clearValueCount = 1;
    //    render_pass_begin_info.pClearValues = &clear_color;

    //    vkCmdBeginRenderPass( command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    //    Tools::EndSingleTimeCommand(m_device, m_command_pool, m_graphics_queue, command_buffer);
    //}
}


void Renderer::Render( ShaderProgram& shader_program )
{
    /*VkSubmitInfo submit_info = {};
    {
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &model.GetCommandBufferList()[ m_render_swapchain_image_index ];
    }
    Tools::VkErrorCheck( vkQueueSubmit( m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE ) );*/

    m_StartNewFrame();

    VkCommandBuffer command_buffer = Tools::BeginSingleTimeCommand( m_device, m_command_pool );

    // BEGIN NEW RENDER PASS
    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = shader_program.GetRenderPass();
    render_pass_begin_info.framebuffer = shader_program.GetFramebufferList()[ m_render_swapchain_image_index ];
    render_pass_begin_info.renderArea.offset = { 0, 0 };
    render_pass_begin_info.renderArea.extent = shader_program.GetExtent();

    VkClearValue clear_color = { 0.f, 0.f, 0.f, 1.f };
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass( command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );


    vkCmdBindPipeline( command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader_program.GetGraphicsPipeline() );

    // DRAW
    for ( auto& model : *shader_program.GetModelList() ) {
         auto p_mesh = model->GetMesh();

         VkBuffer vertex_Buffer_list[] = { p_mesh->GetVertexBuffer() };
         VkDeviceSize offsets[] = { 0 };
         vkCmdBindVertexBuffers( command_buffer, 0, 1, vertex_Buffer_list, offsets );

         vkCmdDraw( command_buffer, p_mesh->GetNumberOfVertices(), 1, 0, 0 );
    }

    // END RENDER PASS
    vkCmdEndRenderPass( command_buffer );
    Tools::EndSingleTimeCommand( m_device, m_command_pool, m_graphics_queue, command_buffer );

    m_EndFrame();
}


void Renderer::m_EndFrame()
{
    //// END RENDER PASS
    //VkCommandBuffer command_buffer = Tools::BeginSingleTimeCommand( m_device, m_command_pool );
    //vkCmdEndRenderPass( command_buffer );
    //Tools::EndSingleTimeCommand( m_device, m_command_pool, m_graphics_queue, command_buffer );

    // SIGNAL THAT RENDER IS DONE
    VkSubmitInfo submit_info = {};
    {
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore signal_semaphores[] = { m_render_complete_semaphore };
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphores;
    }
    Tools::VkErrorCheck( vkQueueSubmit( m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE ) );
}


void Renderer::Present()
{
    VkPresentInfoKHR present_info = {};
    {
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        VkSemaphore wait_semaphores[] = { m_present_complete_semaphore, m_render_complete_semaphore };
        present_info.waitSemaphoreCount = 2;
        present_info.pWaitSemaphores = wait_semaphores;
        VkSwapchainKHR swapchains[] = { m_swapchain };
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &m_render_swapchain_image_index;
    }
    vkQueuePresentKHR( m_present_queue, &present_info );
}


void Renderer::PresentImage(Image& image)
{
    //VkCommandBuffer command_buffer = Tools::BeginSingleTimeCommand( m_device, m_command_pool );

    //VkImageCopy regions = {};
    //vkCmdCopyImage( command_buffer, image.GetImage(), image.GetImageInfo().initialLayout, m_swapchain_image_list[ m_render_swapchain_image_index ], , , &regions );

    //Tools::EndSingleTimeCommand( m_device, m_command_pool, m_graphics_queue, command_buffer );
}


void Renderer::SetWindowTitle( const char* title )
{
    glfwSetWindowTitle( m_window, title );
}


//void Renderer::ResizeWindow( const unsigned int& window_width, const unsigned int& window_height )
//{
//    //m_window_width = window_width;
//    //m_window_height = window_height;
//    m_ReInitSwapchain();
//}


const unsigned int& Renderer::GetWindowWidth()
{
    return m_swapchain_extent.width;
}


const unsigned int& Renderer::GetWindowHeight()
{
    return m_swapchain_extent.height;
}


const VkInstance& Renderer::GetVkInstance()
{
    return m_instance;
}


const VkDevice& Renderer::GetVkDevice()
{
    return m_device;
}


const VkPhysicalDevice& Renderer::GetVkPhysicalDevice()
{
    return m_gpu;
}


const VkQueue& Renderer::GetVkGraphicsQueue()
{
    return m_graphics_queue;
}


const uint32_t& Renderer::GetGraphicsFamilyIndex()
{
    return m_graphics_family_index;
}


const VkExtent2D& Renderer::GetExtent()
{
    return m_swapchain_extent;
}


const VkFormat& Renderer::GetFormat()
{
    return m_surface_format.format;
}


const VkCommandPool& Renderer::GetCommandPool()
{
    return m_command_pool;
}


const std::vector<VkImageView>& Renderer::GetImageViewList()
{
    return m_swapchain_image_view_list;
}
