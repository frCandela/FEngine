#pragma once

#include "imgui/imgui.h"

namespace ImGui
{
    //==================================================================================================================================================================================================
    // utility for displaying imgui icons
    //==================================================================================================================================================================================================
    enum IconType : int
    {
        None16 = 0,

        // editor windows
        Imgui16,
        Renderer16,
        Scene16,
        Inspector16,
        Console16,
        Ecs16,
        Profiler16,
        Preferences16,
        Singleton16,
        Terrain16,
        UnitTests16,

        // special
        Gameobject16,
        Prefab16,
        Mesh16,
        LockOpen16,
        LockClosed16,
        Network16,
        Font16,
        Application16,

        // special
        Play16,
        Pause16,
        Step16,
        Stop16,
        CheckNeutral16,
        CheckFailed16,
        CheckSuccess16,
        SignalSlot16,
        Plus16,
        Minus16,
        Up16,
        Down16,
        In16,
        Out16,
        Undo16,
        Redo16,
        Left16,
        Right16,
        Trash16,

        // small
        Plus8,
        Minus8,

        // scene components
        Image16,
        Camera16,
        Transform16,
        Particles16,
        DirLight16,
        PointLight16,
        Material16,
        MeshRenderer16,
        Rigidbody16,
        CubeShape16,
        SphereShape16,
        UiMeshRenderer16,
        UiTransform16,
        UiProgressBar16,
        FollowTransform16,
        Input16,
        FollowTransformUi16,
        Expiration16,
        Button16,
        Text16,
        Layout16,
        Align16,
        Animator16,

        // singletons
        Time16,
        Spawn16,
        Connection16,
        Socket16,
        Pointer16,
        Grid16,
        Selection16,
        Gizmos16,
        CopyPaste16,
        Link16,
        ClientNet16,
        RenderDebug16,

        // generics
        SolarSystem16,
        Heart16,
        Energy16,
        Joystick16,
        Spaceship16,
        Planet16,
        Sun16,
        SunEruption16,
        MainMenu16,

        // network
        ReliabilityLayer16,
        GameData16,

        //game @todo make a mecanism for games to add custom icons
        StickMan16,

        NumIcons
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct IconImpl
    {
        struct IconData
        {
            ImVec2 mUv0;
            ImVec2 mUv1;
        };
        static const float    sIconImageSize;
        static const IconData sIconsList[];
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    inline void Icon( const IconType _id, const ImVec2& _size, const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ), const ImVec4& _borderColor = ImVec4( 0, 0, 0, 0 ) )
    {
        // @todo remove this hardcoded 42 that tells imgui renderer to bind the icons descriptors
        ImGui::Image( (void*)42, _size, IconImpl::sIconsList[_id].mUv0, IconImpl::sIconsList[_id].mUv1, _tintColor, _borderColor );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    inline bool ButtonIcon( const IconType _id, const ImVec2& _size, const int _framePadding = -1, const ImVec4& _bgColor = ImVec4( 0, 0, 0, 0 ), const ImVec4& _tintColor = ImVec4( 1, 1, 1, 1 ) )
    {
        ImGui::PushID( _id );
        bool result = ImGui::ImageButton( (void*)42, _size, IconImpl::sIconsList[_id].mUv0, IconImpl::sIconsList[_id].mUv1, _framePadding, _bgColor, _tintColor );
        ImGui::PopID();
        return result;
    }
}
