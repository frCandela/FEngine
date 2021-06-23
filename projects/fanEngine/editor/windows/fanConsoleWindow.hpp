#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "core/fanDebug.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Visual output for the debug logging
    //  allow submitting of text commands
    //==================================================================================================================================================================================================
    struct ConsoleWindow : EcsSingleton
    {
    ECS_SINGLETON( ConsoleWindow )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        // Log item with its string already ready to draw
        struct LogItemCompiled
        {
            LogItemCompiled( const Debug::LogItem& _logItem );
            Debug::Severity mSeverity;
            Debug::Type     mType;
            std::string     mLogMessage;
            ImVec4          mColor;
        };

        static const uint32_t        sInputBufferSize = 256;
        std::vector<LogItemCompiled> mLogBuffer;
        int                          mMaxSizeLogBuffers;    // Number of logs kept in memory
        int                          mFirstLogIndex;        // mLogBuffer is used as a circular array
        char                         mInputBuffer[sInputBufferSize];
        bool                         mScrollDown;
        bool                         mGrabFocus;
        int                          mVisibleLogsTypes;

        static ImVec4 GetSeverityColor( const Debug::Severity& _severity );
        void OnNewLog( Debug::LogItem _item );
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiConsoleWindow
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "console";
            info.mIcon       = ImGui::Console16;
            info.mGroup      = EngineGroups::Editor;
            info.mType       = GuiSingletonInfo::Type::ToolWindow;
            info.onGui       = &GuiConsoleWindow::OnGui;
            info.mImGuiWindowFlags = ImGuiWindowFlags_MenuBar;
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsSingleton& _singleton );
    };
}