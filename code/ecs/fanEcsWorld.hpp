#pragma once

#include <unordered_map>
#include "core/fanHash.hpp"
#include "ecs/fanEcsEntity.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"
#include "ecs/fanEcsTag.hpp"
#include "ecs/fanEcsArchetype.hpp"
#include "ecs/fanEcsView.hpp"

namespace fan
{
	struct EcsView;

	//========================================================================================================

	// A transition is created when structural change happen on an entity
	// ( creating / removing components, changing archetype etc )
	//========================================================================================================

	struct EcsTransition
	{
		EcsEntity	  entity;
		EcsSignature  signatureAdd = EcsSignature( 0 );		// bit to 1 means add component
		EcsSignature  signatureRemove = EcsSignature( 0 );	// bit to 1 means remove component
		bool isDead = false;
	};

	//========================================================================================================
	// Contains a component and its destructor,
	// destructor is called at the end of the frame before ApplyTransitions
	//========================================================================================================
	struct DestroyedComponent
	{
		EcsEntity entity;
		EcsComponent& component;
		void ( *destroy )( EcsWorld&, EcsEntity, EcsComponent& );
	};

	//========================================================================================================
	//========================================================================================================
	class EcsWorld
	{
	public:
		// Global
		void ApplyTransitions();
		void Clear();
		int  NumComponents() const	{ return int( m_componentsInfo.size() ); }
		int  GetIndex( const uint32_t  _type ) const;

		// Handles
		EcsEntity	GetEntity		( const EcsHandle _handle ){ return m_handles.at( _handle ); }
		EcsHandle	AddHandle		( const EcsEntity _entity );
		void		SetHandle		( const EcsEntity _entity, EcsHandle _handle );
		EcsHandle	GetHandle		( const EcsEntity _entity ) const;
		void		RemoveHandle	( const EcsEntity _entity );
		EcsHandle	GetNextHandle() const { return m_nextHandle; }
		void		SetNextHandle( const EcsHandle  _handle ) { m_nextHandle = _handle; }
        bool HandleExists( const EcsHandle _handle ) { return m_handles.find( _handle ) != m_handles.end(); }

		// Singletons
		template <typename _SingletonType >	void		AddSingletonType();
		template< typename _SingletonType > _SingletonType& GetSingleton();
        EcsSingleton& GetSingleton( const uint32_t _type ) { return *m_singletons[_type]; }
        const EcsSingleton& GetSingleton( const uint32_t _type ) const { return *m_singletons.at( _type ); }
        const EcsSingletonInfo& GetSingletonInfo( const uint32_t _type ) const
        {
            return m_singletonInfos.at( _type );
        }
		const EcsSingletonInfo*			SafeGetSingletonInfo( const uint32_t _type ) const;
		std::vector< EcsSingletonInfo >	GetVectorSingletonInfo() const;

		// Tags
		template <typename _TagType > void AddTagType();
		template <typename _TagType > void AddTag	( const EcsEntity _entity );
		template <typename _TagType > void RemoveTag( const EcsEntity _entity );
		void AddTag		( const EcsEntity _entity, const uint32_t _type );
		void RemoveTag	( const EcsEntity _entity, const uint32_t _type );
		bool HasTag		( const EcsEntity _entity, const uint32_t _type ) const;
		void AddTagsFromSignature( const EcsEntity _entity, const EcsSignature& _signature );

