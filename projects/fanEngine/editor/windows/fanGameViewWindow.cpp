#include "editor/windows/fanGameViewWindow.hpp"

#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameViewWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GameViewWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        GameViewWindow& gameViewWindow = static_cast<GameViewWindow&>( _singleton );
        gameViewWindow.mOnSizeChanged.Clear();
        gameViewWindow.mOnPlay.Clear();
        gameViewWindow.mOnPause.Clear();
        gameViewWindow.mOnResume.Clear();
        gameViewWindow.mOnStop.Clear();
        gameViewWindow.mOnStep.Clear();
        gameViewWindow.mOnSelectGame.Clear();
        gameViewWindow.mSize        = glm::vec2( 1.f, 1.f );
        gameViewWindow.mPosition    = {};
        gameViewWindow.mIsHovered   = false;
        gameViewWindow.mCurrentGame = 0;
        memcpy( gameViewWindow.mStringGameSelectionCombo, "game 1\0\0,", 16 );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiGameViewWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        GameViewWindow& gameViewWindow = static_cast<GameViewWindow&>( _singleton );

        // update window size
        const ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
        glm::vec2    size      = glm::vec2( imGuiSize.x, imGuiSize.y );
        if( gameViewWindow.mSize != size )
        {
            gameViewWindow.mSize.x = glm::clamp( size.x, 0.f, 3840.f );
            gameViewWindow.mSize.y = glm::clamp( size.y, 0.f, 2160.f );
            gameViewWindow.mOnSizeChanged.Emmit( { (uint32_t)gameViewWindow.mSize[0], (uint32_t)gameViewWindow.mSize[1] } );
        }

        // draw menu bar
        if( ImGui::BeginMenuBar() )
        {

            ImGui::Text( "%d x %d", (int)gameViewWindow.mSize.x, (int)gameViewWindow.mSize.y );

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
                    gameViewWindow.mOnPlay.Emmit();
                }
            }
            else
            {
                // Stop
                if( ImGui::ButtonIcon( ImGui::Stop16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
                {
                    gameViewWindow.mOnStop.Emmit();
                }
            }

            const ImVec4 pauseTint
                                 = playState.mState == EditorPlayState::PLAYING ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                            : playState.mState == EditorPlayState::PAUSED ? ImVec4( 0.9f, 0.9f, 0.9f, 1.f )
                                    : disabledColor;

            // Pause
            if( ImGui::ButtonIcon( ImGui::Pause16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0.f ), pauseTint ) )
            {
                if( playState.mState == EditorPlayState::PLAYING ){ gameViewWindow.mOnPause.Emmit(); }
                else if( playState.mState == EditorPlayState::PAUSED ){ gameViewWindow.mOnResume.Emmit(); }
            }

            // Step
            const ImVec4 stepTint = playState.mState == EditorPlayState::PAUSED
                    ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                    : disabledColor;
            if( ImGui::ButtonIcon( ImGui::Step16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ), stepTint ) &&
                playState.mState == EditorPlayState::PAUSED )
            {
                gameViewWindow.mOnStep.Emmit();
            }

            // combo current world
            ImGui::Spacing();
            ImGui::SetNextItemWidth( 200.f );
            if( ImGui::Combo( "##current game", &gameViewWindow.mCurrentGame, gameViewWindow.mStringGameSelectionCombo ) )
            {
                gameViewWindow.mOnSelectGame.Emmit( gameViewWindow.mCurrentGame );
            }

            ImGui::EndMenuBar();
        }

        // update window position
        const ImVec2 cursorPos = ImGui::GetCursorPos();
        const ImVec2 windowPos = ImGui::GetWindowPos();

        gameViewWindow.mPosition = glm::vec2( cursorPos.x + windowPos.x, cursorPos.y + windowPos.y );

        // Draw game
        ImGui::Image( (void*)12, imGuiSize );

        gameViewWindow.mIsHovered = ImGui::IsItemHovered();
    }
}