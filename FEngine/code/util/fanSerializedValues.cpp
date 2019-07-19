#include "fanIncludes.h"

#include "util/fanSerializedValues.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	SerializedValues::SerializedValues( const std::string & _jsonPath) : 
		m_jsonPath( _jsonPath ) {

		std::ifstream inFile(m_jsonPath);
		if (inFile.good() == true) {
			inFile >> m_json;
		}
		inFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	SerializedValues::~SerializedValues() {
		std::ofstream outFile(m_jsonPath);
		outFile << m_json;
		outFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SerializedValues::Set(const std::string & _key, const btVector3& _value) {
		m_json[_key + std::string("_btvector3_x")] = _value[0];
		m_json[_key + std::string("_btvector3_y")] = _value[1];
		m_json[_key + std::string("_btvector3_z")] = _value[2];
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SerializedValues::Get(const std::string & _key, btVector3& value) const {
		if (Get(_key + std::string("_btvector3_x"), value[0]) == false) {
			return false;
		}
		if (Get(_key + std::string("_btvector3_y"), value[1]) == false) {
			return false;
		}
		if (Get(_key + std::string("_btvector3_z"), value[2]) == false) {
			return false;
		}
		return true;
	}
}