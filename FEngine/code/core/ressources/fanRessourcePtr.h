#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template<typename RessourceType>
	class RessourcePtr {
	public:

		bool					IsLoaded() const { return m_ressource->GetState() == Ressource::loaded; }
		void					ForceLoad() { m_ressource->Load(); }
		RessourceType*			Get();
		const RessourceType*	Get() const;

	private:
		RessourcePtr(RessourceType* _ressource = nullptr, RessourceType* _defaultRessource = nullptr) :
			m_ressource(_ressource)
			, m_defaultRessource(_defaultRessource)
		{}

		RessourceType * m_ressource = nullptr;
		RessourceType * m_defaultRessource;
	};

	//================================================================================================================================
	template<typename RessourceType>
	RessourceType*  RessourcePtr<RessourceType>::Get() {
		if (m_ressource->GetState() == Ressource::State::loaded) {
			return m_ressource;
		}
		else {
			return m_defaultRessource;
		}
	}

	//================================================================================================================================
	template<typename RessourceType>
	const RessourceType*  RessourcePtr<RessourceType>::Get() const {
		if (m_ressource->GetState() == Ressource::State::loaded) {
			return m_ressource;
		}
		else {
			return m_defaultRessource;
		}
	}
}