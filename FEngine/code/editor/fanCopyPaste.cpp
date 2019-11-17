#include "fanGlobalIncludes.h"
#include "editor/fanCopyPaste.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	// Copy gameobject to clipboard
	//================================================================================================================================
	void CopyPaste::Copy( const Gameobject * _gameobject )
	{
		if ( _gameobject != nullptr )
		{
			Json objectJson;
			_gameobject->Save( objectJson );

			std::stringstream ss;
			ss << objectJson;

			ImGui::SetClipboardText( ss.str().c_str() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CopyPaste::Paste( Scene * _scene, Gameobject * _parentGameobject )
	{
		m_newGameobjectPtr.clear();
		m_newComponentPtr.clear();
		m_remapTable.clear();
		m_scene = _scene;

		Gameobject::s_setIDfailed.Connect( &CopyPaste::OnSetIDFailed, this );
		GameobjectPtr::s_onCreateUnresolved.Connect ( &CopyPaste::OnGameobjectPtrCreate, this );
		ComponentIDPtr::s_onCreateUnresolved.Connect( &CopyPaste::OnComponentIDPtrCreate, this );

		// Load gameobject
		std::stringstream ss;
		ss << ImGui::GetClipboardText();
		Json objectJson;
		ss >> objectJson;
		Gameobject * gameobject = _scene->CreateGameobject( "tmp", _parentGameobject, false );
		gameobject->Load( objectJson );

		Gameobject::s_setIDfailed.Disconnect( &CopyPaste::OnSetIDFailed, this );
		GameobjectPtr::s_onCreateUnresolved.Disconnect( &CopyPaste::OnGameobjectPtrCreate, this );
		ComponentIDPtr::s_onCreateUnresolved.Disconnect( &CopyPaste::OnComponentIDPtrCreate, this );

		ResolvePointers();
	}

	//================================================================================================================================
	// Resolves component & gameobject pointers
	//================================================================================================================================
	void  CopyPaste::ResolvePointers()
	{
		// Resolves gameobjects
		for ( int goPtrIndex = 0; goPtrIndex < m_newGameobjectPtr.size(); goPtrIndex++ )
		{
			GameobjectPtr * ptr = m_newGameobjectPtr[goPtrIndex];

			auto it = m_remapTable.find( ptr->GetID() );
			const uint64_t index = ( it != m_remapTable.end() ? it->second : ptr->GetID() );
			Gameobject * gameobject = m_scene->FindGameobject( index );
			assert( gameobject != nullptr );
			*ptr = GameobjectPtr( gameobject, index );
		}

		// Resolves components
		for ( int componentPtrIndex = 0; componentPtrIndex < m_newComponentPtr.size(); componentPtrIndex++ )
		{
			ComponentIDPtr * ptr = m_newComponentPtr[componentPtrIndex];

			auto it = m_remapTable.find( ptr->GetID().gameobjectID );
			const uint64_t index = ( it != m_remapTable.end() ? it->second : ptr->GetID().gameobjectID );
			Gameobject * gameobject = m_scene->FindGameobject( index );
			assert( gameobject != nullptr );
			Component * component = gameobject->GetComponent( ptr->GetID().componentID );
			*ptr = ComponentIDPtr( component, IDPtrData( index, ptr->GetID().componentID ) );
		}
	}

	//================================================================================================================================
	// Saves the duplicates ids for future remap
	//================================================================================================================================
	void CopyPaste::OnSetIDFailed( uint64_t _id, Gameobject * _gameobject )
	{
		assert( _id != 0 );
		const uint64_t remapID = m_scene->GetUniqueID();
		_gameobject->SetUniqueID( remapID );
		m_remapTable[_id] = remapID;
		Debug::Log() << "remapped id " << _id << " to id " << remapID << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void CopyPaste::OnGameobjectPtrCreate( GameobjectPtr * _ptr )
	{
		if ( _ptr->GetID() != 0 )
		{
			m_newGameobjectPtr.push_back( _ptr );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void CopyPaste::OnComponentIDPtrCreate( ComponentIDPtr * _ptr )
	{
		if ( _ptr->GetID().gameobjectID != 0 )
		{
			m_newComponentPtr.push_back( _ptr );
		}
	}
}