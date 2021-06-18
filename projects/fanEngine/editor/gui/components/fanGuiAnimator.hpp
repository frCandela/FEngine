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
            info.mIcon       = ImGui::IconType::None16;
            info.mGroup      = EngineGroups::SceneRender;
            info.onGui       = &GuiAnimator::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "animator";
            return info;
        }

        static void OnGui( EcsWorld& , EcsEntity , EcsComponent& _component )
        {
            Animator& animator = static_cast<Animator&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::FanAnimationPtr("animation", animator.mAnimation );

                if( animator.mAnimation != nullptr )
                {
                    ImGui::SliderInt( "keyframe", &animator.mKeyframe, 0, animator.mAnimation->mNumBones );
                }
            }
            ImGui::PopItemWidth();
        }
    };
}