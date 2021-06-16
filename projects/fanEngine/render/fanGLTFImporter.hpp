#pragma once

#include "fanJson.hpp"
#include <string>
#include "fanGlm.hpp"
#include "render/resources/fanAnimation.hpp"
#include "render/resources/fanSkeleton.hpp"

namespace fan
{
    struct Mesh;
    struct SkinnedMesh;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GLTFImporter
    {
        bool Load( const std::string& _relativePath );
        void GetMesh( Mesh& _mesh );
        void GetMesh( SkinnedMesh& _mesh );
        void GetAnimation( Animation& _animation );

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
            std::string joints0Buffer;
            glm::u8vec4* joints0Array = nullptr;
            std::string weights0Buffer;
            glm::vec4* weights0Array = nullptr;
        };
        std::vector<GLTFSubmeshData> mSubmeshes;
        Skeleton mSkeleton;
        std::vector<Animation> mAnimations;
    };
}