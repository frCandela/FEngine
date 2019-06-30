#pragma once

namespace scene
{
	class Gameobject;

	class Component {
	public:
		friend class GameObject;

		virtual ~Component() {}

		virtual void Initialize() = 0;

		virtual bool IsUnique() const = 0;		// Returns true if there is only one instance of this type of component per GameObject, false otherwise
		virtual std::string GetName() const = 0;

		// Returns a reference on the Gameobject the component is bound to
		inline Gameobject* GetGameobject() const { return m_gameobject; }

	private:
		Gameobject*  m_gameobject;
	};
}