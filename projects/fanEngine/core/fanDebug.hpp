#pragma once

#include <sstream>
#include "core/fanSingleton.hpp"
#include "fanSignal.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Logs messages in the console with various statuses
    // ex: Debug::Error() << "my error" << Debug::Endl();
    //==================================================================================================================================================================================================
    class Debug : public Singleton<Debug>
    {
        friend class Singleton<Debug>;
    public:

        enum class Code
        {
            EndLine
        };
        enum class Severity
        {
            Log = 0, Highlight = 1, Warning = 2, Error = 3
        };
        enum class Type
        {
            Default = 0, Render=1, Engine=2, Game=3, Sound=4, Editor=5, Resources=6, Count=7
        };
        struct LogItem
        {
            Severity    mSeverity;
            Type        mType;
            std::string mMessage;
            double      mTime;
        };
        Signal<LogItem> mOnNewLog;
        int             mTypesPrintedToStd;

        static Code Endl() { return Code::EndLine; }

        static void Log( const std::string _message, const Type _type = Type::Default );
        static void Warning( const std::string _message, const Type _type = Type::Default );
        static void Error( const std::string _message, const Type _type = Type::Default );
        static void Highlight( const std::string _message, const Type _type = Type::Default );

        static Debug& Log();
        static Debug& Warning();
        static Debug& Error();
        static Debug& Highlight();

        static void Clear();
        static std::string SecondsToString( const double _seconds );
        const std::vector<LogItem>& GetLogBuffer() { return mLogBuffer; }

    private:
        Debug();

        Severity             mCurrentSeverity;
        Type                 mCurrentType;
        std::stringstream    mStringstream;
        std::vector<LogItem> mLogBuffer;

        void Flush();

    public:
        Debug& operator<<( Severity _severity )
        {    // Set the severity of the current log
            mCurrentSeverity = _severity;
            return *this;
        }

        Debug& operator<<( Type _type )
        {    // Set the type of the current log
            mCurrentType = _type;
            return *this;
        }

        Debug& operator<<( Fixed _fixed )
        {    // Appends a value to the current log
            mStringstream << _fixed.ToFloat();
            return *this;
        }

        template< typename T >
        Debug& operator<<( T _msg )
        {    // Appends a value to the current log
            mStringstream << _msg;
            return *this;
        }

        Debug& operator<<( Code /*_code*/ )
        {    // Special case of Debug::Get() << Debug::Endl
            Flush();
            return *this;
        }
    };
}

