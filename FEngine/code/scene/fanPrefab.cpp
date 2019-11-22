#include "fanGlobalIncludes.h"
#include "scene/fanPrefab.h"

#include "scene/fanGameobject.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Prefab::Prefab()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	bool Prefab::LoadFromPath( const std::string& _path )
	{
		std::ifstream inStream(_path);
		if ( inStream.is_open() && inStream.good() )
		{
			Debug::Get() << Debug::Severity::log << "loading prefab: " << _path << Debug::Endl();
			inStream >> m_json;

			
			if ( m_json.contains("prefab") )
			{
				return true;

			}
			else
			{
				Debug::Warning()<< "file is not a prefab: " << _path << Debug::Endl();
				return false;
			}
		}
		else
		{
			Debug::Warning()<< "Prefab failed to open file " << _path << Debug::Endl();
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Prefab::LoadFromGameobject( const Gameobject * _gameobject )
	{
		assert( ! m_json.contains("prefab") );

		Json& prefabJson = m_json["prefab"];
		if( ! _gameobject->Save( prefabJson ) )
		{
			Debug::Warning() << "Prefab creation failed for " << _gameobject->GetName() << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Prefab::LoadToGameobject( Gameobject * _gameobject )
	{
		assert( m_json.contains("prefab") );
		_gameobject->Load( m_json["prefab"] );
	}
}