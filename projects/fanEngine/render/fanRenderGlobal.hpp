#pragma once

#include <set>
#include <cstdint>
#include <string>
#include <vector>

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class RenderGlobal
    {
    public:
        static const std::set<std::string> sMeshExtensions;
        static const std::set<std::string> sImagesExtensions;
        static const std::set<std::string> sSceneExtensions;
        static const std::set<std::string> sPrefabExtensions;
        static const std::set<std::string> sFontsExtensions;

        static const char          * sContentPath;
        static const char          * sFontsPath;
        static const char          * sModelsPath;
        static const char          * sScenesPath;
        static constexpr const char* sPrefabsPath = "prefabs/";

        static const char* sDefaultMesh;
        static const char* sDefaultTexture;
        static const char* sWhiteTexture;

        static const char* sEditorIcon;
        static const char* sGameIcon;
        static const char* sDefaultIcons;
        static const char* sDefaultImguiFont;
        static const char* sDefaultGameFont;

        static const char* sMeshCone;
        static const char* sMeshCube;
        static const char* sMeshPlane;
        static const char* sMeshSphere;
        static const char* sMeshAxisCheck;

        static const char* sMesh2DQuad;

        static const char* sTextureUVCheck;
        static const char* sTextureWhite;

        static const char* sDefaultVertexShader;
        static const char* sDefaultFragmentShader;
        static const char* sImguiVertexShader;
        static const char* sImguiFragmentShader;

        static constexpr const uint32_t sMaxBonesInfluences         = 4;
        static constexpr const uint32_t sMaximumNumPointLights      = 16;
        static constexpr const uint32_t sMaximumNumDirectionalLight = 4;

        static const std::vector<const char*> sDesiredValidationLayers;
        static const std::vector<const char*> sDesiredInstanceExtensions;
        static const std::vector<const char*> sDesiredDeviceExtensions;
    };
}