#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SerializedValues : public Singleton<SerializedValues>{
	public:
		friend class Singleton<SerializedValues>;		
		
		bool GetColor( const std::string & _key, Color & _outColor );
		void SetColor( const std::string & _key, Color _color );

		bool GetVec3( const std::string & _key, btVector3 & _outVec3 );
		void SetVec3( const std::string & _key, btVector3 _vec3 );

		template <typename T>
		void SetValue( const std::string & _key, const T& _value ) { m_json[_key] = _value; }

		template <typename T>
		bool GetValue(const std::string & _key, T& value) const {
			auto it = m_json.find(_key);
			if (it != m_json.end()) {
				value = m_json[_key];
				return true;
			}
			else {
				return false;
			}
		}
		void SaveValuesToDisk();

	protected:
		SerializedValues();

	private:
		std::string m_jsonPath;
		nlohmann::json m_json;
	};
}