#pragma once

#include "engine/components/fanAnimator.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"
#include "engine/components/fanSkinnedMeshRenderer.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiAnimator
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Animator16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiAnimator::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "Animator";
            return info;
        }

        static void OnGui( EcsWorld&, EcsEntity, EcsComponent& _component )
        {
            Animator& animator = static_cast<Animator&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::FanAnimationPtr( "animation", animator.mAnimation );

                if( animator.mAnimation != nullptr )
                {
                    if( ImGui::ButtonIcon( ImGui::Left16, { 16, 16 } ) ){ animator.mTime = 0; }
                    ImGui::SameLine();
                    if( ImGui::ButtonIcon( ImGui::Right16, { 16, 16 } ) ){ animator.mTime = animator.mAnimation->mDuration; }
                    ImGui::SameLine();
                    ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 48 );
                    ImGui::SliderFixed( "time", &animator.mTime, 0, animator.mAnimation->mDuration );
                    ImGui::PopItemWidth();
                }
                ImGui::Checkbox( "loop", &animator.mLoop );
            }
            ImGui::PopItemWidth();
        }
    };
}