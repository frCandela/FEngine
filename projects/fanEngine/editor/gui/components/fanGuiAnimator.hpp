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
            info.mGroup      = EngineGroups::Scene;
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

/*::PushID( "animator" );
                (void)animator;
                if( _world.HasComponent<SkinnedMeshRenderer>( _entity ) )
                {
                    SkinnedMeshRenderer& meshRenderer = _world.GetComponent<SkinnedMeshRenderer>( _entity );
                    if( meshRenderer.mMesh != nullptr )
                    {
                        Skeleton& skeleton = meshRenderer.mMesh->mSkeleton;
                        for( int i = 0; i < skeleton.mNumBones; ++i )
                        {
                            ImGui::PushID( i );
                            static Fixed f = 0;
                            if( ImGui::DragFixed( skeleton.mBones[i].mName, &f ) )
                            {
                                const Matrix4& ibm       = skeleton.mInverseBindMatrix[i];
                                Matrix4      & offsetMat = skeleton.mOffsetMatrix[i];

                                offsetMat = ibm.Inverse() * Matrix4( Quaternion::AngleAxis( f, Vector3::sUp ), Vector3::sZero ) * ibm;
                            }
                            ImGui::PopID();
                            ImGui::Separator();
                        }
                    }
                }
                ImGui::PopID();*/
            }
            ImGui::PopItemWidth();
        }
    };
}