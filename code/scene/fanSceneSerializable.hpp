#pragma once

#include "core/fanSerializable.hpp"

namespace fan
{
	class PrefabPtr;
 	struct ComponentPtrBase;

	class EcsWorld;

	namespace Serializable
	{
 		void SaveComponentPtr(Json& _json, const char* _name, const ComponentPtrBase& _ptr);
		void SavePrefabPtr( Json& _json, const char* _name, const PrefabPtr& _ptr );

 		bool LoadComponentPtr(const Json& _json, const char* _name, ComponentPtrBase& _outPtr);
		bool LoadPrefabPtr( const Json& _json, const char* _name, PrefabPtr& _outPtr );
	}
}