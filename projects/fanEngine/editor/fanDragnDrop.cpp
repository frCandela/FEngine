#include "fanDragnDrop.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "core/fanDebug.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "render/resources/fanAnimation.hpp"
#include "editor/fanGroupsColors.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanResourceInfos.hpp"
#include "editor/singletons/fanEditorSettings.hpp"


namespace ImGui
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanBeginDragDropSourceResource( fan::ResourcePtrData& _resourcePtrData, const uint32_t _infoType, ImGuiDragDropFlags _flags )
    {
        if( ImGui::BeginDragDropSource( _flags ) )
        {
            const fan::EditorResourceInfo& resourceInfo = fan::EditorResourceInfo::sResourceInfos.at(_infoType);
            ImGui::SetDragDropPayload( resourceInfo.mResourceName, &_resourcePtrData, sizeof( fan::ResourcePtrData ) );
            ImGui::Icon( resourceInfo.mIcon, { 16, 16 } );
            ImGui::SameLine();
            ImGui::Text( "%s", _resourcePtrData.mHandle->mResource->mPath.c_str() );
            ImGui::EndDragDropSource();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    fan::ResourcePtrData FanBeginDragDropTargetResource( const uint32_t _infoType  )
    {
        fan::ResourcePtrData resourcePtrData;
        if( ImGui::BeginDragDropTarget() )
        {
            const fan::EditorResourceInfo& resourceInfo = fan::EditorResourceInfo::sResourceInfos.at(_infoType);
            if( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( resourceInfo.mResourceName ) )
            {
                fanAssert( payload->DataSize == sizeof( fan::ResourcePtrData ) );
                resourcePtrData = *(fan::ResourcePtrData*)payload->Data;
            }
            ImGui::EndDragDropTarget();
        }
        return resourcePtrData;
    }

    const std::string ComponentPayload::sPrefix = "cpnt_";
    const std::string SingletonPayload::sPrefix = "sing_";

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
            ImGui::Text( "%s", info.mName.c_str() );
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
            ImGui::Text( "%s", info.mName.c_str() );
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
                    fan::Debug::Warning() << "dropped scene node doesn't have a " << info.mName << " component" << fan::Debug::Endl();
                }
            }

            ImGui::EndDragDropTarget();
        }
        return ComponentPayload();
    }
}