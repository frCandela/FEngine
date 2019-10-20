#include "fanGlobalIncludes.h"

#include "core/files/fanSerializedValues.h"
#include "core/fanISerializable.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	SerializedValues::SerializedValues() :
		m_jsonPath ( "editor_data.json" )
		, m_valuesName("values")
		, m_keysBindingsName("key_bindings")
	{
		std::ifstream inFile(m_jsonPath);
		if (inFile.good() == true) {
			inFile >> m_json;
		}
		inFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SerializedValues::SaveValuesToDisk() {

		Input::Get().Manager().Save( m_json[m_keysBindingsName]);
		std::ofstream outFile(m_jsonPath);
		assert(outFile.is_open());
		outFile << m_json;
		outFile.close();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SerializedValues::LoadKeyBindings() {
		Input::Get().Manager().Load( m_json[m_keysBindingsName] );
	}

	void SerializedValues::SetVec2	( const char * _name, const btVector2&	 _vec2 ) { ISerializable::SaveVec2( m_json[m_valuesName], _name, _vec2 ); }
	void SerializedValues::SetVec3	( const char * _name, const btVector3&	 _vec3 ) { ISerializable::SaveVec3( m_json[m_valuesName], _name, _vec3 ); }
	void SerializedValues::SetQuat	( const char * _name, const btQuaternion&_quat ) { ISerializable::SaveQuat( m_json[m_valuesName], _name, _quat ); }
	void SerializedValues::SetColor	( const char * _name, const Color&		 _color ) { ISerializable::SaveColor( m_json[m_valuesName], _name, _color ); }
	void SerializedValues::SetFloat	( const char * _name, const float&		 _float ) { ISerializable::SaveFloat( m_json[m_valuesName], _name, _float ); }
	void SerializedValues::SetInt	( const char * _name, const int&		 _int ) { ISerializable::SaveInt( m_json[m_valuesName], _name, _int ); }
	void SerializedValues::SetUInt	( const char * _name, const unsigned&	 _int ) { ISerializable::SaveUInt( m_json[m_valuesName], _name, _int ); }
	void SerializedValues::SetBool	( const char * _name, const bool&		 _bool ) { ISerializable::SaveBool( m_json[m_valuesName], _name, _bool ); }
	void SerializedValues::SetString( const char * _name, const std::string& _string ) { ISerializable::SaveString( m_json[m_valuesName], _name, _string ); }
	
	bool SerializedValues::GetVec2	( const char * _name, btVector2&		 _outVec2 ) { return ISerializable::LoadVec2( m_json[m_valuesName], _name, _outVec2 ); }
	bool SerializedValues::GetVec3	( const char * _name, btVector3&		 _outVec3 ) { return ISerializable::LoadVec3( m_json[m_valuesName], _name, _outVec3 ); }
	bool SerializedValues::GetQuat	( const char * _name, btQuaternion&		 _outQuat ) { return ISerializable::LoadQuat( m_json[m_valuesName], _name, _outQuat ); }
	bool SerializedValues::GetColor	( const char * _name, Color&			 _outColor ) { return ISerializable::LoadColor( m_json[m_valuesName], _name, _outColor ); }
	bool SerializedValues::GetFloat	( const char * _name, float&			 _outFloat ) { return ISerializable::LoadFloat( m_json[m_valuesName], _name, _outFloat ); }
	bool SerializedValues::GetInt	( const char * _name, int&				 _outInt ) { return ISerializable::LoadInt( m_json[m_valuesName], _name, _outInt ); }
	bool SerializedValues::GetUInt	( const char * _name, unsigned&			 _outUInt ) { return ISerializable::LoadUInt( m_json[m_valuesName], _name, _outUInt ); }
	bool SerializedValues::GetBool	( const char * _name, bool&				 _outBool ) { return ISerializable::LoadBool( m_json[m_valuesName], _name, _outBool ); }
	bool SerializedValues::GetString( const char * _name, std::string&		 _outString ) { return ISerializable::LoadString( m_json[m_valuesName], _name, _outString ); }
}