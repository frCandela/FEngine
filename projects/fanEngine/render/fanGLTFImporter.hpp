#pragma once

#include "fanJson.hpp"
#include <string>
#include "fanGlm.hpp"

namespace fan
{
    struct Mesh;
    struct MeshSkinned;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GLTFImporter
    {
        bool Load( const std::string& _relativePath );
        void GetMesh( Mesh& _mesh );
        void GetMesh( MeshSkinned& _mesh );

        std::string mPath;    // file relative path
        Json        mJson;    // gltf json data

    private:
        struct GLTFSubmeshData
        {
            int verticesCount = 0;
            int indicesCount  = 0;

            std::string indexBuffer;
            unsigned short* indicesArray;

            std::string posBuffer;
            glm::vec3* positionsArray = nullptr;
            std::string normalsBuffer;
            glm::vec3* normalsArray = nullptr;
            std::string texcoords0Buffer;
            glm::vec2* texcoords0Array = nullptr;
        };
        std::vector<GLTFSubmeshData> mSubmeshes;
    };
}