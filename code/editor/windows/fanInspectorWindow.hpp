#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	struct SceneNode;
	class Component;

	//================================================================================================================================
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
		void R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector< const Component*>& _components, const std::vector<std::filesystem::path>& _componentsPath );
		void NewComponentItem( const Component* _component );
	};
}