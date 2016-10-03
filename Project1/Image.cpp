#include "Image.hpp"
#include "RendererTools.hpp"

using namespace Manovreraren;

Image::Image( const VkDevice& device, const VkPhysicalDevice physical_device, const VkExtent2D& extent, const VkFormat& format, const uint32_t& graphics_family_index )
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
    m_extent = extent;
    m_format = format;
    m_graphics_family_index = graphics_family_index;

    m_InitImage();
    m_InitImageView();
    //m_InitFramebuffer();
}


Image::~Image()
{
    vkDeviceWaitIdle( *m_p_device );

    //m_DeInitFramebuffer();
    m_DeInitImageView();
    m_DeInitImage();
}


void Image::m_InitImage()
{
    m_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    m_image_info.format = m_format;
    m_image_info.queueFamilyIndexCount = 1;
    m_image_info.pQueueFamilyIndices = &m_graphics_family_index;
    m_image_info.mipLevels = 1;
    m_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    m_image_info.arrayLayers = 1;
    m_image_info.imageType = VK_IMAGE_TYPE_2D;
    m_image_info.extent.width = m_extent.width;
    m_image_info.extent.height = m_extent.height;
    m_image_info.extent.depth = 1;
    m_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    m_image_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Tools::VkErrorCheck ( vkCreateImage( *m_p_device, &m_image_info, nullptr, &m_image ) );
}


void Image::m_DeInitImage()
{
    vkDestroyImage( *m_p_device, m_image, nullptr );
    m_image = VK_NULL_HANDLE;
}


void Image::m_InitImageView()
{
    VkImageViewCreateInfo image_view_create_info = {};
    image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_create_info.format = m_format;
    image_view_create_info.image = m_image;
    image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_create_info.subresourceRange.baseMipLevel = 0;
    image_view_create_info.subresourceRange.levelCount = 1;
    image_view_create_info.subresourceRange.baseArrayLayer = 0;
    image_view_create_info.subresourceRange.layerCount = 1;

    {
        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements( *m_p_device, m_image, &memory_requirements );

        VkMemoryAllocateInfo memory_allocate_info = {};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = Tools::FindMemoryType( *m_p_physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        // TODO vkAllocateMemory large chunk of memory, then vkBindxMemory() on same memory chunk 
        Tools::VkErrorCheck( vkAllocateMemory( *m_p_device, &memory_allocate_info, nullptr, &m_image_device_memory ) );
    }

    Tools::VkErrorCheck( vkBindImageMemory( *m_p_device, m_image, m_image_device_memory, 0 ) );
    Tools::VkErrorCheck( vkCreateImageView( *m_p_device, &image_view_create_info, nullptr, &m_image_view ) );
}


void Image::m_DeInitImageView()
{
    vkDestroyImageView( *m_p_device, m_image_view, nullptr );
    m_image_view = VK_NULL_HANDLE;
    vkFreeMemory( *m_p_device, m_image_device_memory, nullptr );
    m_image_device_memory = VK_NULL_HANDLE;
}


//void Image::m_InitFramebuffer()
//{
//    VkFramebufferCreateInfo framebuffer_create_info = {};
//    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//    Tools::VkErrorCheck( vkCreateFramebuffer( *m_pDevice, &framebuffer_create_info, nullptr, &m_framebuffer ) );
//}
//
//
//void Image::m_DeInitFramebuffer()
//{
//    vkDestroyFramebuffer( *m_pDevice, m_framebuffer, nullptr );
//    m_framebuffer = VK_NULL_HANDLE;
//}


const VkImageCreateInfo& Image::GetImageInfo()
{
    return m_image_info;
}


const VkImageView& Image::GetImageView()
{
    return m_image_view;
}


const VkImage& Image::GetImage()
{
    return m_image;
}
