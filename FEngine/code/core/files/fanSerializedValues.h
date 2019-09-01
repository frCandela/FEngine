#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class SerializedValues {
	public:
		SerializedValues( const std::string & _jsonPath );
		~SerializedValues();

		template <typename T>
		void Set(const std::string & _key, const T& _value) { m_json[_key] = _value; }

		template <typename T>
		bool Get(const std::string & _key, T& value) const {
			auto it = m_json.find(_key);
			if (it != m_json.end()) {
				value = m_json[_key];
				return true;
			}
			else {
				return false;
			}
		}

		void Set(const std::string & _key, const btVector3& _value);
		bool Get(const std::string & _key, btVector3& value) const;

	private:
		std::string m_jsonPath;
		nlohmann::json m_json;
	};
}