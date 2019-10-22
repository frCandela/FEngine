#pragma once

#include "core/fanSingleton.h"
#include "core/fanSignal.h"

namespace fan {
	class Renderer;
	class Camera;

	//================================================================================================================================
	//================================================================================================================================
	class Debug : public Singleton<Debug> {
		friend class Singleton<Debug>;
	
	public:
		enum class Code		{ endl };
		enum class Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		enum class Type { render, scene, game, other };
		struct LogItem {
			Severity	severity;
			Type		type;
			std::string message;
			double time;
		};
		Signal<LogItem> onNewLog;

		static Code Endl() { return Code::endl;  }

		static void Log		( const std::string _message );
		static void Warning	( const std::string _message );
		static void Error	( const std::string _message );
		static void Highlight	( const std::string _message );

		static Debug& Log();
		static Debug& Warning();
		static Debug& Error();
		static Debug& Highlight();

		static void Clear();
		static void Break() { __debugbreak(); }
		const std::vector< LogItem >& GetLogBuffer() { return m_logBuffer;  }

		static Renderer &	Render() { return * Get().m_renderer; };
		void SetDebug( Renderer * _renderer );	

	protected:
		Debug();

	private:		
		Severity				m_currentSeverity;
		Type					m_currentType;
		std::stringstream		m_stringstream;
		std::vector< LogItem >	m_logBuffer;

		Renderer * m_renderer;

		void Flush();

	public:
		//================================================================================================================================
		Debug& operator<<(Severity _severity){	// Set the severity of the current log
			m_currentSeverity = _severity;
			return *this;
		}

		Debug& operator<<(Type _type){	// Set the type of the current log
			m_currentType = _type;
			return *this;
		}

		//================================================================================================================================
		template <typename T>
		Debug& operator<<(T _msg){	// Appends a value to the current log
			m_stringstream << _msg;
			return *this;
		}
		//================================================================================================================================
		Debug& operator<<( Code /*_code*/ ){	// Special case of Debug::Get() << Debug::Endl()	
			Flush();			
			return *this;
		}
	};
}

