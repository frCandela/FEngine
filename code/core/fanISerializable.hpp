#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{

	struct btVector2;
	class Color;

	//================================================================================================================================
	//================================================================================================================================
	class ISerializable
	{
	public:
		virtual bool Load( const Json& _json ) = 0;
		virtual bool Save( Json& _json ) const = 0;
	};

	namespace Serializable {

		void SaveIVec2( Json& _json, const char* _name, const glm::ivec2& _vec2 );
		void SaveVec2(Json& _json, const char* _name, const btVector2& _vec2);
		void SaveVec3(Json& _json, const char* _name, const btVector3& _vec3);
		void SaveVec4(Json& _json, const char* _name, const btVector4& _vec4);
		void SaveQuat(Json& _json, const char* _name, const btQuaternion& _quat);
		void SaveColor(Json& _json, const char* _name, const Color& _color);
		void SaveFloat(Json& _json, const char* _name, const float& _float);
		void SaveInt(Json& _json, const char* _name, const int& _int);
		void SaveUInt(Json& _json, const char* _name, const unsigned& _int);
		void SaveUInt64(Json& _json, const char* _name, const uint64_t& _uint64);
		void SaveBool(Json& _json, const char* _name, const bool& _bool);
		void SaveString(Json& _json, const char* _name, const std::string& _string);

		bool LoadIVec2( const Json& _json, const char* _name, glm::ivec2& _outVec2 );
		bool LoadVec2(const Json& _json, const char* _name, btVector2& _outVec2);
		bool LoadVec3(const Json& _json, const char* _name, btVector3& _outVec3);
		bool LoadVec4(const Json& _json, const char* _name, btVector4& _outVec4);
		bool LoadQuat(const Json& _json, const char* _name, btQuaternion& _outQuat);
		bool LoadColor(const Json& _json, const char* _name, Color& _outColor);
		bool LoadFloat(const Json& _json, const char* _name, float& _outFloat);
		bool LoadInt(const Json& _json, const char* _name, int& _outInt);
		bool LoadUInt(const Json& _json, const char* _name, unsigned& _outUInt);
		bool LoadUInt64(const Json& _json, const char* _name, uint64_t& _outUInt64);
		bool LoadBool(const Json& _json, const char* _name, bool& _outBool);
		bool LoadString(const Json& _json, const char* _name, std::string& _outString);

		bool ContainsToken(const Json& _json, const char* _name);
		const Json* FindToken(const Json& _json, const char* _name);
	};
}