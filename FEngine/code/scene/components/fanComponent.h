#pragma once

#include "core/fanISerializable.h"
#include "core/fanTypeInfo.h"

namespace scene
{
	class Gameobject;

	class Component : public fan::ISerializable {
	public:
		friend class Gameobject;

		virtual ~Component() {}
		virtual bool			IsActor()								const { return false; }
		virtual bool			IsUnique()								const = 0;		// Returns true if there is only one instance of this type of component per GameObject, false otherwise

		// Returns a reference on the Gameobject the component is bound to
		inline Gameobject* GetGameobject() const	{ return m_gameobject; }
		bool IsBeingDeleted() const					{ return m_isBeingDeleted; }
		bool IsModified() const						{ return m_isModified;  }
		void SetModified(const bool _isModified);
		bool IsRemovable() const					{ return m_isRemovable; }
		void SetRemovable(const bool _isRemovable)	{ m_isRemovable = _isRemovable; }

		DECLARE_ABSTRACT_TYPE_INFO(Component);
	private:
		Gameobject*  m_gameobject;
		bool m_isModified;
		bool m_isBeingDeleted;
		bool m_isRemovable;

	protected:
		// Friend class Gameobject is the factory of components
		Component();	
		virtual void Initialize() = 0;
		virtual void Delete() = 0;
	};
}