#pragma once

namespace fan {
	class GameobjectPtr;

	//================================================================================================================================
	//================================================================================================================================
	class ISerializable {
	public:
		virtual bool Load( Json & _json ) = 0;
		virtual bool Save( Json & _json ) const = 0;

		static void SaveVec2			( Json & _json, const char * _name, const btVector2&	 _vec2 );
		static void SaveVec3			( Json & _json, const char * _name, const btVector3&	 _vec3 );
		static void SaveVec4			( Json & _json, const char * _name, const btVector4&	 _vec4 );
		static void SaveQuat			( Json & _json, const char * _name, const btQuaternion&	 _quat );
		static void SaveColor			( Json & _json, const char * _name, const Color&		 _color );
		static void SaveFloat			( Json & _json, const char * _name, const float&		 _float );
		static void SaveInt				( Json & _json, const char * _name, const int&			 _int );
		static void SaveUInt			( Json & _json, const char * _name, const unsigned&		 _int );
		static void SaveUInt64			( Json & _json, const char * _name, const uint64_t&		 _uint64 );
		static void SaveBool			( Json & _json, const char * _name, const bool&			 _bool );
		static void SaveString			( Json & _json, const char * _name, const std::string&	 _string );
		static void SaveGameobjectPtr	( Json & _json, const char * _name, const GameobjectPtr& _ptr );

		static bool LoadVec2			( Json & _json, const char * _name, btVector2&		_outVec2 );
		static bool LoadVec3			( Json & _json, const char * _name, btVector3&		_outVec3 );
		static bool LoadVec4			( Json & _json, const char * _name, btVector4&		_outVec4 );
		static bool LoadQuat			( Json & _json, const char * _name, btQuaternion&	_outQuat );
		static bool LoadColor			( Json & _json, const char * _name, Color&			_outColor );
		static bool LoadFloat			( Json & _json, const char * _name, float&			_outFloat );
		static bool LoadInt				( Json & _json, const char * _name, int&			_outInt );
		static bool LoadUInt			( Json & _json, const char * _name, unsigned&		_outUInt );
		static bool LoadUInt64			( Json & _json, const char * _name, uint64_t&		_outUInt64 );
		static bool LoadBool			( Json & _json, const char * _name, bool&			_outBool );
		static bool LoadString			( Json & _json, const char * _name, std::string&	_outString );
		static bool LoadGameobjectPtr	( Json & _json, const char * _name, GameobjectPtr&	_outPtr );

	private:
		static Json * FindToken	( Json & _json, const char * _name );
	};
}