		// Components
		template <typename _ComponentType >	void			AddComponentType	();
		template <typename _ComponentType > _ComponentType&    AddComponent		( const EcsEntity _entity );
		template <typename _ComponentType > void			    RemoveComponent		( const EcsEntity _entity );
		template <typename _ComponentType > bool			    HasComponent		( const EcsEntity _entity );
		template< typename _ComponentType >	_ComponentType& GetComponent		( const EcsEntity _entity );
		EcsComponent&					AddComponent		( const EcsEntity _entity, const uint32_t _type );
		void							RemoveComponent		( const EcsEntity _entity, const uint32_t _type );
		bool							HasComponent		( const EcsEntity _entity, const uint32_t _type );
		EcsComponent&					GetComponent		( const EcsEntity _entity, const uint32_t _type );
		EcsComponent&			IndexedGetComponent ( const EcsEntity _entity, const int _componentIndex );
		const EcsComponentInfo&				IndexedGetComponentInfo( const int _componentIndex ) const;
		const EcsComponentInfo&				GetComponentInfo( const uint32_t _type ) const;
        const EcsComponentInfo*			    SafeGetComponentInfo( const uint32_t _type ) const;
		const std::vector< EcsComponentInfo >&	GetComponentInfos() const	{ return m_componentsInfo; }

		// Entities
		EcsEntity	CreateEntity();
		void		Kill	( const EcsEntity _entity );
		bool		IsAlive	( const EcsEntity _entity ) const;

		template< typename _tagOrComponentType >			EcsSignature	GetSignature() const;
		template< typename _SystemType, typename... _Args > void			            Run( _Args&&... _args );
		template< typename _SystemType > EcsView							Match() const;

		// Const accessors
        const std::unordered_map<EcsHandle, EcsEntity>& GetHandles() const { return m_handles; }
        const std::unordered_map<EcsSignature, EcsArchetype*>& GetArchetypes() const { return m_archetypes; }
        const EcsArchetype& GetTransitionArchetype() const { return m_transitionArchetype; }

    private:
        const EcsEntityData&  GetEntityData( const EcsEntity _entity ) const
        {
            return _entity.archetype->GetEntityData( _entity.index );
        }
        EcsEntityData& GetEntityData( const EcsEntity _entity )
        {
            return _entity.archetype->GetEntityData( _entity.index );
        }
		EcsArchetype*		FindArchetype( const EcsSignature _signature );
		EcsArchetype&		CreateArchetype( const EcsSignature _signature );
		EcsTransition&		FindOrCreateTransition( const EcsEntity _entity );
		

		EcsHandle											m_nextHandle = 1;	// 0 is a null handle
		int													m_nextTagIndex = ecsSignatureLength - 1;
		EcsSignature										m_tagsMask;// signature of all tag types combined
		EcsArchetype										m_transitionArchetype;
		std::unordered_map< EcsSignature, EcsArchetype* >	m_archetypes;
		std::unordered_map<uint32_t, int >					m_typeToIndex;
		std::unordered_map< EcsHandle, EcsEntity >			m_handles;
		std::unordered_map< uint32_t, EcsSingleton* >		m_singletons;
		std::unordered_map< uint32_t, EcsSingletonInfo >	m_singletonInfos;
		std::vector< EcsComponentInfo >						m_componentsInfo;
		std::vector< EcsTransition >						m_transitions;
		std::vector< DestroyedComponent >					m_destroyedComponents;
	};

	//========================================================================================================
	//========================================================================================================
	template <typename _SingletonType >	void EcsWorld::AddSingletonType()
	{
		static_assert( std::is_base_of< EcsSingleton, _SingletonType>::value );
		assert( m_singletons.find( _SingletonType::s_type ) == m_singletons.end() );

		// Creates the singleton component
		_SingletonType* singleton = new _SingletonType();
		m_singletons[_SingletonType::s_type] = singleton;

		// Registers singleton info
		EcsSingletonInfo info;
		_SingletonType::SetInfo( info );
		info.init = &_SingletonType::Init;
		info.type = _SingletonType::s_type;
		m_singletonInfos[_SingletonType::s_type] = info;

		// init singleton
		info.init( *this, *singleton );
	}
	
