#include "OBJMesh.hpp"

#include <fstream>
#include <vector>
#include <glm/glm.hpp>

using namespace Manovreraren;
using namespace Geometry;

OBJMesh::OBJMesh( const VkDevice& device, const VkPhysicalDevice& physical_device, const char* file_path ) : SuperGeometry( device, physical_device ) {
    
    std::vector<Vertex>* vertex_list = nullptr;

    if ( file_path == "1" ) {
        vertex_list = new std::vector<Vertex>{
            { { 0.5f, 0.0f}, { 1.0f, 0.0f, 0.0f }},
            { { 1.0f, 1.0f },{ 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f } }
        };
    } else {
        vertex_list = new std::vector<Vertex>{
            { { 0.0f, -0.5f },{ 1.0f, 1.0f, 1.0f } },
            { { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
            { { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
        };
    }

   
    m_InitVertexBuffer( *vertex_list );
    
    vertex_list->clear();
    vertex_list->shrink_to_fit();
    delete vertex_list;

    //std::ifstream file( file_path );

    //if ( file ) {
    //    std::vector<glm::vec3> posVec;
    //    std::vector<Vector3> normVec;
    //    std::vector<Vector2> uvVec;
    //    struct VertP {
    //        unsigned int posIndex, uvIndex, normIndex;
    //    };
    //    std::vector<VertP> vertPVec;

    //    while (!file.eof()) {
    //        char c1, c2;
    //        Vector3 vec3;
    //        Vector2 vec2;
    //        file >> c1;
    //        switch (c1) {
    //        case 'v':
    //            file >> std::noskipws >> c2 >> std::skipws;
    //            switch (c2) {
    //            case ' ':
    //                file >> vec3.x >> vec3.y >> vec3.z;
    //                posVec.push_back(vec3);
    //                break;
    //            case 'n':
    //                file >> vec3.x >> vec3.y >> vec3.z;
    //                normVec.push_back(vec3);
    //                break;
    //            case 't':
    //                file >> vec2.x >> vec2.y;
    //                uvVec.push_back(vec2);
    //                break;
    //            }
    //            break;
    //        case 'f':
    //            for (unsigned int i = 0; i < 3; i++) {
    //                VertP vertP;
    //                file >> vertP.posIndex >> c1 >> vertP.uvIndex >> c2 >> vertP.normIndex;
    //                vertP.posIndex -= 1;
    //                vertP.uvIndex -= 1;
    //                vertP.normIndex -= 1;
    //                vertPVec.push_back(vertP);
    //            }
    //            break;
    //        default:
    //            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //        }
    //    }

    //    file.close();

    //    nrOfVertices = static_cast<unsigned int>(vertPVec.size());
    //    Vertex* vertices = new Vertex[nrOfVertices];

    //    for (unsigned int i = 0; i < nrOfVertices; ++i) {
    //        Vertex vertex;
    //        vertex.position = posVec[vertPVec[i].posIndex];
    //        vertex.normal = normVec[vertPVec[i].normIndex];
    //        vertex.uv = uvVec[vertPVec[i].uvIndex];
    //        vertices[i] = vertex;
    //    }

    //    // Fill out the buffer description to use when creating our vertex buffer.
    //    D3D11_BUFFER_DESC bufferDesc;
    //    ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
    //    bufferDesc.ByteWidth = sizeof(Vertex) * nrOfVertices;// The buffer needs to know the total size of its data, i.e. all vertices.
    //    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;			 // A buffer whose contents never change after creation is IMMUTABLE.
    //    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	 // For a vertex buffer, this flag must be specified.
    //    bufferDesc.CPUAccessFlags = 0;						 // The CPU won't access the buffer after creation.
    //    bufferDesc.MiscFlags = 0;							 // The buffer is not doing anything extraordinary.
    //    bufferDesc.StructureByteStride = 0;					 // Only used for structured buffers, which a vertex buffer is not.

    //                                                         // Define what data our buffer will contain.
    //    D3D11_SUBRESOURCE_DATA bufferContents;
    //    bufferContents.pSysMem = vertices;

    //    // Create the buffer.
    //    Core().device->CreateBuffer(&bufferDesc, &bufferContents, &vertexBuffer);

    //    delete vertices;
    //    posVec.clear();
    //    normVec.clear();
    //    uvVec.clear();
    //    vertPVec.clear();
    //}
}

OBJMesh::~OBJMesh() {
}
