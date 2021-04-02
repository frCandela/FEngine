#pragma once

#include <cstdint>
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "fanJson.hpp"

class Color;
class btQuaternion;
class btVector3;
class btVector4;


namespace fan
{
    class Color;
    struct Vector3;
    struct Quaternion;
    struct Fixed;

	//========================================================================================================
	// Helpers for serializing variables into json
	//========================================================================================================
	namespace Serializable {
		void SaveIVec2( Json& _json, const char* _name, const glm::ivec2& _vec2 );
		void SaveVec2( Json& _json, const char* _name, const glm::vec2& _vec2 );
		void SaveVec3(Json& _json, const char* _name, const btVector3& _vec3);
        void SaveVec3(Json& _json, const char* _name, const Vector3& _vec3);
		void SaveVec4(Json& _json, const char* _name, const btVector4& _vec4);
		void SaveQuat(Json& _json, const char* _name, const btQuaternion& _quat);
        void SaveQuat(Json& _json, const char* _name, const Quaternion& _quat);
		void SaveColor(Json& _json, const char* _name, const Color& _color);
		void SaveFloat(Json& _json, const char* _name, const float& _float);
        void SaveFixed(Json& _json, const char* _name, const Fixed& _fixed);
		void SaveInt(Json& _json, const char* _name, const int& _int);
		void SaveUInt(Json& _json, const char* _name, const unsigned& _int);
		void SaveUInt64(Json& _json, const char* _name, const uint64_t& _uint64);
		void SaveBool(Json& _json, const char* _name, const bool& _bool);
		void SaveString(Json& _json, const char* _name, const std::string& _string);

		bool LoadIVec2( const Json& _json, const char* _name, glm::ivec2& _outVec2 );
		bool LoadVec2( const Json& _json, const char* _name, glm::vec2& _outVec2 );
		bool LoadVec3(const Json& _json, const char* _name, btVector3& _outVec3);
        bool LoadVec3(const Json& _json, const char* _name, Vector3& _outVec3);
		bool LoadVec4(const Json& _json, const char* _name, btVector4& _outVec4);
		bool LoadQuat(const Json& _json, const char* _name, btQuaternion& _outQuat);
        bool LoadQuat(const Json& _json, const char* _name, Quaternion& _outQuat);
		bool LoadColor(const Json& _json, const char* _name, Color& _outColor);
		bool LoadFloat(const Json& _json, const char* _name, float& _outFloat);
		bool LoadFixed(const Json& _json, const char* _name, Fixed& _outFixed);
		bool LoadInt(const Json& _json, const char* _name, int& _outInt);
		bool LoadUInt(const Json& _json, const char* _name, unsigned& _outUInt);
		bool LoadUInt64(const Json& _json, const char* _name, uint64_t& _outUInt64);
		bool LoadBool(const Json& _json, const char* _name, bool& _outBool);
		bool LoadString(const Json& _json, const char* _name, std::string& _outString);

		bool ContainsToken(const Json& _json, const char* _name);
		const Json* FindToken(const Json& _json, const char* _name);
	}
}