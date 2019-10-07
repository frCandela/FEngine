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

		Signal<Camera*> onSetMainCamera;
		Signal<LogItem> onNewLog;

		static Code Endl() { return Code::endl;  }
		static void Log		( const std::string _message );
		static void Warning	( const std::string _message );
		static void Error	( const std::string _message );
		static void Highlight	( const std::string _message );
		static void Clear();
		static void Break() { __debugbreak(); }
		const std::vector< LogItem >& GetLogBuffer() { return m_logBuffer;  }

		// Debug references  WARNING : these methods are not available in retail !!
		static Renderer &	Render() { return * Get().m_renderer; };
		Camera &			EditorCamera() { return *m_editorCamera; };
		Camera &			MainCamera() { return *m_mainCamera; };
		void SetDebug( Renderer * _renderer, Camera * _editorCamera, Camera * _mainCamera );
		void SetMainCamera( Camera * _camera );

	protected:
		Debug();

	private:		
		Severity				m_currentSeverity;
		Type					m_currentType;
		std::stringstream		m_stringstream;
		std::vector< LogItem >	m_logBuffer;

		Renderer * m_renderer;
		Camera * m_editorCamera;
		Camera * m_mainCamera;

		void Flush();

	public:
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, Severity _severity){	// Set the severity of the current log
			_logger.m_currentSeverity = _severity;
			return _logger;
		}

		friend Debug& operator<<(Debug& _logger, Type _type){	// Set the type of the current log
			_logger.m_currentType = _type;
			return _logger;
		}

		//================================================================================================================================
		template <typename T>
		friend Debug& operator<<(Debug& _logger, T _msg){	// Appends a value to the current log
			_logger.m_stringstream << _msg;
			return _logger;
		}
		//================================================================================================================================
		friend Debug& operator<<(Debug& _logger, Code /*_code*/ ){	// Special case of Debug::Get() << Debug::Endl()	
			_logger.Flush();			
			return _logger;
		}
	};
}

