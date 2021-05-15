#pragma once

#include "fanJson.hpp"
#include <string>

namespace fan
{

    struct Mesh;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GLTFImporter
    {
        bool Load( const std::string& _relativePath );
        bool GetMesh( Mesh& _mesh );

        std::string mPath;    // file relative path
        Json        mJson;    // gltf json data
    };
}