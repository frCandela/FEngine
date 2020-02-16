#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanGameobject.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "core/imgui/fanImguiIcons.hpp"
#include "core/imgui/fanModals.hpp"
#include "render/fanRenderGlobal.hpp"


namespace ImGui
{
	static_assert( ( std::is_base_of<fan::Resource, fan::Gameobject>::value ) );

	//================================================================================================================================
	//================================================================================================================================
	bool FanGameobject( const char* _label, fan::GameobjectPtr* _ptr )
	{
		bool returnValue = false;

		fan::Gameobject* gameobject = **_ptr;
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
			_ptr->SetResource( gameobjectDrop );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			_ptr->SetResource( nullptr );
			returnValue = true;
		}

		// label	
		ImGui::Text( _label );

		return returnValue;
	}

	static_assert( ( std::is_base_of<fan::Resource, fan::Prefab>::value ) );
	
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
