#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan {

	struct btVector2;
	class Color;

// 	class Component;
// 	class Gameobject;
// 	class Texture;
// 	class Mesh;
// 	class Prefab;
// 	struct IDPtrData;
// 	template< typename _RessourceType, typename _IDType > class RessourcePtr;
// 	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;
// 	using TexturePtr = RessourcePtr<Texture, std::string>;
// 	using MeshPtr = RessourcePtr<Mesh, std::string>;
// 	using PrefabPtr = RessourcePtr<Prefab, std::string>;
// 	using ComponentIDPtr = RessourcePtr< Component, IDPtrData >;


	//================================================================================================================================
	//================================================================================================================================
	class ISerializable {
	public:
		virtual bool Load( const Json & _json ) = 0;
		virtual bool Save( Json & _json ) const = 0;

		static void SaveVec2			( Json & _json, const char * _name, const btVector2&	  _vec2 );
		static void SaveVec3			( Json & _json, const char * _name, const btVector3&	  _vec3 );
		static void SaveVec4			( Json & _json, const char * _name, const btVector4&	  _vec4 );
		static void SaveQuat			( Json & _json, const char * _name, const btQuaternion&	  _quat );
		static void SaveColor			( Json & _json, const char * _name, const Color&		  _color );
		static void SaveFloat			( Json & _json, const char * _name, const float&		  _float );
		static void SaveInt				( Json & _json, const char * _name, const int&			  _int );
		static void SaveUInt			( Json & _json, const char * _name, const unsigned&		  _int );
		static void SaveUInt64			( Json & _json, const char * _name, const uint64_t&		  _uint64 );
		static void SaveBool			( Json & _json, const char * _name, const bool&			  _bool );
		static void SaveString			( Json & _json, const char * _name, const std::string&	  _string );
// 		static void SaveGameobjectPtr	( Json & _json, const char * _name, const GameobjectPtr&  _ptr );
// 		static void SaveTexturePtr	    ( Json & _json, const char * _name, const TexturePtr&	  _ptr );
// 		static void SaveMeshPtr			( Json & _json, const char * _name, const MeshPtr&		  _ptr );
// 		static void SavePrefabPtr		( Json & _json, const char * _name, const PrefabPtr&	  _ptr );
// 		static void SaveComponentPtr	( Json & _json, const char * _name, const ComponentIDPtr& _ptr );
		

		static bool LoadVec2			( const Json & _json, const char * _name, btVector2&			_outVec2 );
		static bool LoadVec3			( const Json & _json, const char * _name, btVector3&			_outVec3 );
		static bool LoadVec4			( const Json & _json, const char * _name, btVector4&			_outVec4 );
		static bool LoadQuat			( const Json & _json, const char * _name, btQuaternion&		_outQuat );
		static bool LoadColor			( const Json & _json, const char * _name, Color&				_outColor );
		static bool LoadFloat			( const Json & _json, const char * _name, float&				_outFloat );
		static bool LoadInt				( const Json & _json, const char * _name, int&				_outInt );
		static bool LoadUInt			( const Json & _json, const char * _name, unsigned&			_outUInt );
		static bool LoadUInt64			( const Json & _json, const char * _name, uint64_t&			_outUInt64 );
		static bool LoadBool			( const Json & _json, const char * _name, bool&				_outBool );
		static bool LoadString			( const Json & _json, const char * _name, std::string&		_outString );
// 		static bool LoadGameobjectPtr	( const Json & _json, const char * _name, GameobjectPtr&		_outPtr );
// 		static bool LoadTexturePtr		( const Json & _json, const char * _name, TexturePtr&			_outPtr );
// 		static bool LoadMeshPtr			( const Json & _json, const char * _name, MeshPtr&			_outPtr );
// 		static bool LoadPrefabPtr		( const Json & _json, const char * _name, PrefabPtr&			_outPtr );
// 		static bool LoadComponentPtr	( const Json & _json, const char * _name, ComponentIDPtr&		_outPtr  );

		static bool ContainsToken( const Json & _json, const char * _name );

	private:
		static const Json * FindToken	( const Json & _json, const char * _name );
	};
}