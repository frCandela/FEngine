#pragma once

#ifdef FAN_EDITOR

#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "project_empty/editor/singletons/fanGuiTestSingleton.hpp"
#include "project_empty/editor/components/fanGuiTestComponent.hpp"

namespace fan
{
    //========================================================================================================
    // Registers gui informations when launching with an Editor container
    //========================================================================================================

    inline void RegisterEditorGuiInfos( EditorGuiInfo& _editorGui )
    {
        _editorGui.mSingletonInfos[ TestSingleton::Info::sType ] = GuiTestSingleton::GetInfo();
        _editorGui.mComponentInfos[ TestComponent::Info::sType ] = GuiTestComponent::GetInfo();
    }
}

#endif