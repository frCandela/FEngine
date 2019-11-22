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

		Json& GetData() { return m_json["prefab"]; }

		bool IsEmpty() { return ! m_json.contains("prefab"); }

	private:
		 Json m_json;
	};
}