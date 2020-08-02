#include "scene/fanSceneResourcePtr.hpp"

#include "scene/singletons/fanScenePointers.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"
#include "scene/fanDragnDrop.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	// initializes the ComponentPtr from ids :
	// - _sceneNodeID is the unique index of the target scene node on which the component is attached
	// - _staticID is the static id of the component
	//================================================================================================================================
	void ComponentPtrBase::CreateUnresolved( EcsHandle _handle )
	{
		handle = _handle;
		
		// adds to the unresolved pointers list
		if( _handle != 0 )
		{
			ScenePointers& scenePointers = world->GetSingleton<ScenePointers>();
			scenePointers.unresolvedComponentPtr.push_back( this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ComponentPtrBase::Create( EcsHandle _handle )
	{
		handle = _handle;
		assert( world->HasComponent( world->GetEntity( _handle ), type ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ComponentPtrBase::Clear()
	{
		handle = 0;
	}
}

namespace ImGui
{
	//================================================================================================================================
 	// returns true if the component pointer changed value
	//================================================================================================================================
 	bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr )
 	{
		fan::EcsWorld& world = *_ptr.world;
 		bool returnValue = false;
 
		const fan::EcsComponentInfo& info = world.GetComponentInfo( _ptr.type );

		// create button title
		std::string name;
		if( _ptr.handle == 0 )
		{
			name = info.name + " : NULL";
		}
		else
		{
			fan::SceneNode& node = world.GetComponent<fan::SceneNode>( world.GetEntity( _ptr.handle ) );
			name = info.name + " : " + node.name;
		}		
 		// icon
 		if (ImGui::ButtonIcon( info.icon, { 16,16 } ))
 		{
 			returnValue = true;
 		}
		// dragndrop source for icon
		if( _ptr.handle != 0 )
		{
			fan::SceneNode& node = world.GetComponent<fan::SceneNode>( world.GetEntity( _ptr.handle ) );
			ImGui::FanBeginDragDropSourceComponent( world, node.handle, _ptr.type );
		}
		// dragndrop target for icon
		ImGui::ComponentPayload payloadDropOnIcon = ImGui::FanBeginDragDropTargetComponent( world, _ptr.type );
 		
		ImGui::SameLine();
 
 		// name button 
 		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
		{
			// @todo select target in the editor
		}
 		ImGui::SameLine();
 
 		// dragndrop source for button
		if( _ptr.handle != 0 )
		{
			ImGui::FanBeginDragDropSourceComponent( world, _ptr.handle, _ptr.type );
		}

		// dragndrop target for button
  		ImGui::ComponentPayload payloadDropOnButton = ImGui::FanBeginDragDropTargetComponent( world, _ptr.type );
		if( payloadDropOnButton.handle != 0 || payloadDropOnIcon.handle != 0 )
		{
			ImGui::ComponentPayload & payload  = payloadDropOnButton.handle != 0 ? payloadDropOnButton : payloadDropOnIcon;
			_ptr.Create( payload.handle );
			returnValue = true;
		}
 
 		// Right click = clear
  		if (ImGui::IsItemClicked( 1 ))
  		{
  			_ptr.Clear();
			returnValue = true;
  		}
 
 		// label	
 		ImGui::Text( _label );
 
 		return returnValue;
 	}

	//================================================================================================================================
	//================================================================================================================================
	bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr )
	{
		bool returnValue = false;

		fan::Prefab* prefab = *_ptr;
		const std::string name = prefab == nullptr ? "null" : std::filesystem::path( prefab->GetPath() ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find prefab (" ) + _label + ")";
		static std::filesystem::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::PREFAB16, { 16,16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			m_pathBuffer = "content/prefab";
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourcePrefab( prefab );

		// tooltip
		if ( prefab != nullptr )
		{
			ImGui::FanToolTip( prefab->GetPath().c_str() );
		}

		// dragndrop		
		fan::Prefab* prefabDrop = ImGui::FanBeginDragDropTargetPrefab();
		if ( prefabDrop )
		{
			_ptr.Set( prefabDrop );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr.Set( nullptr );
			returnValue = true;
		}

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sPrefabExtensions, m_pathBuffer ) )
		{
			_ptr.Init( m_pathBuffer.string() );
			_ptr.Resolve();
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}
}
