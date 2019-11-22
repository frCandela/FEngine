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

		bool LoadFromPath( const std::string& _path );
		void LoadFromGameobject( const Gameobject * _gameobject );
		void LoadToGameobject( Gameobject * _gameobject );

		bool IsEmpty() { return ! m_json.contains("prefab"); }

	private:
		 Json m_json;
	};
}