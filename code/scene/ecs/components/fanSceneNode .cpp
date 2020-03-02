#include "scene/ecs/components/fanSceneNode.hpp"

namespace fan
{
	REGISTER_COMPONENT( SceneNode, "scene_node" );
	
	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Init()
	{
		name = "";
		scene = nullptr;
		parent = nullptr;
		childs.clear();
	}

	//================================================================================================================================
	// Goes up the parents of _node to find this ancestor
	//================================================================================================================================bool IsAncestorOf( const SceneNode* _node ) const;
	bool SceneNode::IsAncestorOf( const SceneNode& _node ) const
	{
		const SceneNode* node = &_node;
		while( node->parent != nullptr )
		{
			if( node->parent == this )
			{
				return true;
			}
			else
			{
				node = node->parent;
			}
		} 
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================bool IsAncestorOf( const SceneNode* _node ) const;
	void SceneNode::RemoveChild( const SceneNode& _child )
	{
		for( int childIndex = 0; childIndex < childs.size(); childIndex++ )
		{
			SceneNode* child = childs[childIndex];
			if( child == &_child )
			{
				childs.erase( childs.begin() + childIndex );
				return;
			}
		}
	}

	//================================================================================================================================
	// Returns true if _child is a child of this
	//================================================================================================================================
	bool SceneNode::HasChild( const SceneNode& _child )
	{
		for( int childIndex = 0; childIndex < childs.size(); childIndex++ )
		{
			SceneNode* child = childs[childIndex];
			if( child == &_child )
			{
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::AddChild( SceneNode& _child )
	{
		if( _child.IsAncestorOf( *this ) )
		{
			Debug::Log( "Cannot parent an object to one of its children" );
			return;
		}

		if( _child.parent == this )
		{
			Debug::Get() << Debug::Severity::log << _child.name << " is already a child of " << name << Debug::Endl();
			return;
		}

		if( HasChild( _child ) == false )
		{
			if( _child.parent != nullptr )
			{
				_child.parent->RemoveChild( _child );
			}
			childs.push_back( &_child );
			_child.parent = this;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::SetParent( SceneNode* _parent )
	{
		if( _parent == nullptr )
		{
			Debug::Log( "Root cannot have a brother :'(" );
			return;
		}
		_parent->AddChild( *this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::InsertBelow( SceneNode& _brother )
	{
		if( IsAncestorOf( _brother ) )
		{
			Debug::Log( "Cannot parent an object to one of its children" );
			return;
		}
		if( _brother.parent == nullptr )
		{
			Debug::Log( "Root cannot have a brother :'(" );
			return;
		}

		parent->RemoveChild( *this );

		for( int childIndex = 0; childIndex < _brother.parent->childs.size(); childIndex++ )
		{
			SceneNode* child = _brother.parent->childs[childIndex];
			if( child == &_brother )
			{
				_brother.parent->childs.insert( _brother.parent->childs.begin() + childIndex + 1, this );
				parent = _brother.parent;
			}
		}
	}
}