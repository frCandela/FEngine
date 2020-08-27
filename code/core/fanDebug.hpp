#pragma once

#include <sstream>
#include "core/fanSingleton.hpp"
#include "ecs/fanSignal.hpp"

namespace fan
{
	//========================================================================================================
	// Logs messages in the console with various statuses
	// ex: Debug::Error() << "my error" << Debug::Endl();
	//========================================================================================================
	class Debug : public Singleton<Debug>
	{
		friend class Singleton<Debug>;
	public:
		enum class Code { endl };
		enum class Severity { log = 0, highlight = 1, warning = 2, error = 3 };
		enum class Type { render, scene, game, other };
		struct LogItem
		{
			Severity	severity;
			Type		type;
			std::string message;
			double      time;
		};
		Signal<LogItem> onNewLog;

		static Code Endl() { return Code::endl; }

		static void Log( const std::string _message );
		static void Warning( const std::string _message );
		static void Error( const std::string _message );
		static void Highlight( const std::string _message );

		static Debug& Log();
		static Debug& Warning();
		static Debug& Error();
		static Debug& Highlight();

		static void Clear();
		const std::vector< LogItem >& GetLogBuffer() { return mLogBuffer; }

	protected:
		Debug();

	private:
		Severity               mCurrentSeverity;
		Type                   mCurrentType;
		std::stringstream      mStringstream;
		std::vector< LogItem > mLogBuffer;

		void Flush();

	public:
		//====================================================================================================
		Debug& operator<<( Severity _severity )
		{	// Set the severity of the current log
			mCurrentSeverity = _severity;
			return *this;
		}

		Debug& operator<<( Type _type )
		{	// Set the type of the current log
			mCurrentType = _type;
			return *this;
		}

		//====================================================================================================
		template <typename T>
		Debug& operator<<( T _msg )
		{	// Appends a value to the current log
			mStringstream << _msg;
			return *this;
		}
		//====================================================================================================
		Debug& operator<<( Code /*_code*/ )
		{	// Special case of Debug::Get() << Debug::Endl()	
			Flush();
			return *this;
		}
	};
}

