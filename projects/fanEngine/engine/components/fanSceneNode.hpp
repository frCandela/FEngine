#pragma  once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
    struct Scene;

    //==================================================================================================================================================================================================
    // node of the scene tree
    // Contains reference to a parent, a list of child and an entity handle
    //==================================================================================================================================================================================================
    struct SceneNode : public EcsComponent
    {
    ECS_COMPONENT( SceneNode )
        enum Flags
        {
            None           = 0,
            NoSave         = 1 << 0,            // node is ignored while saving the scene
            NoDelete       = 1 << 1,            // node cannot be deleted in the ui
            NoRaycast      = 1 << 2,            // node cannot be selected by raycast
            BoundsOutdated = 1 << 3,    // bounds need to be recomputed
        };

        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

        void Build( const std::string& _name, Scene& _scene, const EcsHandle _handle, SceneNode* const _parent, int _childIndex = -1 );

        bool IsRoot() const { return mParentHandle == 0; }
        bool IsAncestorOf( const SceneNode& _node ) const;
        void RemoveChild( const SceneNode& _child );
        bool HasChild( const SceneNode& _child );
        void AddChild( SceneNode& _child, int _childIndex = -1 );
        void SetParent( SceneNode* _parent );
        SceneNode& GetParent() const;
        void InsertBelow( SceneNode& _brother );
        bool HasFlag( const Flags _flag ) const { return mFlags & _flag; }
        void AddFlag( const Flags _flag ) { mFlags |= _flag; }
        void RemoveFlag( Flags _flag ) { mFlags &= ~_flag; }

        static void GetDescendantsOf( const SceneNode& _root, std::vector<SceneNode*>& _outList );

        template< class _ComponentType >
        static _ComponentType* FindComponentInChildren( const SceneNode& _sceneNode )
        {
            static_assert( std::is_base_of<EcsComponent, _ComponentType>::value );
            return static_cast<_ComponentType*>(FindComponentInChildren( _sceneNode, _ComponentType::Info::sType ));
        }
        static EcsComponent* FindComponentInChildren( const SceneNode& _node, const uint32_t _componentType );
        static void ExecuteInChildren( SceneNode& _sceneNode, void (* _visitor)( SceneNode& _sceneNode ) );

        EcsHandle mHandle;
        EcsHandle mParentHandle;
        uint32_t  mFlags;
        Scene* mScene;
        std::vector<EcsHandle> mChilds;
        std::string            mName;

    private:
        static EcsComponent* RFindComponentInChildren( EcsWorld& _world, const SceneNode& _node, const uint32_t _componentIndex );
        static void RExecuteInChildren( EcsWorld& _world, SceneNode& _sceneNode, void (* _visitor)( SceneNode& _sceneNode ) );
    };
}