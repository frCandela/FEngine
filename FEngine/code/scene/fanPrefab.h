#pragma once

#include "core/fanISerializable.h"

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	// represents a gameobjects tree
	// stores its data in a json
	//================================================================================================================================
	class Prefab  {
	public:
		Prefab();

		bool LoadFromFile( const std::string& _path );
		void LoadFromGameobject( const Gameobject * _gameobject );

		void		SetPath( const std::string _path ){ m_path = _path; }
		std::string GetPath( ) const { return m_path; }

		Json& GetData() { return m_json["prefab"]; }

		bool IsEmpty() { return ! m_json.contains("prefab"); }

	private:
		 Json m_json;
		 std::string m_path = "";
	};
}