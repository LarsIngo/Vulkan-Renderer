#include "BUILD_OPTIONS.hpp"

#include "RendererTools.hpp"
#include "SuperGeometry.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <sstream>
#include <fstream>
#include <assert.h>
#include <iostream>
#include <vulkan/vulkan.h>

std::vector<char> Manovreraren::Tools::ReadFile( const std::string& file_path )
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary );

    if (!file.is_open()) {
        #ifdef _WIN32
        MessageBox(NULL, file_path.c_str(), "READFILEERROR: Tools::ReadFile, failed to open file.", 0);
        #endif
        assert( 0 && "READFILEERROR: Failed to open file." );
        std::exit( -1 );
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer( fileSize );

    file.seekg(0);
    file.read( buffer.data(), fileSize );

    file.close();

    return buffer;
}


void Manovreraren::Tools::CreateShaderModule( const VkDevice& device, const char* shader_spv_path, VkShaderModule& shader_module ) 
{
    auto shader_code = Manovreraren::Tools::ReadFile( shader_spv_path );
    VkShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = shader_code.size();
    shader_module_create_info.pCode = (uint32_t*)shader_code.data();
    VkErrorCheck( vkCreateShaderModule( device, &shader_module_create_info, nullptr, &shader_module ) );
}


VkPipelineShaderStageCreateInfo Manovreraren::Tools::CreatePipelineShaderStageCreateInfo(
    const VkDevice& device, 
    const VkShaderModule& shader_module,
    const VkShaderStageFlagBits& stage_bit, 
    const char* name )
{
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {};
    pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_shader_stage_create_info.stage = stage_bit;
    pipeline_shader_stage_create_info.pName = name;
    pipeline_shader_stage_create_info.module = shader_module;
    return pipeline_shader_stage_create_info;
}


void Manovreraren::Tools::CreateRenderPass( const VkDevice& device, const VkFormat& format, VkRenderPass& render_pass )
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subPass = {};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkErrorCheck( vkCreateRenderPass( device, &renderPassInfo, nullptr, &render_pass ) );
}


void Manovreraren::Tools::CreateCommandBuffers( const VkDevice& device, const VkCommandPool& command_pool, const uint32_t& command_buffer_count, VkCommandBuffer* command_buffer_data )
{
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandBufferCount = command_buffer_count;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkErrorCheck( vkAllocateCommandBuffers( device, &command_buffer_allocate_info, command_buffer_data) );
}


void Manovreraren::Tools::CreateFramebuffer( const VkDevice& device, const VkExtent2D extent, const VkRenderPass& render_pass, const VkImageView& image_view, VkFramebuffer& framebuffer )
{
    VkFramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = render_pass;
    framebuffer_create_info.attachmentCount = 1;
    VkImageView attachments[] = { image_view };
    framebuffer_create_info.pAttachments = attachments;
    framebuffer_create_info.width = extent.width;
    framebuffer_create_info.height = extent.height;
    framebuffer_create_info.layers = 1;

    VkErrorCheck( vkCreateFramebuffer( device, &framebuffer_create_info, nullptr, &framebuffer ) );
}


void Manovreraren::Tools::CreatePipelineLayout(const VkDevice& device, VkPipelineLayout& pipeline_layout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VkErrorCheck( vkCreatePipelineLayout( device, &pipelineLayoutInfo, nullptr, &pipeline_layout ) );
}


void Manovreraren::Tools::CreateGraphicsPipeline( 
    const VkDevice& device, 
    const VkExtent2D& extent, 
    const std::vector<VkPipelineShaderStageCreateInfo>& shader_stage_list,
    const VkRenderPass& render_pass,
    const VkPipelineLayout& pipeline_layout,
    VkPipeline& graphics_pipeline )
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto binding_description = Geometry::Vertex::GetBindingDescription();
    auto attribute_descriptions = Geometry::Vertex::GetAttributeDescriptions();
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &binding_description;
    vertexInputInfo.vertexAttributeDescriptionCount = attribute_descriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attribute_descriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>( extent.width );
    viewport.height = static_cast<float>( extent.height );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>( shader_stage_list.size() );
    pipelineInfo.pStages = shader_stage_list.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipeline_layout;
    pipelineInfo.renderPass = render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkErrorCheck( vkCreateGraphicsPipelines( device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphics_pipeline ) );
}


uint32_t Manovreraren::Tools::FindGraphicsFamilyIndex( const VkPhysicalDevice& gpu )
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_family_count, nullptr );
    std::vector<VkQueueFamilyProperties> queue_family_properties_list( queue_family_count );
    vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_family_count, queue_family_properties_list.data() );

    for ( uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties_list.size()); ++i )
        if ( queue_family_properties_list[ i ].queueCount > 0 && queue_family_properties_list[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            return i;

    #ifdef _WIN32
    MessageBox( NULL, "VKERROR: Queue family supporting graphics not found.", "Vulkan runtime error.", 0 );
    #endif
    assert( 0 && "VKERROR: Queue family supporting graphics not found." );
    std::exit( -1 );

    return 0;
}


uint32_t Manovreraren::Tools::FindPresentFamilyIndex( const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface )
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_family_count, nullptr );
    std::vector<VkQueueFamilyProperties> queue_family_properties_list( queue_family_count );
    vkGetPhysicalDeviceQueueFamilyProperties( gpu, &queue_family_count, queue_family_properties_list.data() );

    for ( uint32_t i = 0; i < static_cast<uint32_t>( queue_family_properties_list.size() ); ++i ) {
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR( gpu, i, surface, &presentSupport );
        if ( queue_family_properties_list[ i ].queueCount > 0 && presentSupport )
            return i;
    }

    #ifdef _WIN32
    MessageBox( NULL, "VKERROR: Queue family supporting present not found.", "Vulkan runtime error.", 0 );
    #endif
    assert( 0 && "VKERROR: Queue family supporting present not found." );
    std::exit( -1 );

    return 0;
}


