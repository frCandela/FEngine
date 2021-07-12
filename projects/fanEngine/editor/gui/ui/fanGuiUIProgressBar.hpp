#pragma once

#include "engine/ui/fanUIProgressBar.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/gui/fanGuiComponentPtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiUIProgressBar
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::UiProgressBar16;
            info.mGroup      = EngineGroups::SceneUI;
            info.onGui       = &GuiUIProgressBar::OnGui;
            info.mEditorPath = "ui/";
            info.mEditorName = "Ui progress bar";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            UIProgressBar& progressBar = static_cast<UIProgressBar&>( _component );

            ImGui::PushID( "UIProgressBar" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                bool update = false;
                // Progress
                if( ImGui::Button( "##progress_reset" ) )
                {
                    update = true;
                    progressBar.mProgress = 1;
                }
                ImGui::SameLine();
                update |= ImGui::DragFloat( "progress", &progressBar.mProgress, 0.01f, 0.f, 1.f );


                // max width
                if( ImGui::Button( "##width_reset" ) )
                {
                    update = true;
                    progressBar.mMaxSize = FindMostAppropriateSize( _world, _entity );
                }
                ImGui::SameLine();
                update |= ImGui::DragInt( "max width", &progressBar.mMaxSize, 0.01f );
                if( update )
                {
                    _world.AddTag<TagUIModified>( _entity );
                }
            }
            ImGui::PopItemWidth();
            ImGui::PopID();
        }

        static int FindMostAppropriateSize( EcsWorld& _world, EcsEntity& _entity )
        {
            SceneNode& node = _world.GetComponent<SceneNode>( _entity );

            if( !node.mChilds.empty() )
            {
                EcsEntity childEntity = _world.GetEntity( node.mHandle );
                // try get first child image width
                UIRenderer* rendererChild = _world.SafeGetComponent<UIRenderer>( childEntity );
                if( rendererChild != nullptr )
                {
                    return rendererChild->mTexture->mExtent.width / rendererChild->mTiling.x;
                }

                // try get first child transform width
                UITransform* transformChild = _world.SafeGetComponent<UITransform>( childEntity );
                if( transformChild != nullptr)
                {
                    return transformChild->mSize.x;
                }
            }

            // try get image width
            UIRenderer* renderer = _world.SafeGetComponent<UIRenderer>( _entity );
            if( renderer != nullptr )
            {
                return renderer->mTexture->mExtent.width / renderer->mTiling.x;
            }

            // try get first child transform width
            UITransform* transform = _world.SafeGetComponent<UITransform>( _entity );
            if( transform != nullptr)
            {
                return transform->mSize.x;
            }

            return 42;
        }
    };
}