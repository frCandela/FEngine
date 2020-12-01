#pragma once

#include "core/memory/fanSerializedValues.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "engine/project/fanIProjectContainer.hpp"

namespace fan
{
	class ProjectViewWindow;
	class MainMenuBar;
	class Window;
	struct Scene;

	//========================================================================================================
	// base class that contains everything
	// contains a project, a renderer, editions windows/ui and references on EcsWorld
	// here can be multiple projects run at the same time ( for client and server to run in the same process )
	//========================================================================================================
	class EditorProjectContainer : public IProjectContainer
	{
	public:
		EditorProjectContainer( LaunchSettings& _settings, const std::vector<IProject*>& _projects );
		~EditorProjectContainer() override;

		void Run();
		void Step();
	private:
		std::vector<IProject*>  mProjects;
		int                     mCurrentProject = 0;
        bool                    mShowUi         = true;
        MainMenuBar      *      mMainMenuBar;
        ProjectViewWindow*      mProjectViewWindow;

		IProject& GetCurrentProject() { return *mProjects[ mCurrentProject ]; }
        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
		static void UseEditorCamera( EcsWorld& _world );

		void OnCycleCurrentProject();
		void OnSwitchPlayStop();
		void OnStart();
		void OnStop() { Stop( GetCurrentProject() ); }
		void OnPause() { Pause( GetCurrentProject() ); }
		void OnResume() { Resume( GetCurrentProject() ); }
		void OnStep();
		void OnToogleCamera();
		void OnOpen();
		void OnReload();
		void OnSave();
		void OnCopy();
		void OnPaste();
        void OnDeleteSelection();
        void OnToogleTransformLock();
		void OnSelect( const int _index );

		static void Start( IProject& _project );
		static void Stop( IProject& _project );
		static void Pause( IProject& _project );
		static void Resume( IProject& _project );

		void OnSceneLoad( Scene& _scene );
		void OnToogleShowUI() { mShowUi = !mShowUi; }
	};
}
