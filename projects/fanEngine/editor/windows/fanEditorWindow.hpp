#pragma once

#include <string>
#include "editor/fanImguiIcons.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    // base class for editor windows
    // automagic hide/show & show buttons in the tools main menu bar
    //==================================================================================================================================================================================================
    class EditorWindow
    {
    public:
        EditorWindow( const std::string _name, const ImGui::IconType _iconType );
        virtual ~EditorWindow();

        void Draw( EcsWorld& _world );

        void SetVisible( bool _value ) { mIsVisible = _value; }
        bool IsVisible() const { return mIsVisible; }
        void AddFlag( const ImGuiWindowFlags _flags ) { mFlags |= _flags; }

        std::string GetName() const { return mName; }
        ImGui::IconType GetIconType() const { return mIconType; }

    protected:
        virtual void OnGui( EcsWorld& _world ) = 0;

        bool mIsVisible;

        ImGuiWindowFlags mFlags = ImGuiWindowFlags_None;

        std::string     mName;
        std::string     mJsonShowWindowKey;
        ImGui::IconType mIconType;
    };
}

