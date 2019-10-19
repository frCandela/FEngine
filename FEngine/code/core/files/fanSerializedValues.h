#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SerializedValues : public Singleton<SerializedValues> {
	public:
		friend class Singleton<SerializedValues>;		
		void SaveValuesToDisk();
		void LoadKeyBindings();

		void SetVec2	( const char * _name, const btVector2&		_vec2 );
		void SetVec3	( const char * _name, const btVector3&		_vec3 );
		void SetQuat	( const char * _name, const btQuaternion&	_quat );
		void SetColor	( const char * _name, const Color&			_color );
		void SetFloat	( const char * _name, const float&			_float );
		void SetInt		( const char * _name, const int&			_int );
		void SetUInt	( const char * _name, const unsigned&		_int );
		void SetBool	( const char * _name, const bool&			_bool );
		void SetString	( const char * _name, const std::string&	_string );

		void GetVec2	( const char * _name, btVector2&		_outVec2 );
		void GetVec3	( const char * _name, btVector3&		_outVec3 );
		void GetQuat	( const char * _name, btQuaternion&		_outQuat );
		void GetColor	( const char * _name, Color&			_outColor );
		void GetFloat	( const char * _name, float&			_outFloat );
		void GetInt		( const char * _name, int&				_outInt );
		void GetUInt	( const char * _name, unsigned&			_outUInt );
		void GetBool	( const char * _name, bool&				_outBool );
		void GetString	( const char * _name, std::string&		_outString );

	protected:
		SerializedValues();

	private:
		const char * m_jsonPath;
		const char * m_valuesName;
		const char * m_keysBindingsName;
		Json m_json;
	};
}