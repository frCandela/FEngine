#include "fanSceneNode.hpp"

#include <stack>
#include "ecs/fanEcsWorld.hpp"
#include "core/fanDebug.hpp"
#include "engine/singletons/fanScene.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::SetInfo( EcsComponentInfo& _info )
    {
        _info.destroy = &SceneNode::Destroy;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        SceneNode& node = static_cast<SceneNode&>( _component );
        node.mHandle       = 0;
        node.mFlags        = 0;
        node.mName         = "";
        node.mScene        = nullptr;
        node.mParentHandle = 0;
        node.mChilds.clear();
    }

    //==================================================================================================================================================================================================
    // disconnects all the node's child hierarchy tree and kills it
    //==================================================================================================================================================================================================
    void SceneNode::Destroy( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        SceneNode& node  = static_cast<SceneNode&>( _component );
        Scene    & scene = _world.GetSingleton<Scene>();
        scene.mOnDeleteSceneNode.Emmit( &node );
        scene.mNodes.erase( node.mHandle );

        // removes from parent
        if( node.mParentHandle != 0 )
        {
            node.GetParent().RemoveChild( node );
        }

        if( !node.mChilds.empty() )
        {
            // stacks of initial nodes
            std::stack<EcsHandle> nodesstack;
            for( EcsHandle        child : node.mChilds )
            {
                nodesstack.push( child );
            }

            // find all child nodes and kills them
            while( !nodesstack.empty() )
            {
                const EcsEntity childEntity = _world.GetEntity( nodesstack.top() );
                SceneNode& childNode = _world.GetComponent<SceneNode>( childEntity );
                nodesstack.pop();

                for( int childIndex = 0; childIndex < (int)childNode.mChilds.size(); childIndex++ )
                {
                    nodesstack.push( childNode.mChilds[childIndex] );
                }

                childNode.mParentHandle = 0;
                childNode.mChilds.clear();
                _world.Kill( childEntity );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::Build( const std::string& _name, Scene& _scene, const EcsHandle _handle, SceneNode* const _parent, int _childIndex )
    {
        mScene  = &_scene;
        mHandle = _handle;
        mName   = _name;
        if( _parent != nullptr )
        {
            _parent->AddChild( *this, _childIndex );
        }
    }

    //==================================================================================================================================================================================================
    // Goes up the parents of _node to find this ancestor
    //==================================================================================================================================================================================================
    bool SceneNode::IsAncestorOf( const SceneNode& _node ) const
    {
        const SceneNode* node = &_node;
        while( node->mParentHandle != 0 )
        {
            if( node->mParentHandle == mHandle )
            {
                return true;
            }
            else
            {
                node = &node->GetParent();
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::RemoveChild( const SceneNode& _child )
    {
        for( int childIndex = 0; childIndex < (int)mChilds.size(); childIndex++ )
        {
            EcsHandle childHandle = mChilds[childIndex];
            if( childHandle == _child.mHandle )
            {
                mChilds.erase( mChilds.begin() + childIndex );
                return;
            }
        }
    }

    //==================================================================================================================================================================================================
    // Returns true if _child is a child of this
    //==================================================================================================================================================================================================
    bool SceneNode::HasChild( const SceneNode& _child )
    {
        for( int childIndex = 0; childIndex < (int)mChilds.size(); childIndex++ )
        {
            if( mChilds[childIndex] == _child.mHandle )
            {
                return true;
            }
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::AddChild( SceneNode& _child, int _childIndex )
    {
        if( _child.IsAncestorOf( *this ) )
        {
            Debug::Warning( "Cannot parent an object to one of its children" );
            return;
        }

        if( _child.mParentHandle == mHandle )
        {
            Debug::Warning() << _child.mName << " is already a child of " << mName << Debug::Endl();
            return;
        }

        if( !HasChild( _child ) )
        {
            if( _child.mParentHandle != 0 )
            {
                _child.GetParent().RemoveChild( _child );
            }

            const bool childIndexIsValid = _childIndex < 0 || mChilds[_childIndex] == 0;
            fanAssertMsg( childIndexIsValid, "invalid child index override when parenting scene node" );
            if( _childIndex >= 0 && childIndexIsValid )
            {
                mChilds[_childIndex] = _child.mHandle;
            }
            else
            {
                mChilds.push_back( _child.mHandle );
            }

            _child.mParentHandle = mHandle;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::SetParent( SceneNode* _parent )
    {
        if( _parent == nullptr )
        {
            Debug::Warning( "Root cannot have a brother :'(" );
            return;
        }
        _parent->AddChild( *this );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    SceneNode& SceneNode::GetParent() const
    {
        fanAssert( mParentHandle != 0 );
        return mScene->mWorld->GetComponent<SceneNode>( mScene->mWorld->GetEntity( mParentHandle ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SceneNode::InsertBelow( SceneNode& _brother )
    {
        if( IsAncestorOf( _brother ) )
        {
            Debug::Warning( "Cannot parent an object to one of its children" );
            return;
        }
        if( _brother.mParentHandle == 0 )
        {
            Debug::Warning( "Root cannot have a brother :'(" );
            return;
        }

        GetParent().RemoveChild( *this );

        SceneNode& brotherParent = _brother.GetParent();
        for( int childIndex = 0; childIndex < (int)brotherParent.mChilds.size(); childIndex++ )
        {
            if( brotherParent.mChilds[childIndex] == _brother.mHandle )
            {
                brotherParent.mChilds.insert( brotherParent.mChilds.begin() + childIndex + 1, mHandle );
                mParentHandle = _brother.mParentHandle;
            }
        }
    }
    //==================================================================================================================================================================================================
    // fills the _list with _root and all its descendants
    //==================================================================================================================================================================================================
    void SceneNode::GetDescendantsOf( const SceneNode& _root, std::vector<SceneNode*>& _outList )
    {
        EcsWorld& world = *_root.mScene->mWorld;

        _outList.clear();
        std::stack<EcsHandle> stack;
        stack.push( _root.mHandle );
        while( !stack.empty() )
        {
            SceneNode& node = world.GetComponent<SceneNode>( world.GetEntity( stack.top() ) );
            stack.pop();
            _outList.push_back( &node );
            for( int i = 0; i < (int)node.mChilds.size(); i++ )
            {
                stack.push( node.mChilds[i] );
            }
        }
    }
}