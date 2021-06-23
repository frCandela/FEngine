#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/math/fanFixedPoint.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // Editor camera data (transform, camera, speed, sensitivity )
    //==================================================================================================================================================================================================
    struct EditorCamera : public EcsSingleton
    {
    ECS_SINGLETON( EditorCamera )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        EcsHandle mCameraHandle;
        Fixed     mSlowSpeed;
        Fixed     mNormalSpeed;
        Fixed     mFastSpeed;
        glm::vec2 mXYSensitivity;

        static void Update( EcsWorld& _world, const Fixed _delta );
        static void CreateEditorCamera( EcsWorld& _world );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorCamera
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor camera";
            info.mIcon       = ImGui::Camera16;
            info.mGroup      = EngineGroups::Editor;
            info.onGui       = &GuiEditorCamera::OnGui;
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );

            ImGui::DragFixed( "slow speed", &editorCamera.mSlowSpeed, 1.f, 0.f, 10000.f );
            ImGui::DragFixed( "normal speed", &editorCamera.mNormalSpeed, 1.f, 0.f, 10000.f );
            ImGui::DragFixed( "fast speed", &editorCamera.mFastSpeed, 1.f, 0.f, 10000.f );
            ImGui::DragFloat2( "xy sensitivity", &editorCamera.mXYSensitivity[0], 1.f, 0.f, 1.f );
        }
    };
}