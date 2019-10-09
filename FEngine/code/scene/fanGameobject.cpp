#include "fanGlobalIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/fanScene.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"

#include "renderer/fanMesh.h"
#include "core/fanSignal.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Gameobject::Gameobject(const std::string _name, Gameobject * _parent) :
		m_name(_name)
		, m_flags(Flag::NONE)
		, m_parent(_parent) {

		if (_parent != nullptr) {
			_parent->m_childs.push_back(this);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject::~Gameobject() {
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
	void Gameobject::OnGui() {
		std::stringstream ss;
		ss << "Gameobject : " << GetName();
		if (ImGui::CollapsingHeader(ss.str().c_str())) {
			ImGui::Checkbox("ComputeAABB", &m_computeAABB);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::DeleteComponent(const Component * component)
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
	Component* Gameobject::AddComponent(const uint32_t _componentID) {
		Component * component = TypeInfo::Instantiate<Component>(_componentID);
		AddComponent(component);
		return component;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::AddComponent(Component * _component) {
		_component->m_gameobject = this;
		m_components.push_back(_component);
		_component->OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::ComputeAABB() {
		if (m_computeAABB) {
			const Model * model = GetComponent< Model >();
			if (model != nullptr && model->IsBeingDeleted() == false && model->GetMesh() != nullptr && model->GetMesh()->GetIndices().size() > 0) {
				m_aabb = model->ComputeAABB();
			}
			else {
				const Transform * transform = GetComponent< Transform >();
				if (transform != nullptr) {
					const btVector3 origin = transform->GetPosition();
					const float size = 0.05f;
					m_aabb = AABB(origin - size * btVector3::One(), origin + size * btVector3::One());
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::IsAncestorOf(const Gameobject * _gameobject) const {
		if (_gameobject == nullptr) {
			Debug::Log("IsAncestorOf: gameobject is null");
			return false;
		}

		while (_gameobject->m_parent != nullptr) {
			if (_gameobject->m_parent == this) {
				return true;
			}
			else {
				_gameobject = _gameobject->m_parent;
			}
		} return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::RemoveChild(const Gameobject * _child) {
		for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
			Gameobject * child = m_childs[childIndex];
			if (child == _child) {
				m_childs.erase(m_childs.begin() + childIndex);
				return;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::HasChild(const Gameobject * _child) {
		for (int childIndex = 0; childIndex < m_childs.size(); childIndex++) {
			Gameobject * child = m_childs[childIndex];
			if (child == _child) {
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::AddChild(Gameobject * _child) {
		if (_child == nullptr) {
			Debug::Log("AddChild : child is null");
			return;
		}

		if (_child->IsAncestorOf(this)) {
			Debug::Log("Cannot parent an object to one of its children");
			return;
		}

		if (_child->m_parent == this) {
			Debug::Get() << Debug::Severity::log << _child->m_name << " is already a child of " << m_name << Debug::Endl();
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
	void Gameobject::SetParent(Gameobject * _parent) {
		if (_parent == nullptr) {
			Debug::Log("Root cannot have a brother :'(");
			return;
		}
		_parent->AddChild(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::InsertBelow(Gameobject * _brother) {
		if (_brother == nullptr) {
			Debug::Log("InsertBelow: gameobject is null");
			return;
		}
		if (IsAncestorOf(_brother)) {
			Debug::Log("Cannot parent an object to one of its children");
			return;
		}
		if (_brother->m_parent == nullptr) {
			Debug::Log("Root cannot have a brother :'(");
			return;
		}

		m_parent->RemoveChild(this);

		for (int childIndex = 0; childIndex < _brother->m_parent->m_childs.size(); childIndex++) {
			Gameobject * child = _brother->m_parent->m_childs[childIndex];
			if (child == _brother) {
				_brother->m_parent->m_childs.insert(_brother->m_parent->m_childs.begin() + childIndex + 1, this);
				m_parent = _brother->m_parent;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::Load(std::istream& _in) {
		if (!ReadSegmentHeader(_in, "Gameobject:")) { return false; }

		std::string buffer;
		if (!ReadString(_in, buffer) || buffer.empty()) { return false; } // gameobject name
		SetName(buffer);
		if (!ReadStartToken(_in)) { return false; }
		{
			if (!ReadSegmentHeader(_in, "computeAABB:")) { return false; } // ComputeAABB
			if (!ReadBool(_in, m_computeAABB)) { return false; }

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
						Component * component = AddComponent(componentID);
						const bool result = component->Load(_in);
						if (result == false) {
							Debug::Get() << Debug::Severity::error << "Failed loading component: " << component->GetName() << Debug::Endl();
						}
						if (result == false) {
							return false;
						}
					} if (!ReadEndToken(_in)) { return false; }
				}
			} if (!ReadEndToken(_in)) { return false; }
			if (!ReadSegmentHeader(_in, "Childs:")) { return false; }
			int nbChilds = -1;
			if (!ReadInteger(_in, nbChilds) || nbChilds < 0) { return false; }
			if (!ReadStartToken(_in)) { return false; }
			{
				for (int gameobjectIndex = 0; gameobjectIndex < nbChilds; gameobjectIndex++)
				{
					Gameobject * child = m_scene->CreateGameobject("tmp", this);
					child->LoadGameobject(_in);
				}
			}if (!ReadEndToken(_in)) { return false; }
		} if (!ReadEndToken(_in)) { return false; }

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::LoadGameobject(std::istream& _in) {
		const bool result = Load(_in);
		if (result == false) {
			Debug::Get() << Debug::Severity::error << "Failed loading gameobject: " << GetName() << Debug::Endl();
		}
		return result;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		const std::string indentation1 = GetIndentation(_indentLevel + 1);
		const std::string indentation2 = GetIndentation(_indentLevel + 2);

		_out << indentation << "Gameobject: " << m_name << " {" << std::endl; { // gameobject	

			_out << indentation1 << "computeAABB: " << BoolToSting(m_computeAABB) << std::endl; // m_computeAABB	

			_out << indentation1 << "Components: " << m_components.size() << " {" << std::endl; { // components
				for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
					Component * component = m_components[componentIndex];
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
					Gameobject * gameobject = m_childs[childIndex];
					if (gameobject->HasFlag(NOT_SAVED) == false) {
						gameobject->Save(_out, _indentLevel + 2);
					}
				}
			} _out << indentation1 << "}" << std::endl; // End childs
		} _out << indentation << "}" << std::endl;; // End gameobject
		return true;
	}
}