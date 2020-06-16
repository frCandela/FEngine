#pragma once

#include <string>
#include <type_traits>
#include "core/fanHash.hpp"
#include "editor/fanImguiIcons.hpp"
#include "ecs/fanEcsEntity.hpp"
#include "ecs/fanEcsTypes.hpp"
#include "fanJson.hpp"

namespace sf
{
	class Packet;
}

namespace fan
{
	struct EcsComponent {};

	//================================
	//================================
	#define ECS_COMPONENT( _ComponentType)															\
	public:																							\
	template <class T> struct EcsComponentInfoImpl	{												\
		static constexpr uint32_t	 s_size		{ sizeof( T )			  };						\
		static constexpr uint32_t	 s_alignment{ alignof( T )			  };						\
		static constexpr const char* s_name		{ #_ComponentType		  };						\
		static constexpr uint32_t	 s_type		{ SSID( #_ComponentType ) };						\
		static EcsComponent& Instanciate( void* _buffer ) { return *new( _buffer ) T(); }			\
	    static void* Memcpy( void* _dst, const void* _src, size_t /*_count*/ ){	new( _dst )T( *static_cast<const T*>( _src ) ); return _dst; } \
	};																								\
	using Info = EcsComponentInfoImpl< _ComponentType >;											\
	


	class EcsWorld;
	//================================
	//================================
	struct EcsComponentInfo
	{
		std::string name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		uint32_t	type;
		int			index;
		uint32_t	size;
		uint32_t	alignment;
		const char* editorPath = "";					// editor path ( for the addComponent ) popup of the inspector

		void ( *init )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			// called at the creation of the component
		void ( *destroy )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			// called at the destruction of the component
		void ( *onGui )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			// called by the editor for gui display
		void ( *save )( const EcsComponent&, Json& ) = nullptr;						// Serializes the component when the scene is saved
		void ( *load )( EcsComponent&, const Json& ) = nullptr;						// Deserializes the component the scene is loaded ( after the init )
		void ( *netSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;	// Serializes the component into a packet to send over network
		void ( *netLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;			// Deserializes the component from a packet
		void ( *rollbackSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;	// Serializes the component rollback data
		void ( *rollbackLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;		// Deserializes the component component rollback data
		EcsComponent& ( *construct )( void* ) = nullptr;							// for constructing components
		void* ( *copy )( void* _dst, const void* _src, size_t _count ) = nullptr;	// for copying components
	};
}