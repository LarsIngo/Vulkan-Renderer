#pragma once

#include "SuperGeometry.hpp"

namespace Manovreraren 
{
    namespace Geometry 
    {
        class OBJMesh : public SuperGeometry {
        public:
            OBJMesh( const VkDevice& device, const VkPhysicalDevice& physical_device, const char* file_path );
            ~OBJMesh();
        };
    }
}