#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"

namespace fan
{
	class Scene;
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SceneNode : public ecComponent
	{
		DECLARE_COMPONENT( SceneNode )
	public:

		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _sceneNode );
		static void OnGui( ecComponent& _sceneNode );

		void Init( const std::string& _name, Scene& _scene, const EntityHandle _entityHandle, const uint32_t _uniqueID, SceneNode* const _parent);

		bool IsRoot() const { return parent == nullptr; }
		bool IsAncestorOf( const SceneNode& _node ) const;
		void RemoveChild( const SceneNode& _child );
		bool HasChild( const SceneNode& _child );
		void AddChild( SceneNode& _child );
		void SetParent( SceneNode* _parent );
		void InsertBelow( SceneNode& _brother );

		std::string				name;
		uint32_t				uniqueID;
		Scene*					scene;
		EntityHandle			entityHandle;
		SceneNode*				parent;
		std::vector<SceneNode*> childs;
	};
	static constexpr size_t sizeof_sceneNode = sizeof( SceneNode );
}