#include "editor/windows/fanSceneWindow.hpp"
#include "game/imgui/fanDragnDrop.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanScene.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/imgui/fanModals.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "scene/fanPrefab.hpp"

#include "scene/ecs/components/fanSceneNode.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow() :
		EditorWindow( "scene", ImGui::IconType::SCENE16 )
	{
		m_textBuffer[0] = '\0';
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::~SceneWindow() {}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::OnGui()
	{
		SCOPED_PROFILE( scene );

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( m_scene->GetName().c_str() );
 		ImGui::SameLine();
 		ImGui::Text( m_scene->IsServer() ? "- server" : "- client" );
 		ImGui::Separator();

		// Draws all scene nodes
		SceneNode* nodeRightClicked = nullptr;
		R_DrawSceneTree( m_scene->GetRootNode(), nodeRightClicked );

 		m_expandSceneHierarchy = false;
 
		if( nodeRightClicked != nullptr )
		{
			ImGui::OpenPopup( "scene_window_node_rclicked" );
			m_lastSceneNodeRightClicked = nodeRightClicked;
 		}
 
		PopupRightClick();
// 
// 		// load prefab popup
// 		if( loadPrefabPopup )
// 		{
// 			m_pathBuffer = "content/prefab";
// 			ImGui::OpenPopup( "Load prefab" );
// 		}
// 		if( ImGui::FanLoadFileModal( "Load prefab", RenderGlobal::s_prefabExtensions, m_pathBuffer ) )
// 		{
// 			Prefab* prefab = Prefab::s_resourceManager.LoadPrefab( m_pathBuffer.string() );
// 			if( prefab != nullptr )
// 			{
// 				m_scene->CreateGameobject( *prefab, m_lastGameobjectRightClicked );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::PopupRightClick()
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
				// Popup empty gameobject
				if( ImGui::IsItemClicked() )
				{
					itemClicked = true;
				}
				// Entities templates
				ImGui::Icon( ImGui::MESH_RENDERER16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Model" ) )
				{
// 					Gameobject* newIntity = m_scene->CreateGameobject( "new_model", m_lastGameobjectRightClicked );
// 					MeshRenderer* meshRenderer = newIntity->AddComponent<MeshRenderer>();
// 					meshRenderer->SetPath( RenderGlobal::s_meshCube );
// 					Material* material = newIntity->AddComponent<Material>();
// 					material->SetTexturePath( RenderGlobal::s_textureWhite );
				}

				ImGui::Icon( ImGui::POINT_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Point light" ) )
				{
// 					Gameobject* newIntity = m_scene->CreateGameobject( "new_point_light", m_lastGameobjectRightClicked );
// 					newIntity->AddComponent<PointLight>();
				}

				ImGui::Icon( ImGui::DIR_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Dir light" ) )
				{
// 					Gameobject* newIntity = m_scene->CreateGameobject( "new_dir_light", m_lastGameobjectRightClicked );
// 					newIntity->AddComponent<DirectionalLight>();
				}

				ImGui::Icon( ImGui::SPHERE_SHAPE16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Sphere" ) )
				{
// 					Gameobject* newIntity = m_scene->CreateGameobject( "new_model", m_lastGameobjectRightClicked );
// 					MeshRenderer* meshRenderer = newIntity->AddComponent<MeshRenderer>();
// 					meshRenderer->SetPath( RenderGlobal::s_meshSphere );
// 					newIntity->AddComponent<Rigidbody>();
// 					newIntity->AddComponent<SphereShape>();
				}

				ImGui::Icon( ImGui::PARTICLES16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "FX" ) )
				{
// 					Gameobject* newIntity = m_scene->CreateGameobject( "new_fx", m_lastGameobjectRightClicked );
// 					newIntity->AddComponent<ParticleSystem>();
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
				m_scene->DeleteSceneNode( *m_lastSceneNodeRightClicked );
			}
			ImGui::EndPopup();
		}

		// new gameobject modal
		if( newNodePopup )
		{
			ImGui::OpenPopup( "new_scenenode" );
		} NewGameobjectModal();

		// rename modal
		if( renameNodePopup )
		{
			ImGui::OpenPopup( "rename_scenenode" );
		} RenameGameobjectModal();

		// export to prefab modal
		if( exportToPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "export_to_prefab" );
		} ExportToPrefabModal();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked )
	{

		std::stringstream ss;
		ss << "##" << &_node; // Unique id

		if( ImGui::IsWindowAppearing() || m_expandSceneHierarchy == true )
		{
			ImGui::SetNextItemOpen( true );
		}
		bool isOpen = ImGui::TreeNode( ss.str().c_str() );

		// Gameobject dragndrop target empty selectable -> place dragged below
		SceneNode* nodeDrop1 = ImGui::FanBeginDragDropTargetSceneNode();
		if( nodeDrop1 && nodeDrop1 != &_node )
		{
			nodeDrop1->InsertBelow( _node );
		}


		ImGui::SameLine();
		bool selected = ( &_node == m_sceneNodeSelected );

		// Draw gameobject empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _node.name << "##" << &_node; // Unique id
		if( ImGui::Selectable( ss2.str().c_str(), &selected ) )
		{
			onSelectSceneNode.Emmit( &_node );
		}
		if( ImGui::IsItemClicked( 1 ) )
		{
			_nodeRightClicked = &_node;
		}

		// SceneNode dragndrop source = selectable -^
		ImGui::FanBeginDragDropSourceSceneNode( _node );

		// SceneNode dragndrop target gameobject name -> place as child
		SceneNode* nodeDrop = ImGui::FanBeginDragDropTargetSceneNode();
		if( nodeDrop )
		{
			nodeDrop->SetParent( &_node );
		}

		if( isOpen )
		{
			for( int childIndex = 0; childIndex < _node.childs.size(); childIndex++ )
			{
				SceneNode* child = _node.childs[childIndex];
				R_DrawSceneTree( *child, _nodeRightClicked );
			}

			ImGui::TreePop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::NewGameobjectModal()
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
					SceneNode& newNode = m_scene->InstanciateSceneNode( m_textBuffer, m_lastSceneNodeRightClicked );
					onSelectSceneNode.Emmit( &newNode );
					m_lastSceneNodeRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
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

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::ExportToPrefabModal()
	{
		if( m_lastSceneNodeRightClicked == nullptr )
		{
			return;
		}

		if( ImGui::FanSaveFileModal( "export_to_prefab", RenderGlobal::s_prefabExtensions, m_pathBuffer ) )
		{
			Debug::Log() << "Exporting prefab to " << m_pathBuffer.string() << Debug::Endl();

			std::ofstream outStream( m_pathBuffer.string() );
			if( outStream.is_open() )
			{
				// Try to update the existing prefab if it exists
// 				Prefab* prefab = Prefab::s_resourceManager.FindPrefab( m_pathBuffer.string() );
// 				if( prefab != nullptr )
// 				{
// 					prefab->CreateFromGameobject( *m_lastSceneNodeRightClicked );
// 				}
// 
// 				Prefab newprefab;
// 				newprefab.CreateFromGameobject( *m_lastSceneNodeRightClicked );
// 				outStream << newprefab.GetJson();

				outStream.close();
			}
			else
			{
				Debug::Warning() << "Prefab export failed : " << m_pathBuffer.string() << Debug::Endl();
			}
		}
	}
}