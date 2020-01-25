#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class UniqueIdBase {
	public:
		virtual const size_t GetID() const = 0;
	protected:
		static size_t m_globalId;
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	class UniqueId : private UniqueIdBase {
	public:
		UniqueId() {
			m_uniqueId = m_globalId++;
		}
		const size_t GetID() const override { return m_uniqueId; }
	private:
		static size_t m_uniqueId;
	};
	template<typename ComponentType>
	size_t UniqueId<ComponentType>::m_uniqueId = 0;
}