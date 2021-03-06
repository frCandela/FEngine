#pragma once

#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSceneNode
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Gameobject16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiSceneNode::OnGui;
            info.mEditorPath = "";
            info.mEditorName = "Scene node";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            SceneNode& node  = static_cast<SceneNode&>( _component );
            EcsWorld & world = *node.mScene->mWorld;
            EcsEntity entity = world.GetEntity( node.mHandle );

            ImGui::Text( "name      : %s", node.mName.c_str() );
            ImGui::Text( "scene     : %s",
                         node.mScene->mPath.empty() ? "<no path>" : node.mScene->mPath.c_str() );
            ImGui::Text( "handle    : %u", node.mHandle );
            ImGui::Text( "entity id : %u", (int)entity.mIndex );

            ImGui::Text( "tags: " );
            if( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
            {
                ImGui::OpenPopup( "set_tag_popup_menu" );
            }

            // display entity tags
            for( int tagIndex = _world.GetFistTagIndex(); tagIndex < (int)ecsSignatureLength; tagIndex++ )
            {
                if( _world.IndexedHasTag( _entity, tagIndex ) )
                {
                    ImGui::SameLine();
                    ImGui::Text( "%s ", _world.IndexedGetTagInfo( tagIndex ).mName.c_str() );
                }
            }

            // set tags popup
            if( ImGui::BeginPopup( "set_tag_popup_menu" ) )
            {
                for( int tagIndex = _world.GetFistTagIndex(); tagIndex < (int)ecsSignatureLength; tagIndex++ )
                {
                    bool hasTag = _world.IndexedHasTag( _entity, tagIndex );
                    const EcsTagInfo& tagInfo = world.IndexedGetTagInfo( tagIndex );
                    if( ImGui::MenuItem( tagInfo.mName.c_str(), nullptr, &hasTag ) )
                    {
                        if( hasTag )
                        {
                            _world.AddTag( _entity, tagInfo.mType );
                        }
                        else
                        {
                            _world.RemoveTag( _entity, tagInfo.mType );
                        }
                    }
                }
                ImGui::EndPopup();
            }
        }
    };
}