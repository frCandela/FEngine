#pragma once

#include "core/resources/fanResource.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	struct SceneNode;

	//========================================================================================================
	// represents a gameobjects tree
	// stores its data in a json
	// Allow scene node tree instantiation ( copy / paste, prefabs )
	//========================================================================================================
	class Prefab : public Resource
	{
	public:
		bool CreateFromJson( const Json& _json );
		bool CreateFromFile( const std::string& _path );
		void CreateFromSceneNode( const SceneNode& _node );
		SceneNode* Instantiate( SceneNode& _parent ) const;

		bool IsEmpty() const { return !mJson.contains( "prefab" ); }
		void Clear() { mJson = Json(); }

		Json        mJson;
		std::string mPath;
	};
}