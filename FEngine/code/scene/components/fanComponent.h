 #pragma once

#include "core/fanISerializable.h"
#include "core/fanTypeInfo.h"

//Editor
#include "scene/fanComponentsRegister.h"

namespace fan
{
	class Entity;

	//================================================================================================================================
	//================================================================================================================================
	class Component : public ISerializable {
	public:
		friend class Entity;

		// Returns a reference on the entity the component is bound to
		inline Entity* GetEntity() const { return m_entity; }
		bool IsBeingDeleted() const { return m_isBeingDeleted; }
		bool IsModified() const;
		void MarkModified(const bool _updateAABB = false);
		bool IsRemovable() const { return m_isRemovable; }
		void SetRemovable(const bool _isRemovable) { m_isRemovable = _isRemovable; }

		virtual bool IsActor() const { return false; }
		virtual bool IsUnique()	const = 0;		// Returns true if there is only one instance of this type of component per entity, false otherwise
		virtual void OnGui();

		DECLARE_ABSTRACT_TYPE_INFO(Component);
	protected:
		// Friend class entity is the factory of components
		Component();
		virtual ~Component() {}
		virtual void OnAttach();
		virtual void OnDetach();

	private:
		Entity*  m_entity;

		uint64_t m_lastModified;	// Frame index at which it was modified
		bool m_isBeingDeleted : 1;
		bool m_isRemovable : 1;
	};
}