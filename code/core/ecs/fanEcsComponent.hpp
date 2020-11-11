#pragma once

#include <string>
#include <type_traits>
#include "fanJson.hpp"
#include "core/fanHash.hpp"
#include "core/ecs/fanEcsEntity.hpp"
#include "core/ecs/fanEcsTypes.hpp"
#include "core/fanImguiIcons.hpp"

namespace sf
{
	class Packet;
}

namespace fan
{
	struct EcsComponent {};

	//========================================================================================================
	// Memcpy is a placement new that uses a copy constructor.
	// If the component is trivially copyable, it is replaced with a fast std::memcpy
    //========================================================================================================
	#define ECS_COMPONENT( _ComponentType)															\
	public:																							\
	template <class T> struct EcsComponentInfoImpl	{												\
		static constexpr uint32_t	 sSize		{ sizeof( T )			  };						\
		static constexpr uint32_t	 sAlignment { alignof( T )			  };						\
		static constexpr const char* sName		{ #_ComponentType		  };						\
		static constexpr uint32_t	 sType		{ SSID( #_ComponentType ) };						\
		static EcsComponent& Instanciate( void* _buffer ) { return *new( _buffer ) T(); }			\
	    static void* Memcpy( void* _dst, const void* _src, size_t /*_count*/ )                      \
	    {	                                                                                        \
	        new( _dst )T( *static_cast<const T*>( _src ) );                                         \
	        return _dst;                                                                            \
        }                                                                                           \
	};																								\
	using Info = EcsComponentInfoImpl< _ComponentType >;											

    struct SlotBase;
	class EcsWorld;

    //========================================================================================================
    //========================================================================================================
	struct EcsComponentInfo
	{
		enum ComponentFlags { 
			None = 0, 
			RollbackNoOverwrite = 1 // on a rollback, old rollback states are not overwritten with new ones
		};

		uint32_t               mType;
		int                    mIndex;
		uint32_t               mSize;
		uint32_t               mAlignment;
		int                    mFlags          = ComponentFlags::None;
		std::vector<SlotBase*> mSlots;                         // callable methods

		void ( *init )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			  // called once at creation
        void ( *setInfo ) ( EcsComponentInfo& ) = nullptr;                            // called once at startup
		void ( *destroy )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			  // called at destruction
		void ( *save )( const EcsComponent&, Json& ) = nullptr;						  // Serialize to json
		void ( *load )( EcsComponent&, const Json& ) = nullptr;						  // Deserialize from json
		void ( *netSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;	  // Serialize to packet
		void ( *netLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;		      // Deserialize from packet
		void ( *rollbackSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;// Serializes rollback
		void ( *rollbackLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;	  // Deserializes rollback
		EcsComponent& ( *construct )( void* ) = nullptr;
		void* ( *copy )( void* _dst, const void* _src, size_t _count ) = nullptr;
	};
}