#include "scene/fanDragnDrop.hpp"

#include "core/fanAssert.hpp"
#include "core/fanDebug.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletons/fanScene.hpp"
#include "render/resources/fanTexture.hpp"
#include "editor/fanGroupsColors.hpp"

namespace ImGui
{
    const std::string ComponentPayload::sPrefix = "cpnt_";
    const std::string SingletonPayload::sPrefix = "sing_";

	//========================================================================================================
	//========================================================================================================
	void FanBeginDragDropSourcePrefab( fan::Prefab* _prefab, ImGuiDragDropFlags _flags )
	{
		if ( _prefab != nullptr )
		{
			if ( ImGui::BeginDragDropSource( _flags ) )
			{
				ImGui::SetDragDropPayload( "dragndrop_prefab", &_prefab, sizeof( fan::Prefab** ) );
				ImGui::Icon( ImGui::IconType::Prefab16, { 16, 16 } ); ImGui::SameLine();
				ImGui::Text( ( _prefab->mPath ).c_str() );
				ImGui::EndDragDropSource();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	fan::Prefab* FanBeginDragDropTargetPrefab()
	{
		fan::Prefab* _prefab = nullptr;
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "dragndrop_prefab" ) )
			{
				assert( payload->DataSize == sizeof( fan::Prefab** ) );
				_prefab = *( fan::Prefab** )payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return _prefab;
	}

	//========================================================================================================
	//========================================================================================================
    void FanBeginDragDropSourceComponent( fan::EcsWorld& _world,
                                          fan::EcsHandle& _handle,
                                          uint32_t _type,
                                          ImGuiDragDropFlags _flags )
	{
		if( ImGui::BeginDragDropSource( _flags ) )
		{
			const fan::EcsComponentInfo& info = _world.GetComponentInfo( _type );

			std::string nameid = ComponentPayload::sPrefix + std::to_string( info.mType);
			ComponentPayload payload = { _handle , _type };
			ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
			ImGui::Icon( info.mIcon, { 16, 16 }, fan::GroupsColors::GetColor( info.mGroup ) );
			ImGui::SameLine();
			ImGui::Text( info.mName.c_str() );
			ImGui::EndDragDropSource();
		}		
	}

    //========================================================================================================
    //========================================================================================================
    void FanBeginDragDropSourceSingleton( fan::EcsWorld& _world, uint32_t _type, ImGuiDragDropFlags _flags )
    {
        if( ImGui::BeginDragDropSource( _flags ) )
        {
            const fan::EcsSingletonInfo& info = _world.GetSingletonInfo( _type );

            std::string nameid = SingletonPayload::sPrefix + std::to_string( info.mType );
            SingletonPayload payload = { _type };
            ImGui::SetDragDropPayload( nameid.c_str(), &payload, sizeof( payload ) );
            ImGui::Icon( info.mIcon, { 16, 16 }, fan::GroupsColors::GetColor( info.mGroup ) );
            ImGui::SameLine();
            ImGui::Text( info.mName.c_str() );
            ImGui::EndDragDropSource();
        }
    }

    //========================================================================================================
    //========================================================================================================
    bool ComponentPayload::IsComponentPayload( const ImGuiPayload* _payload )
    {
        std::string dataTypeStr = _payload->DataType;
        if( dataTypeStr.size() < ComponentPayload::sPrefix.size() ) { return false; }

        const std::string subString = dataTypeStr.substr( 0, ComponentPayload::sPrefix.size() );
        return subString == ComponentPayload::sPrefix;
    }

    //========================================================================================================
    //========================================================================================================
    bool SingletonPayload::IsSingletonPayload( const ImGuiPayload* _payload )
    {
        std::string dataTypeStr = _payload->DataType;
        if( dataTypeStr.size() < SingletonPayload::sPrefix.size() ){ return false; }

        const std::string subString = dataTypeStr.substr( 0, SingletonPayload::sPrefix.size() );
        return subString == SingletonPayload::sPrefix;
    }

	//========================================================================================================
	// if _type == 0 accepts all components types
	//========================================================================================================
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
			nameid = ComponentPayload::sPrefix + std::to_string( fan::SceneNode::Info::sType );
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
}