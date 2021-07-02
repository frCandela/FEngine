#pragma once

#include "engine/components/fanPrefabInstance.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"
#include "editor/singletons/fanEditorSelection.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiPrefabInstance
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Prefab16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiPrefabInstance::OnGui;
            info.mEditorPath = "";
            info.mEditorName = "Prefab Instance";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            PrefabInstance& prefabInstance = static_cast<PrefabInstance&>( _component );
            ImGui::PushReadOnly();
            ImGui::FanResourcePtr<Prefab>( "prefab", prefabInstance.mPrefab );
            ImGui::PopReadOnly();
            if( ImGui::Button( "Save" ) && prefabInstance.mPrefab != nullptr )
            {
                SceneNode& node = _world.GetComponent<SceneNode>( _entity );
                prefabInstance.mPrefab->CreateFromSceneNode( node );
                prefabInstance.mPrefab->Save( prefabInstance.mPrefab->mPath );
                Debug::Highlight() << "Saved prefab " << prefabInstance.mPrefab->mPath << Debug::Endl();
            }
            ImGui::SameLine();
            if( ImGui::Button( "Reload" ) && prefabInstance.mPrefab != nullptr )
            {
                SceneNode& node   = _world.GetComponent<SceneNode>( _entity );
                SceneNode& parent = _world.GetComponent<SceneNode>( _world.GetEntity( node.mParentHandle ) );
                PrefabInstance::CreateOverride( prefabInstance, _world, _entity );
                prefabInstance.mPrefab->CreateFromFile( prefabInstance.mPrefab->mPath );
                SceneNode* reloadedPrefab = prefabInstance.mPrefab->Instantiate( parent );
                if( reloadedPrefab != nullptr )
                {
                    EcsEntity reloadedEntity = _world.GetEntity( reloadedPrefab->mHandle );
                    PrefabInstance::ApplyOverride( prefabInstance, _world, reloadedEntity );
                    _world.Kill( _entity );
                    EditorSelection& editorSelection = _world.GetSingleton<EditorSelection>();
                    editorSelection.mSelectedNodeHandle = reloadedPrefab->mHandle;
                    Debug::Highlight() << "Reloaded prefab " << prefabInstance.mPrefab->mPath << Debug::Endl();
                }
                else
                {
                    Debug::Error() << "Failed to reload prefab " << prefabInstance.mPrefab->mPath << Debug::Endl();
                }
            }
        }
    };
}