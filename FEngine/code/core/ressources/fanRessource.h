#pragma once

#include "core/fanTypeInfo.h"
#include "core/fanSignal.h"

namespace ressource {
	class Ressource;

	template<typename RessourceType>
	class RessourcePtr {
	public:
		friend class RessourceManager;		

		bool IsLoaded() const { return m_ressource->GetState() == Ressource::loaded; }
		void ForceLoad() { m_ressource->Load();  }
		RessourceType* Get() { 
			if (m_ressource->GetState() == Ressource::State::loaded) {
				return m_ressource;
			}
			else {
				return m_defaultRessource;
			}
		}
		const RessourceType* Get() const{
			if (m_ressource->GetState() == Ressource::State::loaded ) {
				return m_ressource;
			}
			else {
				return m_defaultRessource;
			}
		}
	private:
		RessourcePtr(RessourceType* _ressource = nullptr, RessourceType* _defaultRessource = nullptr) :
			m_ressource(_ressource)
			, m_defaultRessource(_defaultRessource)
		{}
		
		RessourceType * m_ressource = nullptr;
		RessourceType * m_defaultRessource;
	};

	class Ressource {
	public:	
		enum State { loaded, unloaded };
		State GetState() const { return m_state; }

		virtual void Load() {
			assert(m_state == unloaded);
			m_state = loaded;
			onRessourceLoaded.Emmit(this);
		}

		uint32_t GetRessourceID() const { return m_ressourceID; }
		void SetRessourceID( const uint32_t _ressourceID) { m_ressourceID = _ressourceID; }

		DECLARE_ABSTRACT_TYPE_INFO(Ressource)
	protected:
		Ressource() {}		
		util::Signal<Ressource*> onRessourceLoaded;


	private:
		uint32_t	m_ressourceID;
		State		m_state = unloaded;
	};
}