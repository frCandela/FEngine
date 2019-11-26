#include "fanGlobalIncludes.h"
#include "scene/fanRessourcePtr.h"

#include "editor/fanDragnDrop.h"
#include "editor/fanImguiIcons.h"
#include "editor/fanModals.h"
#include "scene/fanGameobject.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanMesh.h"
#include "scene/fanPrefab.h"

namespace ImGui
{
	//================================================================================================================================
	//================================================================================================================================
	bool FanGameobject( const char * _label, fan::GameobjectPtr * _ptr )
	{
		bool returnValue = false;

		fan::Gameobject * gameobject = **_ptr;
		const std::string name = gameobject != nullptr ? gameobject->GetName() : "null";

		// icon & set from selection
		if ( ImGui::ButtonIcon( ImGui::IconType::GAMEOBJECT16, { 16,16 } ) )
		{
			fan::GameobjectPtr::s_onSetFromSelection.Emmit( _ptr );
			returnValue = true;
		}
		ImGui::SameLine();

		// name button 
		float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::SameLine();

		// dragndrop
		ImGui::FanBeginDragDropSourceGameobject( gameobject );
		fan::Gameobject * gameobjectDrop = ImGui::FanBeginDragDropTargetGameobject();
		if ( gameobjectDrop )
		{
			( *_ptr ) = fan::GameobjectPtr( gameobjectDrop, gameobjectDrop->GetUniqueID() );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::GameobjectPtr();
			returnValue = true;
		}

		// label	
		ImGui::Text( _label );

		return returnValue;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FanTexture( const char * _label, fan::TexturePtr * _ptr )
	{
		bool returnValue = false;

		fan::Texture * texture = **_ptr;
		const std::string name = texture == nullptr ? "null" : std::fs::path( texture->GetPath() ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find texture (" ) + _label + ")";
		static std::fs::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::IMAGE16, { 16,16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			m_pathBuffer = "content/models";
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceTexture( texture );

		// tooltip
		if ( texture != nullptr )
		{
			if ( ImGui::IsItemHovered() )
			{
				ImGui::BeginTooltip();

				// path
				ImGui::Text( texture->GetPath().c_str() );

				// size
				const glm::uvec3 size = texture->GetSize();
				std::stringstream ss;
				ss << size.x << " x " << size.y << " x " << size.z;
				ImGui::Text( ss.str().c_str() );

				ImGui::EndTooltip();
			}
		}

		// dragndrop		
		fan::Texture * textureDrop = ImGui::FanBeginDragDropTargetTexture();
		if ( textureDrop )
		{
			( *_ptr ) = fan::TexturePtr( textureDrop, textureDrop->GetPath() );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::TexturePtr();
			returnValue = true;
		}

		// Modal set value
		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::GlobalValues::s_imagesExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved( m_pathBuffer.string() );
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FanMesh( const char * _label, fan::MeshPtr * _ptr )
	{
		bool returnValue = false;

		fan::Mesh * mesh = **_ptr;
		const std::string name = mesh == nullptr ? "null" : std::fs::path( mesh->GetPath() ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find mesh (" ) + _label + ")";
		static std::fs::path m_pathBuffer;
		bool openModal = false;
		ImGui::PushID( _label );
		{
			if ( ImGui::ButtonIcon( ImGui::IconType::MESH16, { 16,16 } ) )
			{
				openModal = true;
			}
		} ImGui::PopID();
		if ( openModal )
		{
			ImGui::OpenPopup( modalName.c_str() );
			m_pathBuffer = "content/models";
		}
		ImGui::SameLine();

		// name button 
		const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
		ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
		ImGui::FanBeginDragDropSourceMesh( mesh );

		// tooltip
		if ( mesh != nullptr )
		{
			ImGui::FanToolTip( mesh->GetPath().c_str() );
		}

		// dragndrop		
		fan::Mesh * meshDrop = ImGui::FanBeginDragDropTargetMesh();
		if ( meshDrop )
		{
			( *_ptr ) = fan::MeshPtr( meshDrop, meshDrop->GetPath() );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::MeshPtr();
			returnValue = true;
		}

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::GlobalValues::s_meshExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved( m_pathBuffer.string() );
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FanPrefab( const char * _label, fan::PrefabPtr * _ptr )
	{
		bool returnValue = false;

		fan::Prefab * prefab = **_ptr;
		const std::string name = prefab == nullptr ? "null" : std::fs::path( prefab->GetPath() ).filename().string();

		// Set button icon & modal
		const std::string modalName = std::string( "Find prefab (" ) + _label + ")";
		static std::fs::path m_pathBuffer;
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
		fan::Prefab * prefabDrop = ImGui::FanBeginDragDropTargetPrefab();
		if ( prefabDrop )
		{
			( *_ptr ) = fan::PrefabPtr( prefabDrop, prefabDrop->GetPath() );
			returnValue = true;
		}

		// Right click = clear
		if ( ImGui::IsItemClicked( 1 ) )
		{
			( *_ptr ) = fan::PrefabPtr();
			returnValue = true;
		}

		if ( ImGui::FanLoadFileModal( modalName.c_str(), fan::GlobalValues::s_prefabExtensions, m_pathBuffer ) )
		{
			_ptr->InitUnresolved( m_pathBuffer.string() );
			returnValue = true;
		}

		ImGui::SameLine();
		ImGui::Text( _label );

		return returnValue;
	}
}
