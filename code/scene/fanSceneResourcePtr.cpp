#include "scene/fanSceneResourcePtr.hpp"

#include "scene/singletonComponents/fanScenePointers.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "editor/fanModals.hpp"
#include "scene/fanDragnDrop.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	// set the world of the pointer ( done during the Init() of the component on which the ComponentPtr is a member
	//================================================================================================================================
	void ComponentPtrBase::Init( EcsWorld& _world )
	{
		const_cast<EcsWorld*>( world ) = &_world;
		*const_cast<int*>( &dynamicID ) = world->GetIndex( staticID );
		sceneNodeID = 0;
		component = nullptr;
	}

	//================================================================================================================================
	// initializes the ComponentPtr from ids :
	// - _sceneNodeID is the unique index of the target scene node on which the component is attached
	// - _staticID is the static id of the component
	//================================================================================================================================
	void ComponentPtrBase::Create( uint32_t _sceneNodeID )
	{
		assert( world != nullptr );

		sceneNodeID = _sceneNodeID;
		
		// adds to the unresolved pointers list
		if( sceneNodeID != 0 )
		{
			ScenePointers& scenePointers = world->GetSingleton<ScenePointers>();
			scenePointers.unresolvedComponentPtr.insert( this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ComponentPtrBase::Create( SceneNode& _sceneNode, EcsComponent& _component )
	{
		assert( _sceneNode.scene->world == world );

		sceneNodeID = _sceneNode.uniqueID;
		component = &_component;

		// removes from unresolved pointers list
		ScenePointers& scenePointers = world->GetSingleton<ScenePointers>();
		auto it = scenePointers.unresolvedComponentPtr.find( this );
		if( it != scenePointers.unresolvedComponentPtr.end() )
		{
			scenePointers.unresolvedComponentPtr.erase( this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ComponentPtrBase::Clear()
	{
		sceneNodeID = 0;
		component = nullptr;
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
 
		const fan::EcsComponentInfo& info = world.GetComponentInfo( _ptr.dynamicID );

		// create button title
		std::string name;
		if( _ptr.component == nullptr )
		{
			name = info.name + " : NULL";
		}
		else
		{
			fan::Scene& scene = world.GetSingleton<fan::Scene>();
			fan::SceneNode& node = * scene.nodes[_ptr.sceneNodeID];
			name = info.name + " : " + node.name;
		}		
 
 		// icon
 		if (ImGui::ButtonIcon( info.icon, { 16,16 } ))
 		{
 			returnValue = true;
 		}
		// dragndrop source for icon
		if( _ptr.component != nullptr )
		{
			fan::Scene& scene = _ptr.world->GetSingleton<fan::Scene>();
			fan::SceneNode& node = *scene.nodes.at( _ptr.sceneNodeID );
			ImGui::FanBeginDragDropSourceComponent( node, *_ptr.component );
		}
		// dragndrop target for icon
		ImGui::ComponentPayload payloadDropOnIcon = ImGui::FanBeginDragDropTargetComponent( _ptr.staticID );
 		
		ImGui::SameLine();
 
 		// name button 
 		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
		{
			// @todo select target in the editor
		}
 		ImGui::SameLine();
 
 		// dragndrop source for button
		if( _ptr.component != nullptr )
		{
			fan::Scene& scene = _ptr.world->GetSingleton<fan::Scene>();
			fan::SceneNode& node = *scene.nodes.at( _ptr.sceneNodeID );
			ImGui::FanBeginDragDropSourceComponent( node, *_ptr.component );
		}

		// dragndrop target for button
  		ImGui::ComponentPayload payloadDropOnButton = ImGui::FanBeginDragDropTargetComponent( _ptr.staticID );
		if( payloadDropOnButton.sceneNode != nullptr || payloadDropOnIcon.sceneNode != nullptr )
		{
			ImGui::ComponentPayload & payload  = payloadDropOnButton.sceneNode != nullptr ? payloadDropOnButton : payloadDropOnIcon;
			_ptr.Create( *payload.sceneNode, *payload.component );
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
			_ptr = prefabDrop;
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr = nullptr;
			returnValue = true;
		}

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::s_prefabExtensions, m_pathBuffer ) )
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
