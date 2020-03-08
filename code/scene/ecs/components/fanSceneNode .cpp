#include "scene/ecs/components/fanSceneNode.hpp"

#include "scene/fanScene.hpp"

namespace fan
{
	REGISTER_COMPONENT( SceneNode, "scene_node" );
	
	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::SetInfo( ComponentInfo& _info )
	{
		_info.icon  = ImGui::IconType::GAMEOBJECT16;
		_info.onGui = &SceneNode::OnGui;
		_info.clear = &SceneNode::Clear;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Clear( ecComponent& _sceneNode )
	{
		SceneNode& node = static_cast<SceneNode&>( _sceneNode );
		node.entityHandle = 0;
		node.name = "";
		node.scene = nullptr;
		node.parent = nullptr;
		node.childs.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Init( const std::string& _name, Scene& _scene, const EntityHandle _entityHandle, const uint32_t _uniqueID, SceneNode* const _parent )
	{
		name = _name;
		scene = &_scene;
		entityHandle = _entityHandle;
		uniqueID = _uniqueID;
		if( _parent != nullptr )
		{
			_parent->AddChild( *this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::OnGui( ecComponent& _sceneNode )
	{
		SceneNode& node = static_cast<SceneNode&>( _sceneNode );
		ImGui::Text( "name      : %s", node.name.c_str() );
		ImGui::Text( "scene     : %s", node.scene->GetName().c_str() );
		ImGui::Text( "handle    : %u", node.entityHandle );
		ImGui::Text( "unique id : %u", node.uniqueID );
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