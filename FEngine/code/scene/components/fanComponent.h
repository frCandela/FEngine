#pragma once

namespace scene
{
	class Gameobject;

	class Component {	
	public:
		friend class Gameobject;

		Component(Gameobject * _gameobject);
		virtual ~Component() {}

		virtual bool IsUnique() const = 0;		// Returns true if there is only one instance of this type of component per GameObject, false otherwise
		virtual std::string GetName() const = 0;
		virtual void Delete() { delete this; };

		// Returns a reference on the Gameobject the component is bound to
		inline Gameobject* GetGameobject() const	{ return m_gameobject; }
		bool IsBeingDeleted() const					{ return m_isBeingDeleted; }
		bool IsModified() const						{ return m_isModified;  }
		void SetModified(const bool _isModified);
		bool IsRemovable() const					{ return m_isRemovable; }
		void SetRemovable(const bool _isRemovable)	{ m_isRemovable = _isRemovable; }	

		template<typename ComponentType>
		bool IsType() {
			return typeid(*this) == typeid(ComponentType);
		}

	private:
		Gameobject*  m_gameobject;
		bool m_isModified;
		bool m_isBeingDeleted;
		bool m_isRemovable;
	};
}