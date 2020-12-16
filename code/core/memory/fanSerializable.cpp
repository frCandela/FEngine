#include "core/memory/fanSerializable.hpp"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "core/fanDebug.hpp"
#include "core/math/fanVector2.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveIVec2( Json& _json, const char* _name, const glm::ivec2& _vec2 )
	{
		_json[_name]["x"] = _vec2.x;
		_json[_name]["y"] = _vec2.y;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveVec2( Json& _json, const char* _name, const btVector2& _vec2 )
	{
		_json[ _name ][ "x" ] = _vec2[ 0 ];
		_json[ _name ][ "y" ] = _vec2[ 1 ];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveVec2( Json& _json, const char* _name, const glm::vec2& _vec2 )
	{
		_json[_name]["x"] = _vec2[0];
		_json[_name]["y"] = _vec2[1];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveVec3( Json& _json, const char* _name, const btVector3& _vec3 )
	{
		_json[ _name ][ "x" ] = _vec3[ 0 ];
		_json[ _name ][ "y" ] = _vec3[ 1 ];
		_json[ _name ][ "z" ] = _vec3[ 2 ];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveVec4( Json& _json, const char* _name, const btVector4& _vec4 )
	{
		_json[ _name ][ "x" ] = _vec4[ 0 ];
		_json[ _name ][ "y" ] = _vec4[ 1 ];
		_json[ _name ][ "z" ] = _vec4[ 2 ];
		_json[ _name ][ "w" ] = _vec4[ 3 ];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveQuat( Json& _json, const char* _name, const btQuaternion& _quat )
	{
		_json[ _name ][ "x" ] = _quat[ 0 ];
		_json[ _name ][ "y" ] = _quat[ 1 ];
		_json[ _name ][ "z" ] = _quat[ 2 ];
		_json[ _name ][ "w" ] = _quat[ 3 ];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveColor( Json& _json, const char* _name, const Color& _color )
	{
		_json[ _name ][ "r" ] = _color[ 0 ];
		_json[ _name ][ "g" ] = _color[ 1 ];
		_json[ _name ][ "b" ] = _color[ 2 ];
		_json[ _name ][ "a" ] = _color[ 3 ];
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveFloat( Json& _json, const char* _name, const float& _float )
	{
		_json[ _name ] = _float;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveInt( Json& _json, const char* _name, const int& _int )
	{
		_json[ _name ] = _int;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveUInt( Json& _json, const char* _name, const unsigned& _int )
	{
		_json[ _name ] = _int;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveUInt64( Json& _json, const char* _name, const uint64_t& _uint64 )
	{
		_json[ _name ] = _uint64;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveBool( Json& _json, const char* _name, const bool& _bool )
	{
		_json[ _name ] = _bool;
	}

	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveString( Json& _json, const char* _name, const std::string& _string )
	{
		_json[ _name ] = _string.c_str();
	}

	//========================================================================================================
	//========================================================================================================
	// LOAD
	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadIVec2( const Json& _json, const char* _name, glm::ivec2& _outVec2 )
	{
		const Json* token = FindToken( _json, _name );
		if( token != nullptr )
		{
			_outVec2.x = ( *token )["x"];
			_outVec2.y = ( *token )["y"];
			return true;
		}
		return false;
	}
	
	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadVec2( const Json& _json, const char* _name, btVector2& _outVec2 )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outVec2[ 0 ] = ( *token )[ "x" ];
			_outVec2[ 1 ] = ( *token )[ "y" ];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadVec2( const Json& _json, const char* _name, glm::vec2& _outVec2 )
	{
		const Json* token = FindToken( _json, _name );
		if( token != nullptr )
		{
			_outVec2[0] = ( *token )["x"];
			_outVec2[1] = ( *token )["y"];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadVec3( const Json& _json, const char* _name, btVector3& _outVec3 )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outVec3[ 0 ] = ( *token )[ "x" ];
			_outVec3[ 1 ] = ( *token )[ "y" ];
			_outVec3[ 2 ] = ( *token )[ "z" ];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadVec4( const Json& _json, const char* _name, btVector4& _outVec4 )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outVec4[ 0 ] = ( *token )[ "x" ];
			_outVec4[ 1 ] = ( *token )[ "y" ];
			_outVec4[ 2 ] = ( *token )[ "z" ];
			_outVec4[ 3 ] = ( *token )[ "w" ];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadQuat( const Json& _json, const char* _name, btQuaternion& _outQuat )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outQuat[ 0 ] = ( *token )[ "x" ];
			_outQuat[ 1 ] = ( *token )[ "y" ];
			_outQuat[ 2 ] = ( *token )[ "z" ];
			_outQuat[ 3 ] = ( *token )[ "w" ];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadColor( const Json& _json, const char* _name, Color& _outColor )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outColor[ 0 ] = ( *token )[ "r" ];
			_outColor[ 1 ] = ( *token )[ "g" ];
			_outColor[ 2 ] = ( *token )[ "b" ];
			_outColor[ 3 ] = ( *token )[ "a" ];
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadFloat( const Json& _json, const char* _name, float& _outFloat )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outFloat = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadInt( const Json& _json, const char* _name, int& _outInt )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outInt = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadUInt( const Json& _json, const char* _name, unsigned& _outUInt )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outUInt = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadUInt64( const Json& _json, const char* _name, uint64_t& _outUInt64 )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outUInt64 = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadBool( const Json& _json, const char* _name, bool& _outBool )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outBool = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadString( const Json& _json, const char* _name, std::string& _outString )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outString = ( *token );
			return true;
		}
		return false;
	}

	//========================================================================================================
	// returns true if a token exists
	//========================================================================================================
	bool Serializable::ContainsToken( const Json& _json, const char* _name )
	{
		return _json.find( _name ) != _json.end();
	}

	//========================================================================================================
	// returns the json token associated with a name or nullptr if it doesn't exists
	//========================================================================================================
	const Json* Serializable::FindToken( const Json& _json, const char* _name )
	{
		auto it = _json.find( _name );
		if ( it != _json.end() )
		{
			return &( *it );
		}
		Debug::Warning() << "Failed to find token: " << _name << Debug::Endl();
		return nullptr;
	}
}