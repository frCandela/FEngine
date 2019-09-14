#include "fanGlobalIncludes.h"

#include "scene/fanEntity.h"
#include "scene/fanScene.h"
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
		}

		//================================================================================================================================
		//================================================================================================================================
		Entity::~Entity() {
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				m_components[componentIndex]->OnDetach();				
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
					m_components[componentIndex]->OnDetach();
					m_components.erase(m_components.begin() + componentIndex);
					delete component;
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
			m_components.push_back(_component);
			_component->OnAttach();
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::ComputeAABB() {

			const scene::Model * model = GetComponent< scene::Model >();
			if (model != nullptr && model->IsBeingDeleted() == false && model->GetMesh() != nullptr && model->GetMesh()->GetIndices().size() > 0) {
				m_aabb = model->ComputeAABB();
			} else {
				const scene::Transform * transform = GetComponent< scene::Transform >();
				if (transform != nullptr) {
					const btVector3 origin = transform->GetPosition();
					const float size = 0.05f;
					m_aabb = shape::AABB(origin - size * btVector3::One(), origin + size * btVector3::One());
				}
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
		bool Entity::Load(std::istream& _in) {
			if (!ReadSegmentHeader(_in, "Entity:")) { return false; }

			std::string buffer;
			if (!ReadString(_in, buffer) || buffer.empty()) { return false; } // name
			SetName(buffer);

			if (!ReadStartToken(_in)) { return false; }
			{
				if (!ReadSegmentHeader(_in, "Components:")) { return false; }
				int nbComponents = -1;
				if (!ReadInteger(_in, nbComponents) || nbComponents < 0) { return false; }
				if (!ReadStartToken(_in)) { return false; }
				{
					for (int componentIndex = 0; componentIndex < nbComponents; componentIndex++)
					{
						if (!ReadSegmentHeader(_in)) { return false; } // "componentName:"
						uint32_t componentID = 0;
						if (!ReadUnsigned(_in, componentID) && componentID != 0) { return false; }
						if (!ReadStartToken(_in)) { return false; }
						{
							scene::Component * component = AddComponent(componentID);
							component->Load(_in);
						} if (!ReadEndToken(_in)) { return false; }
					}
				} if (!ReadEndToken(_in)) { return false; }
				if (!ReadSegmentHeader(_in, "Childs:")) { return false; }
				int nbChilds = -1;
				if (!ReadInteger(_in, nbChilds) || nbChilds < 0) { return false; }
				if (!ReadStartToken(_in)) { return false; }
				{
					for (int entityIndex = 0; entityIndex < nbChilds; entityIndex++)
					{
						Entity * child = m_scene->CreateEntity("tmp", this);
						child->LoadEntity( _in );
					}
				}if (!ReadEndToken(_in)) { return false; }
			} if (!ReadEndToken(_in)) { return false; }

			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Entity::LoadEntity(std::istream& _in) {
			return Load(_in);
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Entity::Save(std::ostream& _out, const int _indentLevel) const {
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
				
				// Count childs to save
				int childsToSaveCount = 0;
				for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
					if (m_childs[childIndex]->HasFlag(NOT_SAVED) == false) {
						++childsToSaveCount;
					}
				}

				_out << indentation1 << "Childs: " << childsToSaveCount << " {" << std::endl; { // childs
					for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
						Entity * entity = m_childs[childIndex];
						if (entity->HasFlag(NOT_SAVED) == false) {
							entity->Save(_out, _indentLevel + 2);
						}
					}
				} _out << indentation1 << "}" << std::endl; // End childs
			} _out << indentation << "}" << std::endl;; // End entity
			return true;
		}		
	}
}