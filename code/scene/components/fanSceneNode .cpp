#include "scene/components/fanSceneNode.hpp"

#include <stack>
#include <set>
#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanScene.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon  = ImGui::IconType::GAMEOBJECT16;
		_info.group = EngineGroups::Scene;
		_info.onGui = &SceneNode::OnGui;
		_info.destroy = &SceneNode::Destroy;
		_info.editorPath = "/";
		_info.name = "scene node";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		node.handle = 0;
		node.flags = 0;
		node.name = "";
		node.scene = nullptr;
		node.parentHandle = 0;
		node.childs.clear();
	}

	//================================================================================================================================
	// disconnects all the node's child hierarchy tree and kills it
	//================================================================================================================================
	void SceneNode::Destroy( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		Scene& scene = _world.GetSingleton<Scene>();
		scene.onDeleteSceneNode.Emmit( &node );
		scene.nodes.erase( node.handle );

		// removes from parent
		if( node.parentHandle != 0 )
		{
			node.GetParent().RemoveChild( node );
		}

		if( !node.childs.empty() )
		{
			// stacks of initial nodes
			std::stack< EcsHandle > nodesstack;
			for ( EcsHandle child : node.childs )
			{
				nodesstack.push( child );
			}			

			// find all child nodes and kills them
			while( !nodesstack.empty() )
			{			
				const EcsEntity childEntity = _world.GetEntity( nodesstack.top() );
				SceneNode& childNode = _world.GetComponent<SceneNode>( childEntity );
				nodesstack.pop();
				
				for( int childIndex = 0; childIndex < (int)childNode.childs.size(); childIndex++ )
				{
					nodesstack.push( childNode.childs[childIndex] );
				}

				childNode.parentHandle = 0;
				childNode.childs.clear();
				_world.Kill( childEntity );
			}	
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::Build( const std::string& _name, Scene& _scene, const EcsHandle _handle, SceneNode* const _parent )
	{
		scene = &_scene;
		handle = _handle;
		name = _name;
		if( _parent != nullptr )
		{
			_parent->AddChild( *this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		SceneNode& node = static_cast<SceneNode&>( _component );
		EcsWorld& world = * node.scene->world;
		EcsEntity entity = world.GetEntity( node.handle );

		ImGui::Text( "name      : %s", node.name.c_str() );
		ImGui::Text( "scene     : %s", node.scene->path.empty()	? "<no path>" : node.scene->path.c_str() );
		ImGui::Text( "handle    : %u", node.handle );
		ImGui::Text( "entity id : %u", entity );
	}

	//================================================================================================================================
	// Goes up the parents of _node to find this ancestor
	//================================================================================================================================bool IsAncestorOf( const SceneNode* _node ) const;
	bool SceneNode::IsAncestorOf( const SceneNode& _node ) const
	{
		const SceneNode* node = &_node;
		while( node->parentHandle != 0 )
		{
			if( node->parentHandle == handle )
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

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::RemoveChild( const SceneNode& _child )
	{
		for( int childIndex = 0; childIndex < (int)childs.size(); childIndex++ )
		{
			EcsHandle childHandle = childs[childIndex];
			if( childHandle == _child.handle )
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
		for( int childIndex = 0; childIndex < (int)childs.size(); childIndex++ )
		{
			if( childs[childIndex] == _child.handle )
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
			Debug::Warning( "Cannot parent an object to one of its children" );
			return;
		}

		if( _child.parentHandle == handle )
		{
			Debug::Warning() << _child.name << " is already a child of " << name << Debug::Endl();
			return;
		}

		if( ! HasChild( _child ) )
		{
			if( _child.parentHandle != 0 )
			{
				_child.GetParent().RemoveChild( _child );
			}
			childs.push_back( _child.handle );
			_child.parentHandle = handle;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::SetParent( SceneNode* _parent )
	{
		if( _parent == nullptr )
		{
			Debug::Warning( "Root cannot have a brother :'(" );
			return;
		}
		_parent->AddChild( *this );
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneNode& SceneNode::GetParent() const
	{
		assert( parentHandle != 0 );
		return scene->world->GetComponent<SceneNode>( scene->world->GetEntity( parentHandle ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneNode::InsertBelow( SceneNode& _brother )
	{
		if( IsAncestorOf( _brother ) )
		{
			Debug::Warning( "Cannot parent an object to one of its children" );
			return;
		}
		if( _brother.parentHandle == 0 )
		{
			Debug::Warning( "Root cannot have a brother :'(" );
			return;
		}

		GetParent().RemoveChild( *this );

		SceneNode& brotherParent = _brother.GetParent();
		for( int childIndex = 0; childIndex < (int)brotherParent.childs.size(); childIndex++ )
		{
			if( brotherParent.childs[childIndex] == _brother.handle )
			{
				brotherParent.childs.insert( brotherParent.childs.begin() + childIndex + 1, handle );
				parentHandle = _brother.parentHandle;
			}
		}
	}
	//================================================================================================================================
	// fills the _list with _root and all its descendants  
	//================================================================================================================================
	void SceneNode::GetDescendantsOf( SceneNode& _root, std::vector<SceneNode*>& _outList )
	{
		EcsWorld& world = *_root.scene->world;

		_outList.clear();
		std::stack<EcsHandle> stack;
		stack.push( _root.handle );
		while( !stack.empty() )
		{
			SceneNode& node =  world.GetComponent<SceneNode>( world.GetEntity( stack.top() ) );
			stack.pop();
			_outList.push_back( &node );
			for( int i = 0; i < (int)node.childs.size(); i++ )
			{
				stack.push( node.childs[i] );
			}
		}
	}
}