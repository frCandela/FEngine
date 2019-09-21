#include "fanGlobalIncludes.h"

#include "core/files/fanSerializedValues.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	SerializedValues::SerializedValues() {
		m_jsonPath = "editorValues.json";
		std::ifstream inFile(m_jsonPath);
		if (inFile.good() == true) {
			inFile >> m_json;
		}
		inFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SerializedValues::SaveValuesToDisk() {
		std::ofstream outFile(m_jsonPath);
		assert(outFile.is_open());
		outFile << m_json;
		outFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SerializedValues::SetValue(const std::string & _key, const btVector3& _value) {
		m_json[_key + std::string("_btvector3_x")] = _value[0];
		m_json[_key + std::string("_btvector3_y")] = _value[1];
		m_json[_key + std::string("_btvector3_z")] = _value[2];
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SerializedValues::GetValue(const std::string & _key, btVector3& value) const {
		if (GetValue(_key + std::string("_btvector3_x"), value[0]) == false) {
			return false;
		}
		if (GetValue(_key + std::string("_btvector3_y"), value[1]) == false) {
			return false;
		}
		if (GetValue(_key + std::string("_btvector3_z"), value[2]) == false) {
			return false;
		}
		return true;
	}
}