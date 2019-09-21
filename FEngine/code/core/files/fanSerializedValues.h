#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SerializedValues : public Singleton<SerializedValues>{
	public:
		friend class Singleton<SerializedValues>;

		template <typename T>
		void SetValue(const std::string & _key, const T& _value) { m_json[_key] = _value; }

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
		void SetValue(const std::string & _key, const btVector3& _value);
		bool GetValue(const std::string & _key, btVector3& value) const;
		void SaveValuesToDisk();

	protected:
		SerializedValues();

	private:
		std::string m_jsonPath;
		nlohmann::json m_json;
	};
}