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
	bool Prefab::LoadFromJson( const Json& _json )
	{
		Clear();

		if ( _json.contains( "prefab" ) )
		{
			m_json = _json;
			return true;
		}
		else
		{
			Debug::Warning() << "json is not a prefab" << Debug::Endl();
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Prefab::LoadFromFile( const std::string& _path )
	{
		Clear();

		std::ifstream inStream(_path);
		if ( inStream.is_open() && inStream.good() )
		{
			Debug::Get() << Debug::Severity::log << "loading prefab: " << _path << Debug::Endl();
			inStream >> m_json;
			
			if ( m_json.contains("prefab") )
			{
				m_path = _path;
				return Ressource::LoadFromFile(_path);
			}
			else
			{
				m_json = Json();
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
		Clear();

		Json& prefabJson = m_json["prefab"];
		if( ! _gameobject->Save( prefabJson ) )
		{
			Debug::Warning() << "Prefab creation failed for " << _gameobject->GetName() << Debug::Endl();
		}
	}
}