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
		InspectorWindow();
	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		static void NewComponentPopup( EcsWorld& _world );
		static void R_NewComponentPopup( EcsWorld& _world, std::set< std::filesystem::path >& _componentsPathSet, std::set< std::filesystem::path >::iterator& _current, const std::vector<EcsComponentInfo>& _components, const std::vector<std::filesystem::path>& _componentsPath );
		static void NewComponentItem( EcsWorld& _world,  const EcsComponentInfo& _info );
	};
}