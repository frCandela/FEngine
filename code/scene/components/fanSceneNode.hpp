#pragma  once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	struct Scene;

	//==============================================================================================================================================================
	// node of the scene tree
	// Contains reference to a parent, a list of childs and an entity handle 
	//==============================================================================================================================================================
	struct SceneNode : public EcsComponent
	{
		ECS_COMPONENT( SceneNode )
	public:
		enum Flags
		{
			  NONE = 0
			, NOT_SAVED  = 1 << 0 // node is ignored while saving the scene
			, NO_DELETE  = 1 << 1 // node cannot be deleted in the ui
			, NO_RAYCAST = 1 << 2 // node cannot be selected by raycast
		};

		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		void Build( const std::string& _name, Scene& _scene, const EcsHandle _handle, const uint32_t _uniqueID, SceneNode* const _parent);

		bool IsRoot() const { return parent == nullptr; }
		bool IsAncestorOf( const SceneNode& _node ) const;
		void RemoveChild( const SceneNode& _child );
		bool HasChild( const SceneNode& _child );
		void AddChild( SceneNode& _child );
		void SetParent( SceneNode* _parent );
		void InsertBelow( SceneNode& _brother );
		bool HasFlag( uint32_t _flag ) const { return flags & _flag; }
		void AddFlag( uint32_t _flag ) {  flags |= _flag; }
		void RemoveFlag( Flags _flag ) {  flags &= ~_flag; }

		static void GetDescendantsOf( SceneNode& _root, std::vector<SceneNode*>& _outList );

		EcsHandle			handle;
		uint32_t				uniqueID;
		uint32_t				flags;
		Scene*					scene;
		SceneNode*				parent;
		std::vector<SceneNode*> childs;
		std::string				name;
	};
	static constexpr size_t sizeof_sceneNode = sizeof( SceneNode );
}