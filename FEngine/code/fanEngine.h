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
	class EcsWindow;
	class ProfilerWindow;
	class Renderer;
	class Gameobject;
	class Scene;
	class Actor;
	class Camera;	
	class DirectionalLight;
	class PointLight;
	class MeshRenderer;
	class Material;
	class Camera;
	class FPSCamera;
	class EcsManager;
	class PhysicsManager;
	class Texture;
	class Mesh;

	template< typename _RessourceType, typename _IDType > class RessourcePtr;
	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t >;
	using TexturePtr  = RessourcePtr<Texture, std::string >;
	using MeshPtr  = RessourcePtr<Mesh, std::string >;

	//================================================================================================================================
	//================================================================================================================================	
	class Engine {
	public:
		static Signal<Gameobject*> onGameobjectSelected;
		static Signal<Camera*> onSetCamera;

		Engine();
		~Engine();

		void Run();
		void Exit();

		void SetMainCamera( Camera * _mainCamera );
		void SetSelectedGameobject( Gameobject * _selectedGgameobject);
		void Deselect();

		void RegisterDirectionalLight	( DirectionalLight * _pointLight );
		void UnRegisterDirectionalLight	( DirectionalLight * _pointLight );
		void RegisterPointLight			( PointLight *		 _pointLight );
		void UnRegisterPointLight		( PointLight *		 _pointLight );
		void RegisterMeshRenderer		( MeshRenderer *	 _meshRenderer );
		void UnRegisterMeshRenderer		( MeshRenderer *	 _meshRenderer );
		
		Gameobject *	const	GetSelectedGameobject() const	{ return m_selectedGameobject;  }
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
		EcsWindow *			m_ecsWindow;
		ProfilerWindow*		m_profilerWindow;
		EditorGrid m_editorGrid;

		// Main objects
		Renderer *			m_renderer;
		Scene *				m_scene;
		EcsManager *		m_ecsManager;
		PhysicsManager *	m_physicsManager;
		Gameobject *		m_selectedGameobject;
		Camera *			m_editorCamera = nullptr;
		FPSCamera *			m_editorCameraController = nullptr;
		Camera *			m_mainCamera = nullptr;

		std::vector < DirectionalLight* >	m_directionalLights;
		std::vector < PointLight* >			m_pointLights;
		std::vector < MeshRenderer* >		m_meshRenderers;

		struct GizmoCacheData {
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};
		std::map< size_t, GizmoCacheData > m_gizmoCacheData;

		bool m_applicationShouldExit;

		void DeleteSelection();
		void ManageSelection();
		void UpdateRenderer();
		void SwitchPlayPause();

		void OnSceneLoad(Scene * _scene);
		void OnMaterialSetTexture( Material * _material, std::string _path );
		void OnResolveTexturePtr( TexturePtr * _ptr );
		void OnResolveMeshPtr( MeshPtr * _ptr );
		
		void OnGameobjectDeleted( Gameobject * _gameobject );
		void OnSetGameobjectPtrFromSelection( GameobjectPtr * _ptr );

		void DrawEditorGrid() const;
		void DrawWireframe() const;
		void DrawNormals() const;
		void DrawAABB() const;
		void DrawHull() const;

	};
}
