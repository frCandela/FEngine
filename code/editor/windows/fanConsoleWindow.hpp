#pragma once

#include "editor/fanEditorPrecompiled.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	//================================================================================================================================
	// Visual output for the debug logging
	//  allow submitting of text commands
	//================================================================================================================================
	class ConsoleWindow : public EditorWindow
	{
	private:
		// Fast drawing version
		struct LogItem
		{
			LogItem( const Debug::LogItem& _logItem );
			Debug::Severity	severity;
			Debug::Type		logType;
			std::string		logMessage;
			ImVec4			color;
		};

	public:
		ConsoleWindow();
		~ConsoleWindow() override;

	protected:
		void OnGui() override;

	private:
		static const uint32_t s_inputBufferSize = 256;

		std::vector<LogItem> m_logBuffer;
		int		m_maxSizeLogBuffers;					// Number of logs kept in memory		
		int		m_firstLogIndex;						// m_logBuffer is used as a circular array
		char	m_inputBuffer[ s_inputBufferSize ];
		bool	m_scrollDown;
		bool	m_grabFocus;

		static ImVec4	GetSeverityColor( const Debug::Severity& _severity );
		void			OnNewLog( Debug::LogItem _item );
	};
}