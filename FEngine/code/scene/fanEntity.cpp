#include "fanGlobalIncludes.h"

#include "scene/fanEntity.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		//================================================================================================================================
		//================================================================================================================================
		Entity::Entity(const std::string _name, Entity * _parent) :
			m_name(_name)
			, m_flags(Flag::NONE)
			, m_parent(_parent) {

			if (_parent != nullptr) {
				_parent->m_childs.push_back(this);
			}

			onComponentModified.Connect(&Entity::OnComponentModified, this);
			onComponentDeleted.Connect(&Entity::OnComponentDeleted, this);
		}

		//================================================================================================================================
		//================================================================================================================================
		Entity::~Entity() {
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				m_components[componentIndex]->Delete();
			}
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				delete m_components[componentIndex];
			}
			m_components.clear();
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Entity::DeleteComponent(const Component * component)
		{
			// Find the component
			for (int componentIndex = 0; componentIndex < m_components.size(); ++componentIndex)
			{
				if (m_components[componentIndex] == component)
				{
					// Deletes it
					m_components[componentIndex]->m_isBeingDeleted = true;
					onComponentDeleted.Emmit(m_components[componentIndex]);
					m_components[componentIndex]->Delete();
					m_components.erase(m_components.begin() + componentIndex);
					return true;
				}
			}
			return false;
		}

		//================================================================================================================================
		//================================================================================================================================
		Component* Entity::AddComponent(const uint32_t _componentID) { 
			scene::Component * component = TypeInfo::Instantiate<Component>(_componentID);
			AddComponent(component);
			return component;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::AddComponent(scene::Component * _component) {
			_component->m_entity = this;
			_component->Initialize();
			m_components.push_back(_component);
			onComponentCreated.Emmit(_component);
			onComponentModified.Emmit(_component);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::ComputeAABB() {

			const scene::Model * model = GetComponent< scene::Model >();
			if (model != nullptr && model->IsBeingDeleted() == false && model->GetMesh() != nullptr && model->GetMesh()->GetIndices().size() > 0) {
				m_aabb = model->ComputeAABB();
			}
			else {
				const btVector3 origin = GetComponent< scene::Transform >()->GetPosition();
				const float size = 0.05f;
				m_aabb = shape::AABB(origin - size * btVector3::One(), origin + size * btVector3::One());
			}
		}		

		//================================================================================================================================
		//================================================================================================================================
		bool Entity::IsAncestorOf(const Entity * _entity) const {
			if (_entity == nullptr) {
				fan::Debug::Log("IsAncestorOf: entity is null");
				return false;
			}

			while (_entity->m_parent != nullptr) {
				if (_entity->m_parent == this) {
					return true;
				}
				else {
					_entity = _entity->m_parent;
				}
			} return false;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::RemoveChild(const Entity * _child) {
			for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
				Entity * child = m_childs[childIndex];
				if (child == _child) {
					m_childs.erase(m_childs.begin() + childIndex);
					return;
				}
			}
		}
		
		//================================================================================================================================
		//================================================================================================================================
		bool Entity::HasChild(const Entity * _child) {
			for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
				Entity * child = m_childs[childIndex];
				if (child == _child) {
					return true;
				}
			}
			return false;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::AddChild(Entity * _child) {
			if (_child == nullptr) {
				fan::Debug::Log("AddChild : child is null");
				return;
			}

			if (_child->IsAncestorOf(this)) {
				fan::Debug::Log("Cannot parent an object to one of its children");
				return;
			}

			if (_child->m_parent == this) {
				fan::Debug::Get() << fan::Debug::Severity::log << _child->m_name << " is already a child of " << m_name << Debug::Endl();
				return;
			}

			if (HasChild(_child) == false) {
				if (_child->m_parent != nullptr) {
					_child->m_parent->RemoveChild(_child);
				}
				m_childs.push_back(_child);
				_child->m_parent = this;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::SetParent(Entity * _parent) {
			if (_parent == nullptr) {
				fan::Debug::Log("Root cannot have a brother :'(");
				return;
			}
			_parent->AddChild(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::InsertBelow(Entity * _brother) {
			if (_brother == nullptr) {
				fan::Debug::Log("InsertBelow: entity is null");
				return;
			}
			if (IsAncestorOf(_brother)) {
				fan::Debug::Log("Cannot parent an object to one of its children");
				return;
			}
			if (_brother->m_parent == nullptr) {
				fan::Debug::Log("Root cannot have a brother :'(");
				return;
			}

			m_parent->RemoveChild(this);

			for (int childIndex = 0; childIndex < _brother->m_parent->m_childs.size(); childIndex++) {
				Entity * child = _brother->m_parent->m_childs[childIndex];
				if (child == _brother) {
					_brother->m_parent->m_childs.insert(_brother->m_parent->m_childs.begin() + childIndex + 1, this);
					m_parent = _brother->m_parent;
				}
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::OnComponentModified(scene::Component * _component) {
			if (_component->IsType<scene::Transform>() || _component->IsType<scene::Model>()) {
				ComputeAABB();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::OnComponentDeleted(scene::Component * _component) {
			if (_component->IsType<scene::Model>()) {
				ComputeAABB();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::Load(std::istream& _in) {
			std::string buffer;
			_in >> buffer;
			while (buffer != "end") {

				// Get component id
				uint32_t componentID;
				_in >> componentID;

				fan::Debug::Get() << fan::Debug::Severity::log << "\tComponent: " << buffer << Debug::Endl();

				// Instanciate component

				scene::Component * component = AddComponent(componentID);
				component->Load(_in);
				_in >> buffer; // skip component name
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::Save(std::ostream& _out, const int _indentLevel) {
			const std::string indentation = GetIndentation(_indentLevel);
			const std::string indentation1 = GetIndentation(_indentLevel + 1);
			const std::string indentation2 = GetIndentation(_indentLevel + 2);

			_out << indentation << "Entity: " << m_name << " {" << std::endl;; { // entity
				

				_out << indentation1 << "Components: " << m_components.size() << " {" << std::endl; { // components
					for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
						scene::Component * component = m_components[componentIndex];
						_out << indentation2 << component->GetName() << ": " << component->GetType() << " {" << std::endl;
						component->Save(_out, _indentLevel + 3);
						_out << indentation2 << "}" << std::endl;
					} _out << indentation1 << "}" << std::endl; // End components
				}
				
				_out << indentation1 << "Childs: " << m_childs.size() << " {" << std::endl; { // childs
					for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
						Entity * entity = m_childs[childIndex];
						entity->Save(_out, _indentLevel + 2);
					}
				} _out << indentation1 << "}" << std::endl; // End childs
			} _out << indentation << "}" << std::endl;; // End entity
		}
	}
}