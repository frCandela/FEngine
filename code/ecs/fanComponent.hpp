#pragma once

#include "editor/fanImguiIcons.hpp"
#include "core/fanHash.hpp"
#include "fanEcsTypes.hpp"


// TO MIGRATE LATER
// 	//================================
// 	struct ecsPlanet : ecsIComponent
// 	{
// 		static const char* s_name;
// 		void Init()
// 		{
// 			time = 0.f;
// 			speed = 1.f;
// 			radius = 1.f;
// 			phase = 0.f;
// 		}
// 		void Clear(){}
// 
// 		float time = 0.f;
// 		float speed = 1.f;
// 		float radius = 1.f;
// 		float phase = 0.f;
// 	};	//================================
// 	struct ecsBullet : ecsIComponent
// 	{
// 		static const char* s_name;
// 		void Init( const float _durationLeft, const float _damage )
// 		{
// 			durationLeft = _durationLeft;
// 			damage = _damage;
// 		}
// 		void Clear() {}
// 
// 		float durationLeft = 1.f;
// 		float damage = 5.f;
// 	};






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
	static Component& Instanciate( void * _buffer){ return *new( _buffer ) _componentType();}	\

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
	struct Component
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
	static constexpr size_t sizeComponent = sizeof( Component );
	static_assert( sizeComponent == 4 );

	//==============================================================================================================================================================
	// function pointers :
	// onGui		: draws ui associated with the component
	// attach		: clears the component value and registers it when necessary
	// detach		: unregister the component when necessary	(optional)
	// save			: serializes the component to json
	// load			: deserializes the component from json
	// instanciate  : don't touch it, it's auto generated
	//==============================================================================================================================================================
	struct ComponentInfo
	{
		std::string		name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		const char*		editorPath = "";				// editor path ( for the addComponent ) popup of the inspector
		ComponentIndex	index;							// dynamic index in the ecsWorld
		uint32_t		staticIndex;					// static index

		void		 ( *onGui )( Component& ) = nullptr;
		void		 ( *init  )( Component& ) = nullptr; 
		void		 ( *save  )( const Component&, Json& ) = nullptr;
		void		 ( *load  )( Component&, const Json& ) = nullptr;
		Component& ( *instanciate )( void* ) = nullptr;
	};
}