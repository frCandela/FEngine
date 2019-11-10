#pragma once

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	// A gameobject pointer is used to reference a gameobject using a pointer and a id that can be serialized
	//================================================================================================================================
	template< typename _RessourceType >
	class RessourcePtr
	{		
	public:
		static Signal< RessourcePtr * > s_onCreateUnresolved;
		static Signal< RessourcePtr * > s_onSetFromSelection;

		RessourcePtr( _RessourceType * _ressourceType, uint64_t _ressourceID );		
		RessourcePtr();

		uint64_t GetID( ) const { return m_ressourceId; }
		void	 InitUnresolved( const uint64_t m_ressourceId );
		_RessourceType* operator->() const	{ return m_ressourcePtr; }
		_RessourceType* operator*() const{ return m_ressourcePtr; }

	private:
		_RessourceType *	m_ressourcePtr = nullptr;
		uint64_t			m_ressourceId = 0;
	};

	template< typename _RessourceType >
	Signal< RessourcePtr<_RessourceType> * > RessourcePtr<_RessourceType>::s_onCreateUnresolved;

	template< typename _RessourceType >
	Signal< RessourcePtr<_RessourceType> * >  RessourcePtr<_RessourceType>::s_onSetFromSelection;


	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType >
	RessourcePtr<_RessourceType>::RessourcePtr( _RessourceType * _ressourceType, uint64_t _ressourceID ) :
		m_ressourceId( _ressourceID ),
		m_ressourcePtr( _ressourceType )
	{}

	//================================================================================================================================
	// This constructor requires the scene to manually set its gameobject pointer
	// Useful at loading time
	//================================================================================================================================
	template< typename _RessourceType >
	void RessourcePtr<_RessourceType>::InitUnresolved( const uint64_t _id )
	{
		m_ressourceId = _id;
		m_ressourcePtr = nullptr;
		s_onCreateUnresolved.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType >
	RessourcePtr<_RessourceType>::RessourcePtr() :
		m_ressourceId( 0 ),
		m_ressourcePtr( nullptr )
	{}

	using GameobjectPtr = RessourcePtr<Gameobject>;
}




namespace ImGui
{
	void InputGameobject( const char * _label, fan::GameobjectPtr * _ptr );
}
