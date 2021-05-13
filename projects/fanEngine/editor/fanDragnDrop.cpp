#include "fanDragnDrop.hpp"

#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/fanPrefab.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/gui/fanGroupsColors.hpp"
#include "editor/fanGuiInfos.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanFont.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/singletons/fanEditorSettings.hpp"

namespace ImGui
{
    const std::string ComponentPayload::sPrefix = "cpnt_";
    const std::string SingletonPayload::sPrefix = "sing_";

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourcePrefab( fan::Prefab* _prefab, ImGuiDragDropFlags _flags )
    {
        if( _prefab != nullptr )
        {
            if( ImGui::BeginDragDropSource( _flags ) )
            {
                ImGui::SetDragDropPayload( "dragndrop_prefab", &_prefab, sizeof( fan::Prefab** ) );
                ImGui::Icon( ImGui::IconType::Prefab16, { 16, 16 } );
                ImGui::SameLine();
                ImGui::Text( ( _prefab->mPath ).c_str() );
                ImGui::EndDragDropSource();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    fan::Prefab* FanBeginDragDropTargetPrefab()
    {
        fan::Prefab* _prefab = nullptr;
        if( ImGui::BeginDragDropTarget() )
        {
            if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_prefab" ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::Prefab** ) );
                _prefab = *(fan::Prefab**)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return _prefab;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceComponent( fan::EcsWorld& _world, fan::EcsHandle& _handle, uint32_t _type, ImGuiDragDropFlags _flags )
    {
        if( ImGui::BeginDragDropSource( _flags ) )
        {
            const fan::EcsComponentInfo& info     = _world.GetComponentInfo( _type );
            const fan::EditorSettings  & settings = _world.GetSingleton<fan::EditorSettings>();
            const fan::GuiComponentInfo& guiInfo  = settings.GetComponentInfo( info.mType );

            std::string      nameid  = ComponentPayload::sPrefix + std::to_string( info.mType );
            ComponentPayload payload = { _handle, _type };
            ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
            ImGui::Icon( guiInfo.mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo.mGroup ) );
            ImGui::SameLine();
            ImGui::Text( info.mName.c_str() );
            ImGui::EndDragDropSource();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceSingleton( fan::EcsWorld& _world, uint32_t _type, ImGuiDragDropFlags _flags )
    {
        if( ImGui::BeginDragDropSource( _flags ) )
        {
            const fan::EcsSingletonInfo& info     = _world.GetSingletonInfo( _type );
            const fan::EditorSettings  & settings = _world.GetSingleton<fan::EditorSettings>();
            const fan::GuiSingletonInfo& guiInfo  = settings.GetSingletonInfo( info.mType );

            std::string      nameid  = SingletonPayload::sPrefix + std::to_string( info.mType );
            SingletonPayload payload = { _type };
            ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
            ImGui::Icon( guiInfo.mIcon, { 16, 16 }, settings.mData->mGroupsColors.GetColor( guiInfo.mGroup ) );
            ImGui::SameLine();
            ImGui::Text( info.mName.c_str() );
            ImGui::EndDragDropSource();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool ComponentPayload::IsComponentPayload( const ImGuiPayload* _payload )
    {
        std::string dataTypeStr = _payload->DataType;
        if( dataTypeStr.size() < ComponentPayload::sPrefix.size() ){ return false; }

        const std::string subString = dataTypeStr.substr( 0, ComponentPayload::sPrefix.size() );
        return subString == ComponentPayload::sPrefix;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool SingletonPayload::IsSingletonPayload( const ImGuiPayload* _payload )
    {
        std::string dataTypeStr = _payload->DataType;
        if( dataTypeStr.size() < SingletonPayload::sPrefix.size() ){ return false; }

        const std::string subString = dataTypeStr.substr( 0, SingletonPayload::sPrefix.size() );
        return subString == SingletonPayload::sPrefix;
    }

    //==================================================================================================================================================================================================
    // if _type == 0 accepts all components types
    //==================================================================================================================================================================================================
    ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world, uint32_t _type )
    {
        using namespace fan;

        if( ImGui::BeginDragDropTarget() )
        {
            std::string nameid;
            if( _type == 0 ) // accept all components
            {
                const ImGuiPayload* preGetPayload = ImGui::GetDragDropPayload();
                if( preGetPayload != nullptr && ComponentPayload::IsComponentPayload( preGetPayload ) )
                {
                    nameid = preGetPayload->DataType;
                }
            }

            // accept only components of a given type
            if( nameid.empty() )
            {
                nameid = ComponentPayload::sPrefix + std::to_string( _type );
            }

            // Drop payload component
            const ImGuiPayload* imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
            if( imGuiPayload != nullptr )
            {
                fanAssert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
                return *(ComponentPayload*)imGuiPayload->Data;
            }

            // Drop payload scene node
            nameid       = ComponentPayload::sPrefix + std::to_string( fan::SceneNode::Info::sType );
            imGuiPayload = ImGui::AcceptDragDropPayload( nameid.c_str() );
            if( imGuiPayload != nullptr )
            {
                // find the component and assigns it if it exists
                fanAssert( imGuiPayload->DataSize == sizeof( ComponentPayload ) );
                ComponentPayload& payload = *(ComponentPayload*)imGuiPayload->Data;
                if( _world.HasComponent( _world.GetEntity( payload.mHandle ), _type ) )
                {
                    return payload;
                }
                else
                {
                    const fan::EcsComponentInfo& info = _world.GetComponentInfo( _type );
                    fan::Debug::Warning()
                            << "dropped scene node doesn't have a "
                            << info.mName
                            << " component" << fan::Debug::Endl();
                }
            }

            ImGui::EndDragDropTarget();
        }
        return ComponentPayload();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceTexture( fan::Texture* _texture, ImGuiDragDropFlags _flags )
    {
        if( _texture != nullptr )
        {
            if( ImGui::BeginDragDropSource( _flags ) )
            {
                ImGui::SetDragDropPayload( "dragndrop_texture", &_texture, sizeof( fan::Texture** ) );
                ImGui::Icon( ImGui::IconType::Image16, { 16, 16 } );
                ImGui::SameLine();
                ImGui::Text( _texture->mPath.c_str() );
                ImGui::EndDragDropSource();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    fan::Texture* FanBeginDragDropTargetTexture()
    {
        fan::Texture* texture = nullptr;
        if( ImGui::BeginDragDropTarget() )
        {
            if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_texture" ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::Texture** ) );
                texture = *(fan::Texture**)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return texture;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceMesh( fan::Mesh* _mesh, ImGuiDragDropFlags _flags )
    {
        if( _mesh != nullptr )
        {
            if( ImGui::BeginDragDropSource( _flags ) )
            {
                ImGui::SetDragDropPayload( "dragndrop_mesh", &_mesh, sizeof( fan::Mesh** ) );
                ImGui::Icon( ImGui::IconType::Mesh16, { 16, 16 } );
                ImGui::SameLine();
                ImGui::Text( _mesh->mPath.c_str() );
                ImGui::EndDragDropSource();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    fan::Mesh* FanBeginDragDropTargetMesh()
    {
        fan::Mesh* mesh = nullptr;
        if( ImGui::BeginDragDropTarget() )
        {
            if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_mesh" ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::Mesh** ) );
                mesh = *(fan::Mesh**)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return mesh;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceFont( fan::Font* _font, ImGuiDragDropFlags _flags )
    {
        if( _font != nullptr )
        {
            if( ImGui::BeginDragDropSource( _flags ) )
            {
                ImGui::SetDragDropPayload( "dragndrop_font", &_font, sizeof( fan::Font** ) );
                ImGui::Icon( ImGui::IconType::Font16, { 16, 16 } );
                ImGui::SameLine();
                ImGui::Text( _font->mPath.c_str() );
                ImGui::EndDragDropSource();
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    fan::Font* FanBeginDragDropTargetFont()
    {
        fan::Font* font = nullptr;
        if( ImGui::BeginDragDropTarget() )
        {
            if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_font" ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::Font** ) );
                font = *(fan::Font**)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return font;
    }
}