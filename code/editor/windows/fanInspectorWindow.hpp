#pragma once

#include <set>
#include <filesystem>
#include "editor/windows/fanEditorWindow.hpp"
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	struct EcsComponentInfo;

	//================================================================================================================================
	// displays a scene node and its components
	//================================================================================================================================
	class InspectorWindow : public EditorWindow
	{
	public:
		InspectorWindow( EcsWorld& _world );

		// Callbacks
		void OnSceneNodeSelected( SceneNode* _node );

	protected:
		void OnGui() override;

	private:
		EcsWorld& m_world;
		EcsHandle m_handleNodeSelected = 0;

		void NewComponentPopup();
		void R_NewComponentPopup( std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector<EcsComponentInfo>& _components, const std::vector<std::filesystem::path>& _componentsPath );
		void NewComponentItem( const EcsComponentInfo& _info );
	};
}