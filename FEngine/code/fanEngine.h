#pragma once

#include "core/files/fanSerializedValues.h"

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

	//================================================================================================================================
	//================================================================================================================================	
	class Engine {
	public:

		struct EditorGrid {
			bool		isVisible;
			Color		color;
			int			linesCount;
			float		spacing;
		};

		Engine();
		~Engine();

		void Run();
		void Exit();

		inline static Engine & GetEngine() { return * ms_engine; }

		void SetMainCamera( Camera * _mainCamera );
		void SetSelectedEntity( Entity * _selectedentity) { m_selectedentity = _selectedentity;	}
		void Deselect() { m_selectedentity = nullptr; }
		
		void RegisterDirectionalLight	( DirectionalLight * _pointLight );
		void UnRegisterDirectionalLight	( DirectionalLight * _pointLight );
		void RegisterPointLight			( PointLight *		 _pointLight );
		void UnRegisterPointLight		( PointLight *		 _pointLight );
		void RegisterModel				( Model *			 _model );
		void UnRegisterModel			( Model *			 _model );
		
		Entity *	const		GetSelectedentity() const		{ return m_selectedentity;  }
		Camera *				GetEditorCamera() const			{ return m_editorCamera; }
		Camera *				GetMainCamera() const			{ return m_mainCamera; }
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

		// Main components
		Scene *	 m_scene;
		Entity * m_selectedentity;
		Camera * m_editorCamera;
		Camera * m_mainCamera;

		std::vector < DirectionalLight* >	m_directionalLights;
		std::vector < PointLight* >			m_pointLights;
		std::vector < Model* >				m_models;

		struct GizmoCacheData {
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};
		std::map< size_t, GizmoCacheData > m_gizmoCacheData;

		static Engine * ms_engine;
		bool m_applicationShouldExit;

		void ManageSelection();
		void UpdateRenderer();
		void OnSceneLoad(Scene * _scene);
		void DrawUI();
		void DrawEditorGrid() const;
		void DrawWireframe() const;
		void DrawNormals() const;
		void DrawAABB() const;
		void DrawHull() const;

	};
}
