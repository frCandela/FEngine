#include "editor/windows/fanConsoleWindow.hpp"
#include "platform/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanBits.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ConsoleWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        ConsoleWindow& consoleWindow = static_cast<ConsoleWindow&>( _singleton );
        consoleWindow.mMaxSizeLogBuffers = 65536;
        consoleWindow.mFirstLogIndex     = 0;
        consoleWindow.mGrabFocus         = false;
        consoleWindow.mInputBuffer[0] = '\0';
        consoleWindow.mLogBuffer.clear();
        consoleWindow.mLogBuffer.reserve( consoleWindow.mMaxSizeLogBuffers );
        consoleWindow.mScrollDown       = false;
        consoleWindow.mGrabFocus        = false;
        consoleWindow.mVisibleLogsTypes = ~0;
        BIT_CLEAR( consoleWindow.mVisibleLogsTypes, int( Debug::Type::Render) );
        BIT_CLEAR( consoleWindow.mVisibleLogsTypes, int( Debug::Type::Resources) );
        Debug::Get().mOnNewLog.Connect( &ConsoleWindow::OnNewLog, &consoleWindow );

        // catch up with already displayed logs
        for( const Debug::LogItem& log : Debug::Get().GetLogBuffer() )
        {
            consoleWindow.OnNewLog( log );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiConsoleWindow::OnGui( EcsWorld&, EcsSingleton& _singleton )
    {
        ConsoleWindow& console = static_cast<ConsoleWindow&>( _singleton );

        SCOPED_PROFILE( console_win )

        // menu bar
        if( ImGui::BeginMenuBar() )
        {
            static_assert( (int)Debug::Type::Count == 7 );
            const char* names[(int)Debug::Type::Count] = { "Default", "Render", "Engine", "Game", "Sound", "Editor", "Resources" };

            if( ImGui::BeginMenu( "View" ) )
            {
                for( int i = 0; i < (int)Debug::Type::Count; i++ )
                {
                    bool visible = BIT_TRUE( console.mVisibleLogsTypes, i );
                    if( ImGui::MenuItem( names[i], nullptr, &visible ) )
                    {
                        if( visible )
                        {
                            BIT_SET( console.mVisibleLogsTypes, i );
                        }
                        else
                        {
                            BIT_CLEAR( console.mVisibleLogsTypes, i );
                        }
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // List the logs
        const float height = ImGui::GetWindowHeight();
        if( height > 60 )
        {
            ImGui::BeginChild( "scrolling", ImVec2( 0, height - 85 ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );
            for( int logIndex = console.mFirstLogIndex; logIndex < console.mFirstLogIndex + (int)console.mLogBuffer.size(); logIndex++ )
            {
                const ConsoleWindow::LogItemCompiled& item = console.mLogBuffer[logIndex % console.mMaxSizeLogBuffers];
                if( BIT_TRUE( console.mVisibleLogsTypes, (int)item.mType ) )
                {
                    ImGui::TextColored( item.mColor, item.mLogMessage.c_str() );    // Time
                }
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
                    Debug::Log() << message << Debug::Endl();
                }
                else
                {
                    Debug::Log( "Clearing logs ", Debug::Type::Editor );
                    Debug::Get().Clear();
                    console.mLogBuffer.clear();
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
        mSeverity   = _logItem.mSeverity;
        mLogMessage = Debug::SecondsToString( _logItem.mTime ).c_str() + std::string( " " ) + _logItem.mMessage;
        mColor      = GetSeverityColor( _logItem.mSeverity );
        mType       = _logItem.mType;
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
            case Debug::Severity::Log:
            {
                return { 1, 1, 1, 1 };        // White
            }
                break;
            case Debug::Severity::Highlight:
            {
                return { 0, 1, 0, 1 };        // Green
            }
                break;
            case Debug::Severity::Warning:
            {
                return { 1, 1, 0, 1 };        // Yellow
            }
                break;
            case Debug::Severity::Error:
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