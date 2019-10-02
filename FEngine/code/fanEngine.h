#pragma once

#include "core/files/fanSerializedValues.h"
#include "editor/fanEditorGrid.h"

namespace fan {
	class MainMenuBar;
	class RenderWindow;
	class SceneWindow;
	class InspectorWindow;
	class PreferencesWindow;
	class ConsoleWindow;
	class EditorWindow;
	class Renderer;
	class Scene;
	class Entity;
	class Actor;
	class Camera;	
	class DirectionalLight;
	class PointLight;
	class Model;
	class Material;
	class Camera;

	//================================================================================================================================
	//================================================================================================================================	
	class Engine {
	public:
		static Signal<Entity*> onEntitySelected;
		static Signal<Camera*> onSetCamera;

		Engine();
		~Engine();

		void Run();
		void Exit();

		void SetMainCamera( Camera * _mainCamera );
		void SetSelectedEntity( Entity * _selectedentity);
		void Deselect();
		
		void RegisterDirectionalLight	( DirectionalLight * _pointLight );
		void UnRegisterDirectionalLight	( DirectionalLight * _pointLight );
		void RegisterPointLight			( PointLight *		 _pointLight );
		void UnRegisterPointLight		( PointLight *		 _pointLight );
		void RegisterModel				( Model *			 _model );
		void UnRegisterModel			( Model *			 _model );
		
		Entity *	const		GetSelectedentity() const		{ return m_selectedentity;  }
		Camera *				GetEditorCamera() const			{ return m_editorCamera; }
		Camera *				GetMainCamera() const			{ return m_mainCamera; }
		Renderer &				GetRenderer() const				{ return * m_renderer; }
		MainMenuBar  &			GetMainMenuBar() const			{ return * m_mainMenuBar; }
		RenderWindow &			GetRenderWindow() const			{ return * m_renderWindow; }
		SceneWindow  &			GetSceneWindow() const			{ return * m_sceneWindow; }
		InspectorWindow  &		GetInspectorWindow() const		{ return * m_inspectorWindow; }
		PreferencesWindow  &	GetPreferencesWindow() const	{ return * m_preferencesWindow; }
		ConsoleWindow  &		GetConsoleWindow() const		{ return * m_consoleWindow; }
		Scene &					GetScene() const				{ return * m_scene; }

		EditorGrid GetEditorGrid() const { return m_editorGrid;  }
		void SetEditorGrid( const EditorGrid _editorGrid) { m_editorGrid =_editorGrid; }
		bool DrawMoveGizmo(const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition);

	private:

		// UI elements
		MainMenuBar *		m_mainMenuBar;

		RenderWindow *		m_renderWindow;
		SceneWindow *		m_sceneWindow;
		InspectorWindow *	m_inspectorWindow;
		PreferencesWindow *	m_preferencesWindow;
		ConsoleWindow *		m_consoleWindow;
		std::vector< EditorWindow * > m_editorWindows;

		EditorGrid m_editorGrid;

		// Main objects
		Renderer * m_renderer;
		Scene *	 m_scene;
		Entity * m_selectedentity;
		Camera * m_editorCamera = nullptr;
		Camera * m_mainCamera = nullptr;

		std::vector < DirectionalLight* >	m_directionalLights;
		std::vector < PointLight* >			m_pointLights;
		std::vector < Model* >				m_models;

		struct GizmoCacheData {
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};
		std::map< size_t, GizmoCacheData > m_gizmoCacheData;

		bool m_applicationShouldExit;

		void ManageKeyShortcuts();
		void ManageSelection();
		void UpdateRenderer();
		void OnSceneLoad(Scene * _scene);
		void OnMaterialSetTexture( Material * _material, std::string _path );
		void OnModelSetPath( Model * _model, std::string _path );
		void OnEntityDeleted( Entity * _entity );

		void DrawUI();
		void DrawEditorGrid() const;
		void DrawWireframe() const;
		void DrawNormals() const;
		void DrawAABB() const;
		void DrawHull() const;

	};
}
