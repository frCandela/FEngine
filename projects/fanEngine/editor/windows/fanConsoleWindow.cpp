#include "editor/windows/fanConsoleWindow.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.destroy = &ConsoleWindow::Destroy;
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        ConsoleWindow& consoleWindow = static_cast<ConsoleWindow&>( _singleton );
        consoleWindow.mMaxSizeLogBuffers = 1024;
        consoleWindow.mFirstLogIndex     = 0;
        consoleWindow.mGrabFocus         = false;
        consoleWindow.mInputBuffer[0] = '\0';
        consoleWindow.mLogBuffer.clear();
        consoleWindow.mLogBuffer.reserve( consoleWindow.mMaxSizeLogBuffers );
        consoleWindow.mScrollDown = false;
        consoleWindow.mGrabFocus  = false;
        Debug::Get().onNewLog.Connect( &ConsoleWindow::OnNewLog, &consoleWindow );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::Destroy( EcsWorld&, EcsSingleton& )
    {
        Debug::Get().onNewLog.Clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiConsoleWindow::OnGui( EcsWorld&, EcsSingleton& _singleton )
    {
        ConsoleWindow& console = static_cast<ConsoleWindow&>( _singleton );

        SCOPED_PROFILE( console_win )

        // List the logs
        const float height = ImGui::GetWindowHeight();
        if( height > 60 )
        {
            ImGui::BeginChild( "scrolling",
                               ImVec2( 0, height - 65 ),
                               true,
                               ImGuiWindowFlags_AlwaysVerticalScrollbar );
            for( int logIndex = console.mFirstLogIndex; logIndex < console.mFirstLogIndex + (int)console.mLogBuffer.size(); logIndex++ )
            {
                const ConsoleWindow::LogItemCompiled& item = console.mLogBuffer[logIndex % console.mMaxSizeLogBuffers];
                ImGui::TextColored( item.mColor, item.mLogMessage.c_str() );    // Time
            }
            if( console.mScrollDown )
            {
                ImGui::SetScrollHereY( 1.0f );
                console.mScrollDown = false;
            }
            ImGui::EndChild();
        }

        // Text input

        if( console.mGrabFocus )
        {
            ImGui::SetKeyboardFocusHere();
            console.mGrabFocus = false;
        }

        // Icon
        ImGui::Icon( ImGui::Console16, { 16, 16 } );
        ImGui::SameLine();

        // Input
        bool pressed = false;
        ImGui::PushItemWidth( ImGui::GetWindowWidth() - 90 );
        if( ImGui::InputText( "##input_console", console.mInputBuffer, IM_ARRAYSIZE( console.mInputBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
        {
            pressed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        // Button
        if( ImGui::Button( ">>" ) )
        {
            pressed = true;
        }
        if( pressed == true )
        {
            const std::string message = console.mInputBuffer;
            if( message.size() > 0 )
            {
                if( message != "clear" )
                {
                    Debug::Get() << "Unknown command: " << message << Debug::Endl();
                }
                else
                {
                    Debug::Get().Clear();
                }
                console.mInputBuffer[0] = '\0';
                console.mScrollDown = true;
                console.mGrabFocus  = true;
            }
        }
    }

    //==================================================================================================================================================================================================
    // ConsoleWindow::LogItem nested constructor
    //==================================================================================================================================================================================================
    ConsoleWindow::LogItemCompiled::LogItemCompiled( const Debug::LogItem& _logItem )
    {
        mSeverity   = _logItem.severity;
        mLogMessage = Debug::SecondsToString( _logItem.time ).c_str() + std::string( " " ) + _logItem.message;
        mColor      = GetSeverityColor( _logItem.severity );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::OnNewLog( Debug::LogItem _item )
    {
        mScrollDown = true;

        if( (int)mLogBuffer.size() < mMaxSizeLogBuffers )
        {
            mLogBuffer.push_back( LogItemCompiled( _item ) );
        }
        else
        {
            mLogBuffer[mFirstLogIndex] = LogItemCompiled( _item );
            mFirstLogIndex = ( mFirstLogIndex + 1 ) % mMaxSizeLogBuffers;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    ImVec4 ConsoleWindow::GetSeverityColor( const Debug::Severity& _severity )
    {
        switch( _severity )
        {
            case Debug::Severity::log:
            {
                return { 1, 1, 1, 1 };        // White
            }
                break;
            case Debug::Severity::highlight:
            {
                return { 0, 1, 0, 1 };        // Green
            }
                break;
            case Debug::Severity::warning:
            {
                return { 1, 1, 0, 1 };        // Yellow
            }
                break;
            case Debug::Severity::error:
            {
                return { 1, 0, 0, 1 };        // Red
            }
                break;
            default:
                fanAssert( false );
                break;
        }
        return { 0, 0, 0, 1 };        // Black
    }
}