#pragma once

#include "engine/components/fanFollowTransform.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiFollowTransform
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;

            info.mIcon       = ImGui::IconType::FollowTransform16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiFollowTransform::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "follow transform";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
        {
            FollowTransform& followTransform = static_cast<FollowTransform&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );

            enum ImGui::IconType icon = followTransform.mLocked ? ImGui::LockClosed16 : ImGui::LockOpen16;
            if( ImGui::ButtonIcon( icon, { 16, 16 } ) )
            {
                followTransform.mLocked = !followTransform.mLocked;

                FxTransform* transform = _world.SafeGetComponent<FxTransform>( _entityID );
                SceneNode  * sceneNode = _world.SafeGetComponent<SceneNode>( _entityID );
                if( transform != nullptr && sceneNode != nullptr )
                {
                    FollowTransform::UpdateLocalTransform( followTransform, *transform, *sceneNode );
                }
            }
            ImGui::SameLine();
            ImGui::Text( "lock transform " );
            ImGui::SameLine();
            ImGui::FanShowHelpMarker( "press L to toggle" );

            // draw the local transform offset
            /*if( followTransform.mTargetTransform != nullptr && _world.HasComponent<Transform>( _entityID ) )
            {
                btTransform& target = followTransform.mTargetTransform->mTransform;
                btTransform& local = followTransform.mLocalTransform;
                btTransform t = target * local;

                _world.GetSingleton<RenderDebug>().DebugLine( target.getOrigin(), t.getOrigin(), Color::sRed );
            }*/
        }
    };
}