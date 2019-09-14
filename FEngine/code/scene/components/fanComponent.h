 #pragma once

#include "core/fanISerializable.h"
#include "core/fanTypeInfo.h"

namespace fan
{
	namespace scene
	{
		class Entity;

		//================================================================================================================================
		//================================================================================================================================
		class Component : public fan::ISerializable {
		public:
			friend class Entity;

			virtual bool			IsUnique()								const = 0;		// Returns true if there is only one instance of this type of component per entity, false otherwise

			// Returns a reference on the entity the component is bound to
			inline Entity* GetEntity() const			{ return m_entity; }
			bool IsBeingDeleted() const					{ return m_isBeingDeleted; }
			bool IsModified() const;
			void SetModified( const bool _updateAABB = false );
			bool IsRemovable() const					{ return m_isRemovable; }
			void SetRemovable(const bool _isRemovable)	{ m_isRemovable = _isRemovable; }

			//TMP
			static bool RegisterComponent(scene::Component * _component) {//TMP
				GetComponents().push_back(_component);//TMP
				return true;
			}//TMP
			static std::vector< scene::Component * >& GetComponents() {//TMP
				static std::vector<scene::Component * > m_components;//TMP
				return m_components;//TMP
			}//TMP

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
			bool m_isBeingDeleted	: 1;
			bool m_isRemovable		: 1;


		};
	}
}