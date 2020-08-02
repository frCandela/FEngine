#include "editor/windows/fanSceneWindow.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "scene/singletons/fanRenderResources.hpp"
#include "scene/fanDragnDrop.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorSelection.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	SceneWindow::SceneWindow() : EditorWindow( "scene", ImGui::IconType::SCENE16 )
	{
		m_textBuffer[0] = '\0';
	}

	//========================================================================================================
	//========================================================================================================
	SceneWindow::~SceneWindow() {}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::OnGui( EcsWorld& _world )
	{
		//SCOPED_PROFILE( scene );

		Scene& scene = _world.GetSingleton<Scene>();

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( scene.path.c_str() );
 		ImGui::Separator();

		// Draws all scene nodes
		SceneNode* nodeRightClicked = nullptr;
		R_DrawSceneTree( scene.GetRootNode(), nodeRightClicked );

 		m_expandSceneHierarchy = false;
 
		if( nodeRightClicked != nullptr )
		{
			ImGui::OpenPopup( "scene_window_node_rclicked" );
			m_lastSceneNodeRightClicked = nodeRightClicked;
 		}
 
		PopupRightClick( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::PopupRightClick( EcsWorld& _world )
	{
		// Popup set gameobject when right click
		bool newNodePopup = false;
		bool renameNodePopup = false;
		bool exportToPrefabPopup = false;
		bool loadPrefabPopup = false;

		if( ImGui::BeginPopup( "scene_window_node_rclicked" ) )
		{
			// New scene node 
			bool itemClicked = false;
			if( ImGui::BeginMenu( "New node" ) )
			{
				assert( m_lastSceneNodeRightClicked != nullptr );
				Scene& scene = *m_lastSceneNodeRightClicked->scene;
				EcsWorld& world = *scene.world;				
				btVector3 origin = btVector3::Zero();
				const EcsEntity parentID = world.GetEntity( m_lastSceneNodeRightClicked->handle );
				if( world.HasComponent<Transform>( parentID ) )
				{
					origin = world.GetComponent<Transform>( parentID ).GetPosition();
				}

				// Popup empty gameobject
				if( ImGui::IsItemClicked() )
				{
					itemClicked = true;
				}

				// Entities templates
				ImGui::Icon( ImGui::CUBE_SHAPE16, { 16,16 } ); ImGui::SameLine();
				
				// model 
				if( ImGui::MenuItem( "Model" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "model", m_lastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.handle );
                    RenderResources& renderResources = _world.GetSingleton<RenderResources>();

					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );

					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entity );
					meshRenderer.mesh = renderResources.mMeshManager->GetOrLoad( RenderGlobal::sMeshSphere );
					
					Material& material = world.AddComponent<Material>( entity );

					material.texture = renderResources.mTextureManager->Get( RenderGlobal::sTextureWhite );
					onSelectSceneNode.Emmit( &node );
				}

				// model with a rigidbody & shape
				ImGui::Icon( ImGui::RIGIDBODY16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Physics model" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "physics_model", m_lastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.handle );
                    RenderResources& renderResources = _world.GetSingleton<RenderResources>();

					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );

					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entity );
					meshRenderer.mesh = renderResources.mMeshManager->GetOrLoad( RenderGlobal::sMeshCube );
					Material& material = world.AddComponent<Material>( entity );
					material.texture = renderResources.mTextureManager->Get( RenderGlobal::sTextureWhite );
					onSelectSceneNode.Emmit( &node );
					
					Rigidbody& rigidbody = world.AddComponent<Rigidbody>( entity );
					MotionState& motionState = world.AddComponent<MotionState>( entity );
					BoxShape& shape = world.AddComponent<BoxShape>( entity );
					rigidbody.SetMotionState( motionState.motionState );
					rigidbody.SetCollisionShape( shape.boxShape );
				}

				// point light
				ImGui::Icon( ImGui::POINT_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Point light" ) )

				{
					SceneNode& node = scene.CreateSceneNode( "point_light", m_lastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.handle );
					
					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );					
					world.AddComponent<PointLight>( entity );
					onSelectSceneNode.Emmit( &node );
				}

				// directional light
				ImGui::Icon( ImGui::DIR_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Dir light" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "directional_light", m_lastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.handle );
					
					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin  );
					
					world.AddComponent<DirectionalLight>( entity );
					transform.SetRotationEuler( btVector3(30.f,10.f,0.f) );
					onSelectSceneNode.Emmit( &node );
				}

				// particle system
				ImGui::Icon( ImGui::PARTICLES16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "particle system" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "particle_system", m_lastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.handle );
					
					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );

					world.AddComponent<ParticleEmitter>( entity );
					transform.SetPosition( origin );
				}

				ImGui::EndMenu();
			}

			if( ImGui::IsItemClicked() )
			{
				newNodePopup = true;
			}

			if( ImGui::MenuItem( "Import prefab" ) )
			{
				loadPrefabPopup = true;
			}

			ImGui::Separator();

			// rename
			if( ImGui::Selectable( "Rename" ) )
			{
				renameNodePopup = true;
			}

			// export to prefab
			if( ImGui::Selectable( "Export to prefab" ) )
			{
				exportToPrefabPopup = true;
			}

			// delete
			ImGui::Separator();
			if( ImGui::Selectable( "Delete" ) && m_lastSceneNodeRightClicked != nullptr )
			{
				EcsWorld& world = *m_lastSceneNodeRightClicked->scene->world;
				world.Kill( world.GetEntity( m_lastSceneNodeRightClicked->handle ) );
			}
			ImGui::EndPopup();
		}

		// new entity modal
		if( newNodePopup )
		{
			ImGui::OpenPopup( "new_scenenode" );
		} NewGameobjectModal( _world );

		// rename modal
		if( renameNodePopup )
		{
			ImGui::OpenPopup( "rename_scenenode" );
		} RenameGameobjectModal();

		// export to prefab modal
		if( exportToPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "export_prefab" );
		} ExportPrefabModal();

		// load prefab popup
		if( loadPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "import_prefab" );
		} ImportPrefabModal(); 		
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked )
	{
		EcsWorld& world = *_node.scene->world;
		Scene& scene = world.GetSingleton<Scene>();

		std::stringstream ss;
		ss << "##" << _node.name; // @ todo create some sort of unique id not based on name

		if( ImGui::IsWindowAppearing() || m_expandSceneHierarchy == true )
		{
			ImGui::SetNextItemOpen( true );
		}
		bool isOpen = ImGui::TreeNode( ss.str().c_str() );

		// SceneNode dragndrop target empty selectable -> place dragged below
		ImGui::ComponentPayload payload = ImGui::FanBeginDragDropTargetComponent<SceneNode>( world );
		if( payload.handle != 0 )
		{
			assert( payload.type == SceneNode::Info::s_type );
			SceneNode& nodeDrop1 = scene.world->GetComponent<SceneNode>( scene.world->GetEntity( payload.handle) );
			if( &nodeDrop1 != &_node )
			{
				nodeDrop1.InsertBelow( _node );
			}
		}

		ImGui::SameLine();
		const EcsHandle handleSelected = world.GetSingleton<EditorSelection>().m_selectedNodeHandle;
		bool selected = ( _node.handle == handleSelected );

		// Draw scene node empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _node.name; // @ todo create some sort of unique id not based on name
		if( ImGui::Selectable( ss2.str().c_str(), &selected ) )
		{
			onSelectSceneNode.Emmit( &_node );
		}
		if( ImGui::IsItemClicked( 1 ) )
		{
			_nodeRightClicked = &_node;
		}

		// SceneNode dragndrop source = selectable -^
		ImGui::FanBeginDragDropSourceComponent( world, _node.handle, SceneNode::Info::s_type );

		// SceneNode dragndrop target scene node name -> place as child
		ImGui::ComponentPayload payload2 = ImGui::FanBeginDragDropTargetComponent<SceneNode>( world );
		if( payload2.handle != 0 )
		{
			assert( payload2.type == SceneNode::Info::s_type );
			SceneNode& nodeDrop2 = world.GetComponent<SceneNode>( world.GetEntity( payload2.handle ) );
			nodeDrop2.SetParent( &_node );
		}

		if( isOpen )
		{
			for( int childIndex = 0; childIndex < (int)_node.childs.size(); childIndex++ )
			{
				SceneNode& child = world.GetComponent<SceneNode>( world.GetEntity( _node.childs[childIndex] ) );
				R_DrawSceneTree( child, _nodeRightClicked );
			}

			ImGui::TreePop();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::NewGameobjectModal( EcsWorld& _world )
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "new_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if( ImGui::InputText( "Name ", m_textBuffer, IM_ARRAYSIZE( m_textBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				m_lastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || enterPressed )
			{
				if( std::string( m_textBuffer ) != "" )
				{
					//Create new scene node 
					Scene& scene = _world.GetSingleton<Scene>();
					SceneNode& newNode = scene.CreateSceneNode( m_textBuffer, m_lastSceneNodeRightClicked );
					onSelectSceneNode.Emmit( &newNode );
					m_lastSceneNodeRightClicked = nullptr;
					m_textBuffer[0] = '\0';
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::RenameGameobjectModal()
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "rename_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				strcpy_s( m_textBuffer, 32, m_lastSceneNodeRightClicked->name.c_str() );
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if( ImGui::InputText( "New Name ", m_textBuffer, IM_ARRAYSIZE( m_textBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				m_lastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || ImGui::IsKeyPressed( GLFW_KEY_ENTER, false ) || enterPressed )
			{
				if( std::string( m_textBuffer ) != "" )
				{
					m_lastSceneNodeRightClicked->name = m_textBuffer;
					m_lastSceneNodeRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::ImportPrefabModal()
	{
		if( ImGui::FanLoadFileModal( "import_prefab", RenderGlobal::sPrefabExtensions, m_pathBuffer ) )
		{
			Prefab* prefab = Prefab::s_resourceManager.LoadPrefab( m_pathBuffer.string() );
			if( prefab != nullptr )
			{
				prefab->Instanciate( *m_lastSceneNodeRightClicked );
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::ExportPrefabModal()
	{
		if( m_lastSceneNodeRightClicked == nullptr )
		{
			return;
		}

		if( ImGui::FanSaveFileModal( "export_prefab", RenderGlobal::sPrefabExtensions, m_pathBuffer ) )
		{
			Debug::Log() << "Exporting prefab to " << m_pathBuffer.string() << Debug::Endl();

			std::ofstream outStream( m_pathBuffer.string() );
			if( outStream.is_open() )
			{
				// Try to update the existing prefab if it exists
				Prefab* prefab = Prefab::s_resourceManager.FindPrefab( m_pathBuffer.string() );
				if( prefab != nullptr )
				{
					prefab->CreateFromSceneNode( *m_lastSceneNodeRightClicked );
					outStream << prefab->GetJson();
				}
				else
				{
					Prefab newprefab;
					newprefab.CreateFromSceneNode( *m_lastSceneNodeRightClicked );
					outStream << newprefab.GetJson();
				}

				
				outStream.close();
			}
			else
			{
				Debug::Warning() << "Prefab export failed : " << m_pathBuffer.string() << Debug::Endl();
			}
		}
	}
}