#pragma once

#include "core/ecs/fanEcsTypes.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GroupsColors
    {
        static const int sCount = static_cast<int>( EngineGroups::Count );

        ImVec4 mColors[sCount] = {
                { 1.000f, 1.000f, 1.000f, 1.000f }, // None
                { 0.969f, 0.904f, 0.291f, 1.000f }, // Network
                { 0.476f, 0.968f, 1.000f, 1.000f }, // Scene
                { 0.583f, 0.870f, 0.232f, 1.000f }, // SceneUI
                { 0.443f, 0.371f, 0.887f, 1.000f }, // ScenePhysics
                { 0.133f, 0.824f, 0.635f, 1.000f }, // SceneRender
                { 0.886f, 0.184f, 0.367f, 1.000f }, // Game
                { 0.962f, 0.409f, 0.224f, 1.000f }, // GameNetwork
                { 0.582f, 0.397f, 0.727f, 1.000f }, // Editor
        };

        ImVec4 GetColor( const EngineGroups _group )
        {
            return mColors[static_cast<int>( _group )];
        }
    };
}