#pragma once

#include "core/fanSingleton.hpp"
#include "fanJson.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
	class Color;
    struct Quaternion;
    struct Vector3;

	//========================================================================================================
	// holds global editor value that are saved/restored
	// @todo put this in a singleton component in the editor ecs
	//========================================================================================================
	class SerializedValues : public Singleton<SerializedValues>
	{
	public:
		friend class Singleton<SerializedValues>;
		void SaveValuesToDisk();
		void LoadKeyBindings();

		void SetVec3	( const char * _name, const Vector3&		_vec3 );
        void SetVec3	( const char * _name, const glm::vec3 &		_vec3 );
		void SetQuat	( const char * _name, const Quaternion&	_quat );
		void SetColor	( const char * _name, const Color&			_color );
		void SetFloat	( const char * _name, const float&			_float );
		void SetInt		( const char * _name, const int&			_int );
		void SetUInt	( const char * _name, const unsigned&		_int );
		void SetBool	( const char * _name, const bool&			_bool );
		void SetString	( const char * _name, const std::string&	_string );

		bool GetVec3	( const char * _name, Vector3&		    _outVec3 );
        bool GetVec3	( const char * _name, glm::vec3&		_outVec3 );
		bool GetQuat	( const char * _name, Quaternion&		_outQuat );
		bool GetColor	( const char * _name, Color&			_outColor );
		bool GetFloat	( const char * _name, float&			_outFloat );
		bool GetInt		( const char * _name, int&				_outInt );
		bool GetUInt	( const char * _name, unsigned&			_outUInt );
		bool GetBool	( const char * _name, bool&				_outBool );
		bool GetString	( const char * _name, std::string&		_outString );

		static void SaveWindowSize( const glm::ivec2 _size );
        static void LoadWindowSize( glm::ivec2& _outSize );
        static void SaveWindowPosition( const glm::ivec2 _position );
        static void LoadWindowPosition( glm::ivec2& _outPosition );
	protected:
		SerializedValues();

	private:
		const char* mJsonPath;
		const char* mValuesName;
		const char* mKeysBindingsName;
		Json mJson;
	};
}