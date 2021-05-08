#pragma once

#include <set>
#include "editor/windows/fanEditorWindow.hpp"
#include "core/ecs/fanEcsTypes.hpp"

namespace fan
{
    class EcsWorld;
    struct SceneNode;
    struct EcsComponentInfo;

    //========================================================================================================
    // displays a scene node and its components
    //========================================================================================================
    class InspectorWindow : public EditorWindow
    {
    public:
        InspectorWindow();

    protected:
        void OnGui( EcsWorld& _world ) override;

    private:
        static void NewComponentPopup( EcsWorld& _world );
        static std::string SplitPaths( std::vector<std::string>& _current, std::vector<std::string>& _next );
        static void NewComponentItem( EcsWorld& _world, const EcsComponentInfo& _info );
        static void R_DrawHierarchy( std::vector<std::string> _current,
                                     std::string _fullPath,
                                     EcsWorld& _world,
                                     const std::vector<EcsComponentInfo>& _components,
                                     const std::vector<std::string>& _componentsPath );
    };
}