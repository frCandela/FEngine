#include "scene/components/fanSceneNode.hpp"

#include <stack>
#include <set>
#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon  = ImGui::IconType::GAMEOBJECT16;
		_info.onGui = &SceneNode::OnGui;
		_info.init = &SceneNode::Init;
		_info.destroy = &SceneNode::Destroy;
		_info.editorPath = "/";
		_info.name = "scene node";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Init( EcsWorld& _world, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		node.handle = 0;
		node.name = "";
		node.scene = nullptr;
		node.parent = nullptr;
		node.childs.clear();
	}

	//================================================================================================================================
	// disconnects all the node's child hierarchy tree and kills it
	//================================================================================================================================
	void SceneNode::Destroy( EcsWorld& _world, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		Scene& scene = _world.GetSingleton<Scene>();
		scene.onDeleteSceneNode.Emmit( &node );
		scene.nodes.erase( node.uniqueID );

		// removes from parent
		if( node.parent != nullptr )
		{
			node.parent->RemoveChild( node );
		}

		if( !node.childs.empty() )
		{
			// stacks of initial nodes
			std::stack<SceneNode* > nodesstack;
			for ( SceneNode* child : node.childs )
			{
				nodesstack.push( child );
			}			

			// find all child nodes
			std::set<SceneNode* > nodesToDelete;
			while( !nodesstack.empty() )
			{
				SceneNode& node = *nodesstack.top();
				nodesstack.pop();
				nodesToDelete.insert( &node );
				for( int childIndex = 0; childIndex < node.childs.size(); childIndex++ )
				{
					nodesstack.push( node.childs[childIndex] );
				}
			}

			// delete node & childs
			for( SceneNode* node : nodesToDelete )
			{
				EcsEntity entity = _world.GetEntity( node->handle );
				_world.Kill( entity );
				node->parent = nullptr;
				node->childs.clear();
			}		
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Build( const std::string& _name, Scene& _scene, const EcsHandle _handle, const uint32_t _uniqueID, SceneNode* const _parent )
	{
		name = _name;
		scene = &_scene;
		handle = _handle;
		uniqueID = _uniqueID;
		if( _parent != nullptr )
		{
			_parent->AddChild( *this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		EcsWorld& world = * node.scene->world;
		EcsEntity entity = world.GetEntity( node.handle );

		ImGui::Text( "name      : %s", node.name.c_str() );
		ImGui::Text( "scene     : %s", node.scene->path.empty()	? "<null>" : node.scene->path.c_str() );
		ImGui::Text( "handle    : %u", node.handle );
		ImGui::Text( "entity id : %u", entity );
		ImGui::Text( "node   id : %u", node.uniqueID );
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
	//================================================================================================================================
	// fills the _list with _root and all its descendants  
	//================================================================================================================================
	void SceneNode::GetDescendantsOf( SceneNode& _root, std::vector<SceneNode*>& _outList )
	{
		_outList.clear();
		std::stack<SceneNode*> stack;
		stack.push( &_root );
		while( !stack.empty() )
		{
			SceneNode& node = *stack.top();
			stack.pop();
			_outList.push_back( &node );
			for( int i = 0; i < node.childs.size(); i++ )
			{
				stack.push( node.childs[i] );
			}
		}
	}
}