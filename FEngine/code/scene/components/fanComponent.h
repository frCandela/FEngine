#pragma once

#include "util/fanISerializable.h"

namespace scene
{
	class Gameobject;

	class Component : public fan::ISerializable {
	public:
		friend class Gameobject;

		Component(Gameobject * _gameobject);
		virtual ~Component() {}
		virtual bool			IsActor()								const { return false; }
		virtual bool			IsUnique()								const = 0;		// Returns true if there is only one instance of this type of component per GameObject, false otherwise
		virtual const char *	GetName()								const = 0;
		virtual Component *		NewInstance( Gameobject * _gameobject )	const = 0;
		virtual uint32_t		GetType()								const = 0;

		virtual void Delete() { delete this; };

		// Returns a reference on the Gameobject the component is bound to
		inline Gameobject* GetGameobject() const	{ return m_gameobject; }
		bool IsBeingDeleted() const					{ return m_isBeingDeleted; }
		bool IsModified() const						{ return m_isModified;  }
		void SetModified(const bool _isModified);
		bool IsRemovable() const					{ return m_isRemovable; }
		void SetRemovable(const bool _isRemovable)	{ m_isRemovable = _isRemovable; }	

		template<typename ComponentType >
		static uint32_t Register(const uint32_t _id) {
			assert(Components().find(_id) == Components().end()); // Component id already defined
			Components()[_id] = new ComponentType(nullptr);
			return _id;
		}

		template<typename ComponentType >
		bool IsType() {
			return ComponentType::s_type == GetType();
		}

	private:
		Gameobject*  m_gameobject;
		bool m_isModified;
		bool m_isBeingDeleted;
		bool m_isRemovable;

		static Component * NewInstance(const uint32_t _id, Gameobject * _gameobject) {
			return Components()[_id]->NewInstance(_gameobject);
		}
		template<typename ComponentType >
		static const ComponentType * GetSample() { return static_cast< const ComponentType*>(Components()[ComponentType::s_type]); }
		static std::map<uint32_t, const Component * > & Components();
	};
}