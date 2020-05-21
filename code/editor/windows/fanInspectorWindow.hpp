#pragma once

#include <set>
#include <filesystem>
#include "editor/windows/fanEditorWindow.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	struct SceneNode;
	struct EcsComponentInfo;

	//================================================================================================================================
	// displays a scene node and its components
	//================================================================================================================================
	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow();


		// Callbacks
		void OnSceneNodeSelected( SceneNode* _node ) { m_sceneNodeSelected = _node; }

	protected:
		void OnGui() override;

	private:
		SceneNode* m_sceneNodeSelected = nullptr;

		void NewComponentPopup();
		void R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector<EcsComponentInfo>& _components, const std::vector<std::filesystem::path>& _componentsPath );
		void NewComponentItem( const EcsComponentInfo& _info );
	};
}