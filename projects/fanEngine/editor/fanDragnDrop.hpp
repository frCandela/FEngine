#pragma once

#include "imgui/imgui.h"
#include "core/ecs/fanEcsComponent.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
    class Prefab;
    struct EcsComponentInfo;
    class EcsWorld;
    struct Texture;
    struct Mesh;
    class Font;
}

namespace ImGui
{
    //==================================================================================================================================================================================================
    // helper struct for component drag & drop
    //==================================================================================================================================================================================================
    struct ComponentPayload
    {
        static const std::string sPrefix;
        fan::EcsHandle           mHandle        = 0;
        uint32_t                 mComponentType = 0;

        bool IsValid() const { return mHandle != 0; }
        static bool IsComponentPayload( const ImGuiPayload* _payload );
    };

    //==================================================================================================================================================================================================
    // helper struct for singleton drag & drop
    //==================================================================================================================================================================================================
    struct SingletonPayload
    {
        static const std::string sPrefix;
        uint32_t                 mType = 0;

        static bool IsSingletonPayload( const ImGuiPayload* _payload );
    };

    void FanBeginDragDropSourcePrefab( fan::ResourcePtr<fan::Prefab> _prefab, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::ResourcePtr<fan::Prefab> FanBeginDragDropTargetPrefab();
    void FanBeginDragDropSourceSingleton( fan::EcsWorld& _world, uint32_t _type, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    void FanBeginDragDropSourceComponent( fan::EcsWorld& _world, fan::EcsHandle& _handle, uint32_t _type, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world, uint32_t _type );
    void FanBeginDragDropSourceTexture( fan::ResourcePtr<fan::Texture>, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::ResourcePtr<fan::Texture> FanBeginDragDropTargetTexture();
    void FanBeginDragDropSourceMesh( fan::ResourcePtr<fan::Mesh> _mesh, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::ResourcePtr<fan::Mesh> FanBeginDragDropTargetMesh();
    void FanBeginDragDropSourceFont( fan::ResourcePtr<fan::Font> _font, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::ResourcePtr<fan::Font> FanBeginDragDropTargetFont();

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    template< typename _componentType >
    ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world )
    {
        static_assert( ( std::is_base_of<fan::EcsComponent, _componentType>::value ) );
        return FanBeginDragDropTargetComponent( _world, _componentType::Info::sType );
    }
}