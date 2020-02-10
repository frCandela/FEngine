#pragma once

#include "core/fanCorePrecompiled.hpp"

#include "core/ressources/fanRessource.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	class RessourcePtr
	{
		
	public:
		static Signal< RessourcePtr* > s_onInit;

		RessourcePtr(_RessourceType* _ressourceType, _IDType _ressourceID);
		RessourcePtr();

		_IDType			GetID() const { return m_ressourceID; }
		_RessourceType* GetRessource() const { return m_ressource; }

		void			Init(const _IDType m_ressourceId);

		_RessourceType* operator->() const { return m_ressource; }
		_RessourceType* operator*() const { return m_ressource; }
	private:
		_RessourceType* m_ressource = nullptr;
		_IDType			m_ressourceID = 0;
	};

	template< typename _RessourceType, typename _IDType >
	Signal< RessourcePtr<_RessourceType, _IDType>* > RessourcePtr<_RessourceType, _IDType>::s_onInit;

	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	RessourcePtr<_RessourceType, _IDType>::RessourcePtr(_RessourceType* _ressourceType, _IDType _ressourceID) :
		m_ressourceID(_ressourceID),
		m_ressource(_ressourceType)
	{}

	//================================================================================================================================
	// This constructor requires the scene to manually set its gameobject pointer
	// Useful at loading time
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	void RessourcePtr<_RessourceType, _IDType>::Init(const _IDType _id)
	{
		m_ressourceID = _id;
		m_ressource = nullptr;
		s_onInit.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	RessourcePtr<_RessourceType, _IDType>::RessourcePtr() :
		m_ressourceID(),
		m_ressource(nullptr)
	{}
}