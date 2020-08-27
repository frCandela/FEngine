#pragma once

#include "editor/windows/fanEditorWindow.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
	//========================================================================================================
	// Visual output for the debug logging
	//  allow submitting of text commands
	//========================================================================================================
	class ConsoleWindow : public EditorWindow
	{
	private:
		// Log item with its string already ready to draw
		struct LogItemCompiled
		{
			LogItemCompiled( const Debug::LogItem& _logItem );
			Debug::Severity mSeverity;
			std::string     mLogMessage;
			ImVec4          mColor;
		};

	public:
		ConsoleWindow();
		~ConsoleWindow() override;

	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		static const uint32_t sInputBufferSize = 256;

		std::vector<LogItemCompiled> mLogBuffer;
		int                          mMaxSizeLogBuffers;	// Number of logs kept in memory
		int                          mFirstLogIndex;		// mLogBuffer is used as a circular array
		char                         mInputBuffer[ sInputBufferSize ];
		bool                         mScrollDown;
		bool                         mGrabFocus;

		static ImVec4	GetSeverityColor( const Debug::Severity& _severity );
		void			OnNewLog( Debug::LogItem _item );
	};
}