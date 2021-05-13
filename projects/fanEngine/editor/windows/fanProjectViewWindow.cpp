#include "editor/windows/fanProjectViewWindow.hpp"

#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectViewWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ProjectViewWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        ProjectViewWindow& projectViewWindow = static_cast<ProjectViewWindow&>( _singleton );
        projectViewWindow.mOnSizeChanged.Clear();
        projectViewWindow.mOnPlay.Clear();
        projectViewWindow.mOnPause.Clear();
        projectViewWindow.mOnResume.Clear();
        projectViewWindow.mOnStop.Clear();
        projectViewWindow.mOnStep.Clear();
        projectViewWindow.mOnSelectProject.Clear();
        projectViewWindow.mSize           = glm::vec2( 1.f, 1.f );
        projectViewWindow.mPosition = {};
        projectViewWindow.mIsHovered = false;
        projectViewWindow.mCurrentProject = 0;
        memcpy( projectViewWindow.mStringProjectSelectionCombo, "game 1\0\0,", 16 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiProjectViewWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        ProjectViewWindow& projectViewWindow = static_cast<ProjectViewWindow&>( _singleton );

        // update window size
        const ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
        glm::vec2    size      = glm::vec2( imGuiSize.x, imGuiSize.y );
        if( projectViewWindow.mSize != size )
        {
            projectViewWindow.mSize.x = glm::clamp( size.x, 0.f, 3840.f );
            projectViewWindow.mSize.y = glm::clamp( size.y, 0.f, 2160.f );
            projectViewWindow.mOnSizeChanged.Emmit( { (uint32_t)projectViewWindow.mSize[0], (uint32_t)projectViewWindow.mSize[1] } );
        }

        // draw menu bar
        if( ImGui::BeginMenuBar() )
        {

            ImGui::Text( "%d x %d", (int)projectViewWindow.mSize.x, (int)projectViewWindow.mSize.y );

            const ImVec4 disabledColor = ImVec4( 0.3f, 0.3f, 0.3f, 0.3f );

            const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
            if( playState.mState == EditorPlayState::STOPPED )
            {
                // Play
                if( ImGui::ButtonIcon( ImGui::Play16,
                                       { 16, 16 },
                                       -1,
                                       ImVec4( 0, 0, 0, 0 ),
                                       ImVec4( 1.f, 1.f, 1.f, 1.f ) ) )
                {
                    projectViewWindow.mOnPlay.Emmit();
                }
            }
            else
            {
                // Stop
                if( ImGui::ButtonIcon( ImGui::Stop16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
                {
                    projectViewWindow.mOnStop.Emmit();
                }
            }

            const ImVec4 pauseTint
                                 = playState.mState == EditorPlayState::PLAYING ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                            : playState.mState == EditorPlayState::PAUSED ? ImVec4( 0.9f, 0.9f, 0.9f, 1.f )
                                    : disabledColor;

            // Pause
            if( ImGui::ButtonIcon( ImGui::Pause16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0.f ), pauseTint ) )
            {
                if( playState.mState == EditorPlayState::PLAYING ){ projectViewWindow.mOnPause.Emmit(); }
                else if( playState.mState == EditorPlayState::PAUSED ){ projectViewWindow.mOnResume.Emmit(); }
            }

            // Step
            const ImVec4 stepTint = playState.mState == EditorPlayState::PAUSED
                    ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                    : disabledColor;
            if( ImGui::ButtonIcon( ImGui::Step16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ), stepTint ) &&
                playState.mState == EditorPlayState::PAUSED )
            {
                projectViewWindow.mOnStep.Emmit();
            }

            // combo current world
            ImGui::Spacing();
            ImGui::SetNextItemWidth( 200.f );
            if( ImGui::Combo( "##current project", &projectViewWindow.mCurrentProject, projectViewWindow.mStringProjectSelectionCombo ) )
            {
                projectViewWindow.mOnSelectProject.Emmit( projectViewWindow.mCurrentProject );
            }

            ImGui::EndMenuBar();
        }

        // update window position
        const ImVec2 cursorPos = ImGui::GetCursorPos();
        const ImVec2 windowPos = ImGui::GetWindowPos();

        projectViewWindow.mPosition = glm::vec2( cursorPos.x + windowPos.x, cursorPos.y + windowPos.y );

        // Draw project
        ImGui::Image( (void*)12, imGuiSize );

        projectViewWindow.mIsHovered = ImGui::IsItemHovered();
    }
}