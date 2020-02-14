#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanECSComponents.hpp"
#include "game/fanGameSerializable.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"
#include "core/imgui/fanImguiIcons.hpp"
#include "game/fanGameSerializable.hpp"
#include "core/fanTypeInfo.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Component : public Resource, public ISerializable
	{
	public:
		friend class Gameobject;

		inline Gameobject& GetGameobject() const { return *m_gameobject; }
		inline Scene& GetScene() const { return m_gameobject->GetScene(); }

		bool IsBeingDeleted() const { return m_isBeingDeleted; }
		bool IsRemovable() const { return m_isRemovable; }
		void SetRemovable( const bool _isRemovable ) { m_isRemovable = _isRemovable; }

		virtual bool IsCollider()			const { return false; }
		virtual bool IsActor()				const { return false; }
		virtual bool EditorInstantiable()	const { return true; }

		virtual void OnGui();
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::NONE; };

		DECLARE_ABSTRACT_TYPE_INFO( Component, ISerializable );
	protected:
		// Friend class gameobject is the factory of components
		Component();
		virtual ~Component() {}
		virtual void OnAttach();
		virtual void OnDetach();

		Gameobject* const m_gameobject = nullptr;

		bool Save( Json& _json ) const override;
		bool Load( const Json& _json ) override;

	private:
		bool m_isBeingDeleted : 1;
		bool m_isRemovable : 1;
	};
}

//================================================================================================================================
// Disables the component with a warning if the _expression is false
//================================================================================================================================
#define REQUIRE_TRUE( _expression, _message )				\
		if ( ! (_expression) ) {							\
			Debug::Warning() << _message << Debug::Endl();	\
			SetEnabled( false );							\
		}

//================================================================================================================================
// Get _componentType  from the gameobject and set _pointerToComponent. Disable the component with a warning message if no component is found
//================================================================================================================================
#define REQUIRE_COMPONENT( _componentType, _pointerToComponent )																			\
		_pointerToComponent = m_gameobject->GetComponent<_componentType>();																	\
		if ( _pointerToComponent == nullptr ) {																								\
			Debug::Warning() << GetName() << ": " << #_componentType << " not found on " << GetGameobject().GetName() << Debug::Endl();	\
			SetEnabled( false );																											\
		}

//================================================================================================================================
// Find _componentType  from the scene and set _pointerToComponent. Disable the component with a warning message if no component is found
//================================================================================================================================
#define REQUIRE_SCENE_COMPONENT( _componentType, _pointerToComponent )																		\
		_pointerToComponent = m_gameobject->GetScene().FindComponentOfType<_componentType>();												\
		if ( _pointerToComponent == nullptr ) {																								\
			Debug::Warning() << GetName() << ": " << #_componentType << " not found on " << GetGameobject().GetName() << Debug::Endl();	\
			SetEnabled( false );																											\
		}
