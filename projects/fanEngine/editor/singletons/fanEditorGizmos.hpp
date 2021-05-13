#pragma once

#include <map>
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/math/fanVector3.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;
    struct Transform;

    //==================================================================================================================================================================================================
    // allows displaying of the translation manipulator
    //==================================================================================================================================================================================================
    struct EditorGizmos : EcsSingleton
    {
    ECS_SINGLETON( EditorGizmos )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        //================================================================
        //================================================================
        struct GizmoCacheData
        {
            int     axisIndex;
            bool    pressed = false;
            Vector3 offset;
        };
        bool DrawMoveGizmo( const Transform& _transform, const size_t _uniqueID, Vector3& _newPosition );

        std::map<size_t, GizmoCacheData> mGizmoCacheData;
        EcsWorld* mWorld;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorGizmos
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor gizmo";
            info.mIcon       = ImGui::Gizmos16;
            info.mGroup      = EngineGroups::Editor;
            return info;
        }
    };
}