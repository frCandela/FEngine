#include "scene/fanPrefab.hpp"
#include "scene/fanGameobject.hpp"

namespace fan
{
	PrefabManager Prefab::s_resourceManager;

	//================================================================================================================================
	//================================================================================================================================
	bool Prefab::CreateFromJson( const Json& _json )
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
	bool Prefab::CreateFromFile( const std::string& _path )
	{
		Clear();

		std::ifstream inStream( _path );
		if ( inStream.is_open() && inStream.good() )
		{
			Debug::Get() << Debug::Severity::log << "loading prefab: " << _path << Debug::Endl();
			inStream >> m_json;

			if ( m_json.contains( "prefab" ) )
			{
				m_path = _path;
				return true;
			}
			else
			{
				m_json = Json();
				Debug::Warning() << "file is not a prefab: " << _path << Debug::Endl();
				return false;
			}
		}
		else
		{
			Debug::Warning() << "Prefab failed to open file " << _path << Debug::Endl();
			return false;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Prefab::CreateFromGameobject( const Gameobject& _gameobject )
	{
		Clear();

		Json& prefabJson = m_json[ "prefab" ];
		if ( !_gameobject.Save( prefabJson ) )
		{
			Debug::Warning() << "Prefab creation failed for " << _gameobject.GetName() << Debug::Endl();
		}
		else
		{
			RemapGameobjectIndices( m_json );			
		}
	}

	//================================================================================================================================
	// Find all the gameobject indices in the json and remap them on a range close to zero
	// ex: 400, 401, 1051 will be remapped to 0,1,2
	//================================================================================================================================
	void Prefab::RemapGameobjectIndices( Json& _json)
	{
		std::vector< Json* > jsonIndices;
		std::set< uint64_t > uniqueIndices;

		// parse all the json and get all the gameobject ids
		std::stack< Json* > stack;
		stack.push( &_json );
		while ( ! stack.empty() )
		{
			Json& js = * stack.top();
			stack.pop();

			Json::iterator& gameobjectId = js.find( "gameobject_id" );
			if ( gameobjectId != js.end() )
			{
				uint64_t value = *gameobjectId;
				if ( value != 0 )
				{
					jsonIndices.push_back( &(*gameobjectId) );
					uniqueIndices.insert( value );
				}
			}

			if ( js.is_structured() ) {
				for ( auto& element : js )
				{
					stack.push( &element );
				}
			}
		}

		// creates the remap table
		std::map< uint64_t, uint64_t > remapTable;
		int remap = 1;
		for (uint64_t uniqueIndex : uniqueIndices )
		{
			remapTable[ uniqueIndex ] = remap++;
		}

		// remap all indices
		for (int jsonIdIndex = 0; jsonIdIndex < jsonIndices.size() ; jsonIdIndex++)
		{
			Json& js = *jsonIndices[ jsonIdIndex ];
			const uint64_t value = js;
			js = remapTable[ value ];
		}
	}
}