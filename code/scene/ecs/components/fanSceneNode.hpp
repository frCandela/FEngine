#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"

namespace fan
{
	class Scene;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SceneNode : public ecComponent
	{
		DECLARE_COMPONENT()
	public:

		void Clear();
		void Build( const std::string& _name, Scene& _scene, EntityHandle _entityHandle,  SceneNode* _parent );
		static void OnGui( ecComponent& _sceneNode );

		bool IsAncestorOf( const SceneNode& _node ) const;
		void RemoveChild( const SceneNode& _child );
		bool HasChild( const SceneNode& _child );
		void AddChild( SceneNode& _child );
		void SetParent( SceneNode* _parent );
		void InsertBelow( SceneNode& _brother );

		std::string				name;
		uint64_t				uniqueID;
		Scene*					scene;
		EntityHandle			entityHandle;
		SceneNode*				parent;
		std::vector<SceneNode*> childs;
	};
	static constexpr size_t sizeof_sceneNode = sizeof( SceneNode );
}