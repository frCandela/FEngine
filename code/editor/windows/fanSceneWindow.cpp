#include "editor/windows/fanSceneWindow.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "engine/singletons/fanRenderResources.hpp"
#include "engine/fanDragnDrop.hpp"
#include "render/resources/fanTextureManager.hpp"
#include "engine/fanPrefabManager.hpp"
#include "engine/singletons/fanSceneResources.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/fanPrefab.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/components/fanRigidbody.hpp"
#include "engine/components/fanMotionState.hpp"
#include "engine/components/fanBoxShape.hpp"
#include "engine/components/fanParticleEmitter.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorSelection.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	SceneWindow::SceneWindow() : EditorWindow( "scene", ImGui::IconType::Scene16 )
	{
        mTextBuffer[0] = '\0';
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
		ImGui::Text( scene.mPath.c_str() );
 		ImGui::Separator();

		// Draws all scene nodes
		SceneNode* nodeRightClicked = nullptr;
		R_DrawSceneTree( scene.GetRootNode(), nodeRightClicked );

		mExpandSceneHierarchy = false;
 
		if( nodeRightClicked != nullptr )
		{
			ImGui::OpenPopup( "scene_window_node_rclicked" );
			mLastSceneNodeRightClicked = nodeRightClicked;
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
                fanAssert( mLastSceneNodeRightClicked != nullptr );
				Scene& scene = *mLastSceneNodeRightClicked->mScene;
				EcsWorld& world = *scene.mWorld;
				btVector3 origin = btVector3_Zero;
				const EcsEntity parentID = world.GetEntity( mLastSceneNodeRightClicked->mHandle );
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
				ImGui::Icon( ImGui::CubeShape16, { 16, 16 } ); ImGui::SameLine();
				
				// model 
				if( ImGui::MenuItem( "Model" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "model", mLastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.mHandle );
                    RenderResources& renderResources = _world.GetSingleton<RenderResources>();

					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );

					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entity );
					meshRenderer.mMesh = renderResources.mMeshManager->GetOrLoad( RenderGlobal::sMeshSphere );
					
					Material& material = world.AddComponent<Material>( entity );

					material.mTexture = renderResources.mTextureManager->Get( RenderGlobal::sTextureWhite );
					onSelectSceneNode.Emmit( &node );
				}

				// model with a rigidbody & shape
				ImGui::Icon( ImGui::Rigidbody16, { 16, 16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Physics model" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "physics_model", mLastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.mHandle );
                    RenderResources& renderResources = _world.GetSingleton<RenderResources>();

					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );

					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entity );
					meshRenderer.mMesh = renderResources.mMeshManager->GetOrLoad( RenderGlobal::sMeshCube );
					Material& material = world.AddComponent<Material>( entity );
					material.mTexture = renderResources.mTextureManager->Get( RenderGlobal::sTextureWhite );
					onSelectSceneNode.Emmit( &node );
					
					Rigidbody& rigidbody = world.AddComponent<Rigidbody>( entity );
					MotionState& motionState = world.AddComponent<MotionState>( entity );
					BoxShape& shape = world.AddComponent<BoxShape>( entity );
					rigidbody.SetMotionState( motionState.mMotionState );
					rigidbody.SetCollisionShape( shape.mBoxShape );
				}

				// point light
				ImGui::Icon( ImGui::PointLight16, { 16, 16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Point light" ) )

				{
					SceneNode& node = scene.CreateSceneNode( "point_light", mLastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.mHandle );
					
					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin );					
					world.AddComponent<PointLight>( entity );
					onSelectSceneNode.Emmit( &node );
				}

				// directional light
				ImGui::Icon( ImGui::DirLight16, { 16, 16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Dir light" ) )
				{
                    SceneNode& node = scene.CreateSceneNode( "directional_light",
															 mLastSceneNodeRightClicked );
                    const EcsEntity entity = world.GetEntity( node.mHandle );
					
					Transform& transform = world.AddComponent<Transform>( entity );
					transform.SetPosition( origin  );
					
					world.AddComponent<DirectionalLight>( entity );
					transform.SetRotationEuler( btVector3(30.f,10.f,0.f) );
					onSelectSceneNode.Emmit( &node );
				}

				// particle system
				ImGui::Icon( ImGui::Particles16, { 16, 16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "particle system" ) )
				{
					SceneNode& node = scene.CreateSceneNode( "particle_system", mLastSceneNodeRightClicked );
					const EcsEntity entity = world.GetEntity( node.mHandle );
					
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

			ImGui::Separator();

			// rename
			if( ImGui::Selectable( "Rename" ) )
			{
				renameNodePopup = true;
			}

			// delete
			if( ImGui::Selectable( "Delete" ) && mLastSceneNodeRightClicked != nullptr )
			{
				EcsWorld& world = *mLastSceneNodeRightClicked->mScene->mWorld;
				world.Kill( world.GetEntity( mLastSceneNodeRightClicked->mHandle ) );
			}

			ImGui::Separator();

            if( ImGui::MenuItem( "Import prefab" ) )
            {
                loadPrefabPopup = true;
            }

            // export to prefab
            if( ImGui::Selectable( "Export to prefab" ) )
            {
                exportToPrefabPopup = true;
            }

			ImGui::EndPopup();
		}

		// new entity modal
		if( newNodePopup )
		{
			ImGui::OpenPopup( "new_scenenode" );
		}
        NewSceneNodeModal( _world );

		// rename modal
		if( renameNodePopup )
		{
			ImGui::OpenPopup( "rename_scenenode" );
		}
        RenameSceneNodeModal();

		// export to prefab modal
		if( exportToPrefabPopup )
		{
            mPathBuffer = "content/prefab";
			ImGui::OpenPopup( "export_prefab" );
		}
		ExportPrefabModal( _world);

		// load prefab popup
		if( loadPrefabPopup )
		{
            mPathBuffer = "content/prefab";
			ImGui::OpenPopup( "import_prefab" );
		}
		ImportPrefabModal( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked )
	{
		EcsWorld& world = *_node.mScene->mWorld;
		Scene& scene = world.GetSingleton<Scene>();

		std::stringstream ss;
		ss << "##" << _node.mName;
		ImGui::PushID( _node.mHandle );

		if( ImGui::IsWindowAppearing() || mExpandSceneHierarchy == true )
		{
			ImGui::SetNextItemOpen( true );
		}
		bool isOpen = ImGui::TreeNode( ss.str().c_str() );

		// SceneNode dragndrop target empty selectable -> place dragged below
		ImGui::ComponentPayload payload = ImGui::FanBeginDragDropTargetComponent<SceneNode>( world );
		if( payload.IsValid() )
		{
            fanAssert( payload.mComponentType == SceneNode::Info::sType );
			SceneNode& nodeDrop1 = scene.mWorld->GetComponent<SceneNode>( scene.mWorld->GetEntity( payload.mHandle) );
			if( &nodeDrop1 != &_node )
			{
				nodeDrop1.InsertBelow( _node );
			}
		}

		ImGui::SameLine();
		const EcsHandle handleSelected = world.GetSingleton<EditorSelection>().mSelectedNodeHandle;
		bool selected = ( _node.mHandle == handleSelected );

		// Draw scene node empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _node.mName; // @ todo create some sort of unique id not based on name
		if( ImGui::Selectable( ss2.str().c_str(), &selected ) )
		{
			onSelectSceneNode.Emmit( &_node );
		}
		if( ImGui::IsItemClicked( 1 ) )
		{
			_nodeRightClicked = &_node;
		}

		// SceneNode dragndrop source = selectable -^
		ImGui::FanBeginDragDropSourceComponent( world, _node.mHandle, SceneNode::Info::sType );

		// SceneNode dragndrop target scene node name -> place as child
		ImGui::ComponentPayload payload2 = ImGui::FanBeginDragDropTargetComponent<SceneNode>( world );
		if( payload2.IsValid() )
		{
            fanAssert( payload2.mComponentType == SceneNode::Info::sType );
			SceneNode& nodeDrop2 = world.GetComponent<SceneNode>( world.GetEntity( payload2.mHandle ) );
			nodeDrop2.SetParent( &_node );
		}

		ImGui::PopID();

		if( isOpen )
		{
			for( int childIndex = 0; childIndex < (int)_node.mChilds.size(); childIndex++ )
			{
                SceneNode& child = world.GetComponent<SceneNode>( world.GetEntity( _node.mChilds[childIndex] ) );
                R_DrawSceneTree( child, _nodeRightClicked );
			}

			ImGui::TreePop();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::NewSceneNodeModal( EcsWorld& _world )
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "new_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
            if( ImGui::InputText( "Name ",
                                  mTextBuffer,
                                  IM_ARRAYSIZE( mTextBuffer ),
                                  ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				mLastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || enterPressed )
			{
				if( std::string( mTextBuffer ) != "" )
				{
					//Create new scene node 
					Scene& scene = _world.GetSingleton<Scene>();
					SceneNode& newNode = scene.CreateSceneNode( mTextBuffer, mLastSceneNodeRightClicked );
					onSelectSceneNode.Emmit( &newNode );
					mLastSceneNodeRightClicked = nullptr;
                    mTextBuffer[0] = '\0';
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::RenameSceneNodeModal()
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "rename_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				strcpy_s( mTextBuffer, 32, mLastSceneNodeRightClicked->mName.c_str() );
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
            if( ImGui::InputText( "New Name ",
                                  mTextBuffer,
                                  IM_ARRAYSIZE( mTextBuffer ),
                                  ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				mLastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || ImGui::IsKeyPressed( GLFW_KEY_ENTER, false ) || enterPressed )
			{
				if( std::string( mTextBuffer ) != "" )
				{
					mLastSceneNodeRightClicked->mName = mTextBuffer;
					mLastSceneNodeRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::ImportPrefabModal( EcsWorld& _world )
	{
		if( ImGui::FanLoadFileModal( "import_prefab", RenderGlobal::sPrefabExtensions, mPathBuffer ) )
		{
            SceneResources& sceneResources = _world.GetSingleton<SceneResources>();
			Prefab* prefab = sceneResources.mPrefabManager->Load( mPathBuffer.string() );
			if( prefab != nullptr )
			{
                prefab->Instantiate( *mLastSceneNodeRightClicked );
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SceneWindow::ExportPrefabModal( EcsWorld& _world )
	{
		if( mLastSceneNodeRightClicked == nullptr )
		{
			return;
		}

		if( ImGui::FanSaveFileModal( "export_prefab", RenderGlobal::sPrefabExtensions, mPathBuffer ) )
		{
			Debug::Log() << "Exporting prefab to " << mPathBuffer.string() << Debug::Endl();

			std::ofstream outStream( mPathBuffer.string() );
			if( outStream.is_open() )
			{
				// Try to update the existing prefab if it exists
                SceneResources& sceneResources = _world.GetSingleton<SceneResources>();
				Prefab* prefab = sceneResources.mPrefabManager->Get( mPathBuffer.string() );
				if( prefab != nullptr )
				{
					prefab->CreateFromSceneNode( *mLastSceneNodeRightClicked );
					outStream << prefab->mJson;
				}
				else
				{
					Prefab newprefab;
					newprefab.CreateFromSceneNode( *mLastSceneNodeRightClicked );
					outStream << newprefab.mJson;
				}

				
				outStream.close();
			}
			else
			{
				Debug::Warning() << "Prefab export failed : " << mPathBuffer.string() << Debug::Endl();
			}
		}
	}
}