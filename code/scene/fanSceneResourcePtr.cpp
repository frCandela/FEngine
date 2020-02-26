#include "scene/fanSceneResourcePtr.hpp"

#include "scene/fanGameobject.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanScene.hpp"
#include "core/imgui/fanImguiIcons.hpp"
#include "core/imgui/fanModals.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameobjectPtr::~GameobjectPtr()
	{
		( *this ) = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameobjectPtr::Init( Scene& _scene, uint64_t _gameobjectId )
	{
		m_gameobjectId = _gameobjectId;
		m_scene = &_scene;
		if ( m_gameobjectId != 0 )
		{
			_scene.GetInstanciator().RegisterUnresolvedGameobjectPtr( *this );
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	GameobjectPtr& GameobjectPtr::operator=( Gameobject* _resource ) 
	{ 
		if ( _resource == GetResource() ) { return  *this; }

		if ( GetResource() != nullptr ) 
		{
			assert( m_scene != nullptr );
			m_scene->GetInstanciator().UnregisterGameobjectPtr( *this );
		}

		if ( _resource != nullptr )
		{
			m_scene = & _resource->GetScene();
			m_scene->GetInstanciator().RegisterGameobjectPtr( *this );
		}
		else
		{
			m_scene = nullptr;
		}
		
		SetResource( _resource ); 

		return *this; 
	}
}

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	bool FanGameobject( const char* _label, fan::GameobjectPtr& _ptr )
	{
		bool returnValue = false;

		fan::Gameobject* gameobject = *_ptr;
		const std::string name = gameobject != nullptr ? gameobject->GetName() : "null";

		// icon & set from selection
		if ( ImGui::ButtonIcon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ) )
		{
			returnValue = true;
		}
		ImGui::SameLine();

		// name button 
		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::SameLine();

		// dragndrop
		ImGui::FanBeginDragDropSourceGameobject( gameobject );
		fan::Gameobject* gameobjectDrop = ImGui::FanBeginDragDropTargetGameobject();
		if ( gameobjectDrop )
		{
			_ptr = gameobjectDrop;
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr = nullptr;
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
