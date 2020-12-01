#pragma once

#include "editor/singletons/fanEditorGuiInfo.hpp"

#include "project_empty/editor/singletons/fanGuiTestSingleton.hpp"
#include "project_empty/editor/components/fanGuiTestComponent.hpp"

namespace fan
{
#ifdef FAN_EDITOR
    inline void RegisterEditorGuiInfos( EditorGuiInfo& _editorGui )
    {
        _editorGui.mSingletonInfos[ TestSingleton::Info::sType ] = GuiTestSingleton::GetInfo();
        _editorGui.mComponentInfos[ TestComponent::Info::sType ] = GuiTestComponent::GetInfo();
    }
#endif
}
