#pragma once

#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiSceneNode
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Gameobject16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiSceneNode::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "scene node";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            SceneNode& node  = static_cast<SceneNode&>( _component );
            EcsWorld & world = *node.mScene->mWorld;
            EcsEntity entity = world.GetEntity( node.mHandle );

            ImGui::Text( "name      : %s", node.mName.c_str() );
            ImGui::Text( "scene     : %s",
                         node.mScene->mPath.empty() ? "<no path>" : node.mScene->mPath.c_str() );
            ImGui::Text( "handle    : %u", node.mHandle );
            ImGui::Text( "entity id : %u", entity );

            ImGui::Text( "tags: " );
            for( int tagIndex = _world.GetFistTagIndex(); tagIndex < ecsSignatureLength; tagIndex++ )
            {
                if( _world.IndexedHasTag( _entity, tagIndex ) )
                {
                    ImGui::SameLine();
                    ImGui::Text( "%s ", _world.IndexedGetTagInfo( tagIndex ).mName.c_str() );
                }
            }
        }
    };
}