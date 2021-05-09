#pragma once

#include "core/ecs/fanSignal.hpp"
#include "editor/windows/fanEditorWindow.hpp"
#include "engine/project/fanLaunchSettings.hpp"
#include "glfw/glfw3.h"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // 3D view displaying a world in a project
    //==================================================================================================================================================================================================
    class ProjectViewWindow : public EditorWindow
    {
    public:
        Signal<VkExtent2D> mOnSizeChanged;
        Signal<>           mOnPlay;
        Signal<>           mOnPause;
        Signal<>           mOnResume;
        Signal<>           mOnStop;
        Signal<>           mOnStep;
        Signal<int>        mOnSelectProject;

        ProjectViewWindow( /*const LaunchSettings::NetworkMode _launchMode*/ );
        glm::vec2 GetSize() const { return mSize; }
        glm::vec2 GetPosition() const { return mPosition; }
        float GetAspectRatio() const { return (float)mSize[0] / (float)mSize[1]; }
        bool IsHovered() const { return mIsHovered; }
        void SetCurrentProject( const int _index ) { mCurrentProject = _index; }
    protected:
        void OnGui( EcsWorld& _world ) override;

    private:
        glm::vec2 mSize           = glm::vec2( 1.f, 1.f );
        glm::vec2 mPosition;
        bool      mIsHovered;
        char      mStringProjectSelectionCombo[16];
        int       mCurrentProject = 0;
    };
}