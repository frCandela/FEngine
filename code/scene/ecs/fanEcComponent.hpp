#pragma once

#include "core/imgui/fanImguiIcons.hpp"
#include "core/fanHash.hpp"
#include "fanEcsTypes.hpp"

namespace fan
{
	//================================================================================================================================
	// component
	//================================================================================================================================
#define DECLARE_COMPONENT( _componentType)														\
	public:																						\
	static const uint32_t s_typeInfo;															\
	static const char* s_typeName;																\
	private:																					\
	friend class EcsWorld;																		\
	static ecComponent& Instanciate( void * _buffer){ return *new( _buffer ) _componentType();}	\

#define REGISTER_COMPONENT( _componentType, _name)				\
	const uint32_t _componentType::s_typeInfo = SSID(#_name);	\
	const char* _componentType::s_typeName = _name;				\

	//==============================================================================================================================================================
	// Component is a data struct that stores no logic. Components are processed through System::Run() calls
	// If your component is unique, create a SingletonComponent instead
	//
	// Component must call the (DECLARE/REGISTER)_COMPONENT macro and implement an Clear() method.
	// It also must be registered in the EcsWorld constructor to be assigned a unique ID
	// - localTypeID is a unique id that also correspond to the index of the ComponentsCollection in its specific Entity world
	// - chunckIndex is the index of the Chunck in the ComponentsCollection
	// - index is the index of the component inside the chunck
	//==============================================================================================================================================================
	struct ecComponent
	{
	private:
		friend class ComponentsCollection;
		friend class EcsWorld;
		ComponentIndex		 componentIndex;
		ChunckIndex			 chunckIndex;
		ChunckComponentIndex chunckComponentIndex;

	public:
		ComponentIndex	GetIndex()	const { return componentIndex; };
		Signature		GetSignature()	const { return Signature( 1 ) << componentIndex; }
	};
	static constexpr size_t sizeComponent = sizeof( ecComponent );
	static_assert( sizeComponent == 4 );

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct ComponentInfo
	{
		std::string name;
		ImGui::IconType icon = ImGui::IconType::NONE;
		void ( *onGui )( ecComponent& ) = nullptr;
		void ( *clear )( ecComponent& ) = nullptr;
		void ( *save )( const ecComponent&, Json& ) = nullptr;
		void ( *load )( ecComponent&, const Json& ) = nullptr;
		ecComponent& ( *instanciate )( void* ) = nullptr;
		const char* editorPath = "";
		ComponentIndex index;
		uint32_t staticIndex;
	};
}