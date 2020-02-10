#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/ressources/fanRessource.hpp"
#include "game/fanGameSerializable.hpp"

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	// represents a gameobjects tree
	// stores its data in a json
	//================================================================================================================================
	class Prefab : public Resource  {
	public:
		Prefab();

		bool LoadFromJson( const Json& _json );
		bool LoadFromFile( const std::string& _path );
		void LoadFromGameobject( const Gameobject * _gameobject );

		const Json& GetJsonGameobject() const	{ return m_json["prefab"]; }
		const Json& GetJson() const				{ return m_json; }
		std::string GetPath() const				{ return m_path; }

		bool IsEmpty() const { return ! m_json.contains("prefab"); }
		void Clear( ) { m_json = Json(); } 

	private:
		 Json m_json;
		 std::string m_path;
	};
}