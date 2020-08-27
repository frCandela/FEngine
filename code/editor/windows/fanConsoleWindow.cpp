#include "editor/windows/fanConsoleWindow.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//========================================================================================================
	// ConsoleWindow::LogItem nested constructor
	//========================================================================================================
	ConsoleWindow::LogItemCompiled::LogItemCompiled( const Debug::LogItem& _logItem )
	{
        mSeverity   = _logItem.severity;
        mLogMessage = Time::SecondsToString( _logItem.time ).c_str() + std::string( " " ) + _logItem.message;
        mColor      = GetSeverityColor( _logItem.severity );
	}

	//========================================================================================================
	//========================================================================================================
	ConsoleWindow::ConsoleWindow() :
            EditorWindow( "console", ImGui::IconType::Console16 ),
            mMaxSizeLogBuffers( 256 ),
            mFirstLogIndex( 0 ),
            mGrabFocus( false )
	{
        mInputBuffer[ 0 ] = '\0';
		mLogBuffer.reserve( mMaxSizeLogBuffers );
		Debug::Get().onNewLog.Connect( &ConsoleWindow::OnNewLog, this );
	}

	//========================================================================================================
	//========================================================================================================
	ConsoleWindow::~ConsoleWindow()
	{
		Debug::Get().onNewLog.Clear();
	}

	//========================================================================================================
	//========================================================================================================
	void ConsoleWindow::OnNewLog( Debug::LogItem _item )
	{
        mScrollDown = true;

		if ( (int)mLogBuffer.size() < mMaxSizeLogBuffers )
		{
			mLogBuffer.push_back( LogItemCompiled( _item ) );
		}
		else
		{
            mLogBuffer[ mFirstLogIndex ] = LogItemCompiled( _item );
            mFirstLogIndex = ( mFirstLogIndex + 1 ) % mMaxSizeLogBuffers;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ConsoleWindow::OnGui( EcsWorld& /*_world*/ )
	{
		SCOPED_PROFILE(console)

		// List the logs
		const float height = ImGui::GetWindowHeight();
		if ( height > 60 )
		{
            ImGui::BeginChild( "scrolling",
                               ImVec2( 0, height - 65 ),
                               true,
                               ImGuiWindowFlags_AlwaysVerticalScrollbar );
            for( int logIndex = mFirstLogIndex;
                 logIndex < mFirstLogIndex + (int)mLogBuffer.size();
                 logIndex++ )
            {
				const LogItemCompiled& item = mLogBuffer[logIndex % mMaxSizeLogBuffers ];
				ImGui::TextColored( item.mColor, item.mLogMessage.c_str() );	// Time
			}
			if ( mScrollDown )
			{
				ImGui::SetScrollHereY( 1.0f );
                mScrollDown = false;
			}
			ImGui::EndChild();
		}

		// Text input

		if ( mGrabFocus )
		{
			ImGui::SetKeyboardFocusHere();
            mGrabFocus = false;
		}

		// Icon
		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();

		// Input
		bool pressed = false;
        ImGui::PushItemWidth( ImGui::GetWindowWidth() - 90 );
        if( ImGui::InputText( "##input_console",
                              mInputBuffer,
                              IM_ARRAYSIZE( mInputBuffer ),
                              ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			pressed = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Button
		if ( ImGui::Button( ">>" ) )
		{
			pressed = true;
		}
		if ( pressed == true )
		{
			const std::string message = mInputBuffer;
			if ( message.size() > 0 )
			{
				if ( message != "clear" )
				{
					Debug::Get() << "Unknown command: " << message << Debug::Endl();
				}
				else
				{
					Debug::Get().Clear();
				}
                mInputBuffer[ 0 ] = '\0';
                mScrollDown = true;
                mGrabFocus  = true;
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	ImVec4 ConsoleWindow::GetSeverityColor( const Debug::Severity& _severity )
	{
		switch ( _severity )
		{
		case Debug::Severity::log:
		{
			return { 1,1,1,1 };		// White
		} break;
		case Debug::Severity::highlight:
		{
			return { 0,1,0,1 };		// Green
		} break;
		case Debug::Severity::warning:
		{
			return { 1,1,0,1 };		// Yellow
		} break;
		case Debug::Severity::error:
		{
			return { 1,0,0,1 };		// Red
		} break;
		default:
			assert( false );
			break;
		}
		return { 0,0,0,1 };		// Black
	}
}