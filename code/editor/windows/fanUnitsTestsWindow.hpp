#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "editor/unit_tests/examples/fanUnitTestsCurrency.hpp"

namespace fan
{
    class EcsWorld;
    //================================================================================================================================
    // shows the singletons from the game ecs
    //================================================================================================================================
    class UnitTestsWindow : public EditorWindow
    {
    public:
        UnitTestsWindow();

    protected:
        void OnGui( EcsWorld& _world ) override;

        std::vector< UnitTestDisplayResult > mUnitTestsDisplay;
    };
}