uint32_t Manovreraren::Tools::FindMemoryType( const VkPhysicalDevice& gpu, const uint32_t& type_filter, const VkMemoryPropertyFlags& memory_property_flags )
{
    VkPhysicalDeviceMemoryProperties gpu_memory_properties;
    vkGetPhysicalDeviceMemoryProperties( gpu, &gpu_memory_properties );

    for ( uint32_t i = 0; i < gpu_memory_properties.memoryTypeCount; ++i ) {
        if ( (type_filter & (1 << i)) && ( gpu_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags ) == memory_property_flags ) {
            return i;
        }
    }

    #ifdef _WIN32
    MessageBox( NULL, "VKERROR: Memory type not found.", "Vulkan runtime error.", 0 );
    #endif
    assert( 0 && "VKERROR: Memory type not found." );
    std::exit( -1 );

    return 0;
}


VkCommandBuffer Manovreraren::Tools::BeginSingleTimeCommand( const VkDevice& device, const VkCommandPool& command_pool ) {
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkErrorCheck( vkAllocateCommandBuffers( device, &command_buffer_allocate_info, &command_buffer) );

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkErrorCheck( vkBeginCommandBuffer( command_buffer, &command_buffer_begin_info ) );

    return command_buffer;
}

void Manovreraren::Tools::EndSingleTimeCommand( const VkDevice& device, const VkCommandPool& command_pool, const VkQueue& queue, const VkCommandBuffer& command_buffer ) {
    VkErrorCheck( vkEndCommandBuffer( command_buffer ) );

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;

    VkErrorCheck( vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE ) );
    VkErrorCheck( vkQueueWaitIdle( queue ) );

    vkFreeCommandBuffers( device, command_pool, 1, &command_buffer );
}


void Manovreraren::Tools::AssertErrorMsg( const char* error_type, const char* error_msg )
{
    #ifdef _WIN32
    MessageBox( NULL, error_type, error_msg, 0 );
    #endif
    assert( 0 && error_msg );
    std::exit( -1 );
}


#if BUILD_ENABLE_VULKAN_RUNTIME_DEBUG

void Manovreraren::Tools::VkErrorCheck( VkResult result )
{
    std::ostringstream stream;
    if ( result != VK_SUCCESS ) {
        stream << "VKERROR CHECK: " << std::endl;
        switch ( result ) {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            stream << "VK_ERROR_OUT_OF_HOST_MEMORY" << std::endl;
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            stream << "VK_ERROR_OUT_OF_DEVICE_MEMORY" << std::endl;
            break;
        case VK_ERROR_INITIALIZATION_FAILED:
            stream << "VK_ERROR_INITIALIZATION_FAILED" << std::endl;
            break;
        case VK_ERROR_DEVICE_LOST:
            stream << "VK_ERROR_DEVICE_LOST" << std::endl;
            break;
        case VK_ERROR_MEMORY_MAP_FAILED:
            stream << "VK_ERROR_MEMORY_MAP_FAILED" << std::endl;
            break;
        case VK_ERROR_LAYER_NOT_PRESENT:
            stream << "VK_ERROR_LAYER_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            stream << "VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_FEATURE_NOT_PRESENT:
            stream << "VK_ERROR_FEATURE_NOT_PRESENT" << std::endl;
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            stream << "VK_ERROR_INCOMPATIBLE_DRIVER" << std::endl;
            break;
        case VK_ERROR_TOO_MANY_OBJECTS:
            stream << "VK_ERROR_TOO_MANY_OBJECTS" << std::endl;
            break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            stream << "VK_ERROR_FORMAT_NOT_SUPPORTED" << std::endl;
            break;
        case VK_ERROR_SURFACE_LOST_KHR:
            stream << "VK_ERROR_SURFACE_LOST_KHR" << std::endl;
            break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            stream << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR" << std::endl;
            break;
        case VK_SUBOPTIMAL_KHR:
            stream << "VK_SUBOPTIMAL_KHR" << std::endl;
            break;
        case VK_ERROR_OUT_OF_DATE_KHR:
            stream << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
            break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            stream << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR" << std::endl;
            break;
        case VK_ERROR_VALIDATION_FAILED_EXT:
            stream << "VK_ERROR_VALIDATION_FAILED_EXT" << std::endl;
            break;
        case VK_ERROR_INVALID_SHADER_NV:
            stream << "VK_ERROR_INVALID_SHADER_NV" << std::endl;
            break;
        default:
            break;
        }
        #ifdef _WIN32
        MessageBox( NULL, stream.str().c_str(), "Vulkan runtime error.", 0 );
        #endif
        assert( 0 && "Vulkan runtime error." );
        std::exit( -1 );
    }
}

#else

void Manovreraren::Tools::VkErrorCheck( VkResult result ) {};

#endif //BUILD_ENABLE_VULKAN_RUNTIME_DEBUG