	//========================================================================================================
	//========================================================================================================
	template< typename _SingletonType > _SingletonType& EcsWorld::GetSingleton()
	{
		static_assert( std::is_base_of< EcsSingleton, _SingletonType >::value );
		return static_cast<_SingletonType&>( GetSingleton( _SingletonType::s_type ) );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _TagType > void EcsWorld::AddTagType()
	{
		static_assert( std::is_base_of< EcsTag, _TagType>::value );
		assert( m_nextTagIndex >= NumComponents() );
		const int newTagIndex = m_nextTagIndex--;
		m_typeToIndex[_TagType::Info::s_type] = newTagIndex;

		m_tagsMask[newTagIndex] = 1;
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _TagType > void EcsWorld::AddTag( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsTag, _TagType>::value );
		AddTag( _entity, _TagType::Info::s_type );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _TagType > void EcsWorld::RemoveTag( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsTag, _TagType>::value );
		RemoveTag( _entity, _TagType::Info::s_type );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _ComponentType >	void EcsWorld::AddComponentType()
	{
		static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );

		const int nextTypeIndex = NumComponents();
		assert( m_nextTagIndex >= nextTypeIndex );

		// Set component info
		EcsComponentInfo info;
		info.name = _ComponentType::Info::s_name;
		info.construct = &_ComponentType::Info::Instanciate;
        info.copy = std::is_trivially_copyable<_ComponentType>::value
                ? &std::memcpy
                : &_ComponentType::Info::Memcpy;
		info.init = &_ComponentType::Init;
		info.size = _ComponentType::Info::s_size;
		info.alignment = _ComponentType::Info::s_alignment;
		info.type = _ComponentType::Info::s_type;
		info.index = nextTypeIndex;
		_ComponentType::SetInfo( info );
		m_componentsInfo.push_back( info );

		m_typeToIndex[_ComponentType::Info::s_type] = nextTypeIndex;

		m_transitionArchetype.AddComponentType( info );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _ComponentType > _ComponentType& EcsWorld::AddComponent( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
		return static_cast<_ComponentType&>( AddComponent( _entity, _ComponentType::Info::s_type ) );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _ComponentType > void EcsWorld::RemoveComponent( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
		RemoveComponent( _entity, _ComponentType::Info::s_type );
	}

	//========================================================================================================
	//========================================================================================================
	template <typename _ComponentType > bool EcsWorld::HasComponent( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
		return HasComponent( _entity, _ComponentType::Info::s_type );
	}

	//========================================================================================================
	//========================================================================================================
	template< typename _ComponentType >	_ComponentType& EcsWorld::GetComponent( const EcsEntity _entity )
	{
		static_assert( std::is_base_of< EcsComponent, _ComponentType>::value );
		return static_cast<_ComponentType&> ( GetComponent( _entity, _ComponentType::Info::s_type ) );
	}

	//========================================================================================================
	//========================================================================================================
	template< typename _SystemType, typename... _Args > void EcsWorld::Run( _Args&&... _args )
	{
		static_assert( std::is_base_of< EcsSystem, _SystemType >::value );
		_SystemType::Run( *this, Match<_SystemType>(), _args... );
	}

	//========================================================================================================
	//========================================================================================================
	template< typename _tagOrComponentType > EcsSignature EcsWorld::GetSignature() const
	{
        static_assert( std::is_base_of<EcsTag, _tagOrComponentType>::value ||
                       std::is_base_of<EcsComponent, _tagOrComponentType>::value );
		return EcsSignature( 1 ) << m_typeToIndex.at( _tagOrComponentType::Info::s_type );
	}

	//========================================================================================================
	// returns an ecs view matching a system signature
	//========================================================================================================
	template< typename _SystemType > EcsView EcsWorld::Match() const
	{
		static_assert( std::is_base_of< EcsSystem, _SystemType >::value );

		const EcsSignature signature = _SystemType::GetSignature( *this );

		EcsView view( m_typeToIndex, signature );
		for( auto it = m_archetypes.begin(); it != m_archetypes.end(); ++it )
		{
			if( ( it->first & signature ) == signature && !it->second->Empty() )
			{
				view.m_archetypes.push_back( it->second );
			}
		}
		return view;
	}
}