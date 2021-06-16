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
        static const std::set<std::string> sAnimationsExtensions;

        static constexpr const char* sContentPath    = "/";
        static constexpr const char* sFontsPath      = "fonts/";
        static constexpr const char* sModelsPath     = "models/";
        static constexpr const char* sScenesPath     = "scenes/";
        static constexpr const char* sAnimationsPath = "animations/";
        static constexpr const char* sPrefabsPath    = "prefabs/";

        static constexpr const char* sEditorIcon = "_default/editorIcon.png";
        static constexpr const char* sGameIcon = "_default/gameIcon.png";
        static constexpr const char* sDefaultIcons = "_default/texture/icons.png";
        static constexpr const char* sDefaultImguiFont = "_default/imgui_font.ttf";
        static constexpr const char* sDefaultGameFont = "fonts/Vera.ttf";

        static constexpr const char* sMeshCone = "_default/mesh/cone.gltf";
        static constexpr const char* sMeshCube = "_default/mesh/cube.gltf";
        static constexpr const char* sMeshPlane = "_default/mesh/plane.gltf";
        static constexpr const char* sMeshSphere = "_default/mesh/sphere.gltf";
        static constexpr const char* sMeshAxisCheck = "_default/mesh/axis_check_yup.gltf";

        static constexpr const char* sDefaultMesh = "_default/mesh/cube.gltf";
        static constexpr const char* sDefaultTexture = "_default/texture/texture.png";
        static constexpr const char* sWhiteTexture = "_default/texture/white32.png";

        static constexpr const char* sMesh2DQuad = "2Dquad";

        static constexpr const char* sTextureUVCheck = "_default/texture/uv_checker.png";
        static constexpr const char* sTextureWhite = "_default/texture/white.png";

        static constexpr const char* sDefaultVertexShader = "shaders/empty.vert";
        static constexpr const char* sDefaultFragmentShader = "shaders/empty.frag";
        static constexpr const char* sImguiVertexShader = "shaders/imgui.vert";
        static constexpr const char* sImguiFragmentShader = "shaders/imgui.frag";

        static constexpr const uint32_t sMaxBonesInfluences         = 4;
        static constexpr const uint32_t sMaxBones                   = 64;
        static constexpr const uint32_t sMaximumNumPointLights      = 16;
        static constexpr const uint32_t sMaximumNumDirectionalLight = 4;

        static const std::vector<const char*> sDesiredValidationLayers;
        static const std::vector<const char*> sDesiredInstanceExtensions;
        static const std::vector<const char*> sDesiredDeviceExtensions;
    };
}