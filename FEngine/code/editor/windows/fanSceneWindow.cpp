#include "fanGlobalIncludes.h"
#include "editor/windows/fanSceneWindow.h"

#include "editor/fanDragnDrop.h"
#include "renderer/fanRenderer.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanDirectionalLight.h"
#include "scene/components/fanRigidbody.h"
#include "scene/components/fanSphereShape.h"
#include "scene/actors/fanParticleSystem.h"
#include "scene/fanSceneInstantiate.h"
#include "scene/fanPrefab.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanMouse.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanRessourceManager.h"
#include "editor/fanModals.h"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow( Scene * _scene ) :
		EditorWindow("scene", ImGui::IconType::SCENE16 )
		, m_scene(_scene)
	{
		m_textBuffer[0] = '\0';
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::~SceneWindow() {}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::OnGui() {
		SCOPED_PROFILE( scene )

		ImGui::Icon( GetIconType(), {16,16}); ImGui::SameLine();
		ImGui::Text(m_scene->GetName().c_str());
		ImGui::Separator();

		Gameobject * gameobjectRightClicked = nullptr;
		R_DrawSceneTree( m_scene->GetRoot(), gameobjectRightClicked);
		m_expandSceneHierarchy = false;

		if (gameobjectRightClicked != nullptr) {
			ImGui::OpenPopup("scene_window_gameobject_rclicked");
			m_lastGameobjectRightClicked = gameobjectRightClicked;
		}

		// Popup set gameobject when right clic
		bool newGameobjectPopup = false;
		bool renameGameobjectPopup = false;
		bool exportToPrefabPopup = false;
		bool loadPrefabPopup = false;

		if (ImGui::BeginPopup("scene_window_gameobject_rclicked")) {

			// New gameobject 
			bool itemClicked = false;
			if (ImGui::BeginMenu("New gameobject")) {
				// Popup empty gameobject
				if ( ImGui::IsItemClicked() ) {
					itemClicked = true;
				}
				// Entities templates
				ImGui::Icon(ImGui::MESH_RENDERER16, {16,16}); ImGui::SameLine();
				if ( ImGui::MenuItem( "Model" ) ) {
					Gameobject *  newIntity = m_scene->CreateGameobject("new_model", m_lastGameobjectRightClicked );
					MeshRenderer * meshRenderer = newIntity->AddComponent<MeshRenderer>();
					meshRenderer->SetPath( GlobalValues::s_meshCube );
					Material * material =  newIntity->AddComponent<Material>();
					material->SetTexturePath( GlobalValues::s_textureWhite );
				}

				ImGui::Icon( ImGui::POINT_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if ( ImGui::MenuItem( "Point light" ) ) {
					Gameobject *  newIntity = m_scene->CreateGameobject( "new_point_light", m_lastGameobjectRightClicked );
					newIntity->AddComponent<PointLight>();
				}

				ImGui::Icon( ImGui::DIR_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if ( ImGui::MenuItem( "Dir light" ) ) {
					Gameobject *  newIntity = m_scene->CreateGameobject( "new_dir_light", m_lastGameobjectRightClicked );
					newIntity->AddComponent<DirectionalLight>();
				}

				ImGui::Icon( ImGui::SPHERE_SHAPE16, { 16,16 } ); ImGui::SameLine();
				if ( ImGui::MenuItem( "Sphere" ) ) {
					Gameobject *  newIntity = m_scene->CreateGameobject( "new_model", m_lastGameobjectRightClicked );
					MeshRenderer * meshRenderer = newIntity->AddComponent<MeshRenderer>();
					meshRenderer->SetPath( GlobalValues::s_meshSphere );
					newIntity->AddComponent<Rigidbody>();
					newIntity->AddComponent<SphereShape>();
				}

				ImGui::Icon( ImGui::PARTICLES16, { 16,16 } ); ImGui::SameLine();
				if ( ImGui::MenuItem( "FX" ) )
				{
					Gameobject *  newIntity = m_scene->CreateGameobject( "new_fx", m_lastGameobjectRightClicked );
					newIntity->AddComponent<ParticleSystem>();
				}

				ImGui::EndMenu();
			}

			if ( ImGui::IsItemClicked() ) {
				newGameobjectPopup = true;
			}

			if (ImGui::MenuItem("Import prefab")) {
				loadPrefabPopup = true;
			}

			ImGui::Separator();

			// rename
			if (ImGui::Selectable("Rename")) {
				renameGameobjectPopup = true;
			}

			// export to prefab
			if ( ImGui::Selectable( "Export to prefab" ) )
			{
				exportToPrefabPopup = true;
			}

			// delete
			ImGui::Separator();
			if (ImGui::Selectable("Delete")) {
				m_scene->DeleteGameobject(m_lastGameobjectRightClicked);
			}
			ImGui::EndPopup();
		}

		// new gameobject modal
		if (newGameobjectPopup) {
			ImGui::OpenPopup("New gameobject");
		} NewGameobjectModal();

		// rename modal
		if (renameGameobjectPopup) {
			ImGui::OpenPopup("Rename gameobject");
		} RenameGameobjectModal();

		// export to prefab modal
		if ( exportToPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "Export to prefab" );
		} ExportToPrefabModal();

		// load prefab popup
		if( loadPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "Load prefab" );
		}
		if ( ImGui::FanLoadFileModal( "Load prefab", GlobalValues::s_prefabExtensions, m_pathBuffer ) )
		{
			Prefab prefab;
			if ( prefab.LoadFromFile( m_pathBuffer.string() ) )
			{
				m_scene->CreateGameobject( prefab, m_lastGameobjectRightClicked );
			}
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::R_DrawSceneTree(Gameobject * _gameobjectDrawn, Gameobject* & _gameobjectRightClicked) {

		std::stringstream ss;
		ss << "##" << _gameobjectDrawn; // Unique id

		if (ImGui::IsWindowAppearing() || m_expandSceneHierarchy == true) {
			ImGui::SetNextTreeNodeOpen(true);
		}
		bool isOpen = ImGui::TreeNode(ss.str().c_str());

		// Gameobject dragndrop target empty selectable -> place dragged below
		Gameobject * gameobjectDrop1 = ImGui::FanBeginDragDropTargetGameobject();
		if ( gameobjectDrop1 && gameobjectDrop1 != _gameobjectDrawn )
		{
			gameobjectDrop1->InsertBelow( _gameobjectDrawn );
		}


		ImGui::SameLine();
		bool selected = ( _gameobjectDrawn == m_gameobjectSelected );

		// Draw gameobject empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _gameobjectDrawn->GetName() << "##" << _gameobjectDrawn; // Unique id
		if (ImGui::Selectable(ss2.str().c_str(), &selected)) {
			onSelectGameobject.Emmit( _gameobjectDrawn );
		}
		if (ImGui::IsItemClicked(1)) {
			_gameobjectRightClicked = _gameobjectDrawn;
		}

		// Gameobject dragndrop source = selectable -^
		ImGui::FanBeginDragDropSourceGameobject( _gameobjectDrawn  );

		// Gameobject dragndrop target gameobject name -> place as child
		Gameobject * gameobjectDrop = ImGui::FanBeginDragDropTargetGameobject();
		if ( gameobjectDrop )
		{
			gameobjectDrop->SetParent( _gameobjectDrawn );
		}

		if (isOpen) {
			const std::vector<Gameobject*>& childs = _gameobjectDrawn->GetChilds();
			for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
				Gameobject * child = childs[childIndex];
				R_DrawSceneTree(child, _gameobjectRightClicked);
			}

			ImGui::TreePop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::NewGameobjectModal()
	{
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		if (ImGui::BeginPopupModal("New gameobject"))
		{
			if (ImGui::IsWindowAppearing()) {
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if (ImGui::InputText("Name ", m_textBuffer, IM_ARRAYSIZE(m_textBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterPressed = true;
			}
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE, false)) {
				m_lastGameobjectRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Ok") || enterPressed)
			{
				if (std::string(m_textBuffer) != "") {
					//Create new gameobject 
					Gameobject* newGameobject = m_scene->CreateGameobject(m_textBuffer, m_lastGameobjectRightClicked);
					onSelectGameobject.Emmit( newGameobject );
					m_lastGameobjectRightClicked = nullptr;
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
		ImGui::SetNextWindowSize(ImVec2(200, 200));
		if (ImGui::BeginPopupModal("Rename gameobject"))
		{
			if (ImGui::IsWindowAppearing()) {
				strcpy_s(m_textBuffer, 32, m_lastGameobjectRightClicked->GetName().c_str());
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if (ImGui::InputText("New Name ", m_textBuffer, IM_ARRAYSIZE(m_textBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				enterPressed = true;
			}
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(GLFW_KEY_ESCAPE, false)) {
				m_lastGameobjectRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Ok") || ImGui::IsKeyPressed(GLFW_KEY_ENTER, false) || enterPressed)
			{
				if (std::string(m_textBuffer) != "") {
					m_lastGameobjectRightClicked->SetName(m_textBuffer);
					m_lastGameobjectRightClicked = nullptr;
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
		if ( ImGui::FanSaveFileModal( "Export to prefab", GlobalValues::s_prefabExtensions, m_pathBuffer ) )
		{
			Debug::Log() << "Exporting prefab to "  << m_pathBuffer.string() << Debug::Endl();

			std::ofstream outStream( m_pathBuffer.string() );
			if ( outStream.is_open() )
			{
				// Try to update the existing prefab if it exists
				Prefab * prefab = RessourceManager::Get().FindPrefab( m_pathBuffer.string() );
				if ( prefab != nullptr )
				{
					prefab->LoadFromGameobject(m_lastGameobjectRightClicked);
				}

				Json json;
				Json& prefabJson = json["prefab"];
				if ( m_lastGameobjectRightClicked->Save( prefabJson ) )
				{
					outStream << json;
				}
				else
				{
					Debug::Warning() << "Prefab export failed for " << m_lastGameobjectRightClicked->GetName() << Debug::Endl();
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