#include "core/fanSerializedValues.hpp"

#include <fstream>
#include "core/fanDebug.hpp"
#include "core/math/fanVector2.hpp"
#include "core/fanColor.hpp"
#include "bullet/LinearMath/btQuaternion.h"
#include "core/fanSerializable.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"


namespace fan
{
	//========================================================================================================
	//========================================================================================================
	SerializedValues::SerializedValues() :
            mJsonPath( "editor_data.json" )
		, mValuesName( "values" )
		, mKeysBindingsName( "key_bindings" )
	{
		std::ifstream inFile( mJsonPath );
		if ( inFile.good() == true )
		{
			inFile >> mJson;
		}
		inFile.close();
	}

	//========================================================================================================
	//========================================================================================================
	void SerializedValues::SaveValuesToDisk()
	{

		Debug::Log( "Saving value to disk" );
		Input::Get().Manager().Save( mJson[ mKeysBindingsName ] );
		std::ofstream outFile( mJsonPath );
		assert( outFile.is_open() );
		outFile << mJson;
		outFile.close();
	}

	//========================================================================================================
	//========================================================================================================
	void SerializedValues::LoadKeyBindings()
	{
		Input::Get().Manager().Load( mJson[ mKeysBindingsName ] );
	}


	//========================================================================================================
	//========================================================================================================
	void SerializedValues::SaveWindowSize( const glm::ivec2 _size )
    {
        SerializedValues::Get().SetUInt( "renderer_extent_width", _size.x );
        SerializedValues::Get().SetUInt( "renderer_extent_height", _size.y );
    }

    //========================================================================================================
	//========================================================================================================
	void SerializedValues::LoadWindowSize( glm::ivec2& _outSize )
    {
        SerializedValues::Get().GetInt( "renderer_extent_width", _outSize.x );
        SerializedValues::Get().GetInt( "renderer_extent_height", _outSize.y );
    }

    //========================================================================================================
	//========================================================================================================
	void SerializedValues::SaveWindowPosition( const glm::ivec2 _position )
    {
        SerializedValues::Get().SetInt( "renderer_position_x", _position.x );
        SerializedValues::Get().SetInt( "renderer_position_y", _position.y );
    }

    //========================================================================================================
	//========================================================================================================
	void SerializedValues::LoadWindowPosition( glm::ivec2& _outPosition )
    {
        SerializedValues::Get().GetInt( "renderer_position_x", _outPosition.x );
        SerializedValues::Get().GetInt( "renderer_position_y", _outPosition.y );
    }

    void SerializedValues::SetVec2( const char* _name, const btVector2& _vec2 )
    {
        Serializable::SaveVec2( mJson[mValuesName], _name, _vec2 );
    }
    void SerializedValues::SetVec3( const char* _name, const btVector3& _vec3 )
    {
        Serializable::SaveVec3( mJson[mValuesName], _name, _vec3 );
    }
    void SerializedValues::SetQuat( const char* _name, const btQuaternion& _quat )
    {
        Serializable::SaveQuat( mJson[mValuesName], _name, _quat );
    }
    void SerializedValues::SetColor( const char* _name, const Color& _color )
    {
        Serializable::SaveColor( mJson[mValuesName], _name, _color );
    }
    void SerializedValues::SetFloat( const char* _name, const float& _float )
    {
        Serializable::SaveFloat( mJson[mValuesName], _name, _float );
    }
    void SerializedValues::SetInt( const char* _name, const int& _int )
    {
        Serializable::SaveInt( mJson[mValuesName], _name, _int );
    }
    void SerializedValues::SetUInt( const char* _name, const unsigned& _int )
    {
        Serializable::SaveUInt( mJson[mValuesName], _name, _int );
    }
    void SerializedValues::SetBool( const char* _name, const bool& _bool )
    {
        Serializable::SaveBool( mJson[mValuesName], _name, _bool );
    }
    void SerializedValues::SetString( const char* _name, const std::string& _string )
    {
        Serializable::SaveString( mJson[mValuesName], _name, _string );
    }

    bool SerializedValues::GetVec2( const char* _name, btVector2& _outVec2 )
    {
        return Serializable::LoadVec2( mJson[mValuesName], _name, _outVec2 );
    }
    bool SerializedValues::GetVec3( const char* _name, btVector3& _outVec3 )
    {
        return Serializable::LoadVec3( mJson[mValuesName], _name, _outVec3 );
    }
    bool SerializedValues::GetQuat( const char* _name, btQuaternion& _outQuat )
    {
        return Serializable::LoadQuat( mJson[mValuesName], _name, _outQuat );
    }
    bool SerializedValues::GetColor( const char* _name, Color& _outColor )
    {
        return Serializable::LoadColor( mJson[mValuesName], _name, _outColor );
    }
    bool SerializedValues::GetFloat( const char* _name, float& _outFloat )
    {
        return Serializable::LoadFloat( mJson[mValuesName], _name, _outFloat );
    }
    bool SerializedValues::GetInt( const char* _name, int& _outInt )
    {
        return Serializable::LoadInt( mJson[mValuesName], _name, _outInt );
    }
    bool SerializedValues::GetUInt( const char* _name, unsigned& _outUInt )
    {
        return Serializable::LoadUInt( mJson[mValuesName], _name, _outUInt );
    }
    bool SerializedValues::GetBool( const char* _name, bool& _outBool )
    {
        return Serializable::LoadBool( mJson[mValuesName], _name, _outBool );
    }
    bool SerializedValues::GetString( const char* _name, std::string& _outString )
    {
        return Serializable::LoadString( mJson[mValuesName], _name, _outString );
    }
}