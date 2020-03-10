#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

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
		enum Flags
		{
			  NONE = 0
			, NOT_SAVED = 1 << 0 // node is ignored while saving the scene
			, NO_DELETE = 1 << 1 // node cannot be deleted in the ui
		};

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
		bool HasFlag( uint32_t _flag ) { return flags & _flag; }
		void AddFlag( uint32_t _flag ) {  flags |= _flag; }
		void RemoveFlag( Flags _flag ) {  flags &= ~_flag; }

		EntityHandle			entityHandle;
		uint32_t				uniqueID;
		uint32_t				flags;
		Scene*					scene;
		SceneNode*				parent;
		std::vector<SceneNode*> childs;
		std::string				name;
	};
	static constexpr size_t sizeof_sceneNode = sizeof( SceneNode );
}