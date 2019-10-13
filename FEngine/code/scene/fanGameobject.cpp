#include "fanGlobalIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/fanScene.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"

#include "renderer/fanMesh.h"
#include "ecs/fanECSManager.h"
#include "core/fanSignal.h"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Gameobject::Gameobject(const std::string _name, Gameobject * _parent, Scene * _scene ) :
		m_name(_name)
		, m_flags(Flag::NONE)
		, m_parent(_parent)
		, m_scene( _scene )
	{

		if (_parent != nullptr) {
			_parent->m_childs.push_back(this);
		}

		ecsEntity entity = m_scene->GetEcsManager()->CreateEntity();
		m_ecsHandleEntity = m_scene->GetEcsManager()->CreateHandle( entity );
		AddEcsComponent<ecsAABB>();
		m_transform = AddComponent<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject::~Gameobject() {
		// Delete components
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			m_components[componentIndex]->OnDetach();
		}
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			delete m_components[componentIndex];
		}
		m_components.clear();

		// Delete ecs entity
		ecsEntity entity;
		if ( m_scene->GetEcsManager()->FindEntity( m_ecsHandleEntity, entity ) ) {
			m_scene->GetEcsManager()->DeleteEntity( entity );
		} else {
			Debug::Get() << Debug::Severity::warning << "Unable to destroy ecsEntity for gameobject " << m_name << Debug::Endl();
		}
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
	// Remove the component from the gameobject and deletes it			
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
	// Add component with id
	//================================================================================================================================
	Component* Gameobject::AddComponent(const uint32_t _componentID) {
		Component * component = TypeInfo::Instantiate<Component>(_componentID);
		AddComponent(component);
		return component;
	}

	//================================================================================================================================
	// Private method used to factorize add components methods 
	//================================================================================================================================
	void Gameobject::AddComponent(Component * _component) {
		_component->m_gameobject = this;
		m_components.push_back(_component);
		_component->OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	const AABB & Gameobject::GetAABB() const {
		return GetEcsComponent<ecsAABB>()->aabb;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::ComputeAABB() {
		if (m_computeAABB) {
			const Model * model = GetComponent< Model >();
			if (model != nullptr && model->IsBeingDeleted() == false && model->GetMesh() != nullptr && model->GetMesh()->GetIndices().size() > 0) {
				GetEcsComponent<ecsAABB>()->aabb = model->ComputeAABB();
			}
			else {
				const Transform * transform = GetComponent< Transform >();
				if (transform != nullptr) {
					const btVector3 origin = transform->GetPosition();
					const float size = 0.05f;
					GetEcsComponent<ecsAABB>()->aabb = AABB(origin - size * btVector3::One(), origin + size * btVector3::One());
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
	bool Gameobject::Load( Json & _json ) {

		LoadString( _json, "name", m_name );
		LoadBool( _json, "compute_aabb", m_computeAABB );

		Json& jComponents = _json["components"]; {
			for ( int childIndex = 0; childIndex < jComponents.size(); childIndex++ ) {
				Json& jComponent_i = jComponents[childIndex]; {
					unsigned componentID = 0;
					LoadUInt( jComponent_i, "id", componentID );

					// Don't add a transform two times
					Component * component = nullptr;
					if ( componentID == Transform::s_typeID ) {
						component = GetComponent<Transform>();
					} else {
						component = AddComponent( componentID );
					}
					if ( ! component->Load( jComponent_i ) ) {
						Debug::Get() << Debug::Severity::error << "Failed loading component: " << component->GetName() << Debug::Endl();
					}
				}
			}
		}
		Json& jchilds = _json["childs"]; {
			for (int childIndex = 0; childIndex < jchilds.size(); childIndex++)	{
				Json& jchild_i = jchilds[childIndex]; {
					Gameobject * child = m_scene->CreateGameobject( "tmp", this );
					child->Load( jchild_i );
				}
			}
		}
 		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Gameobject::Save( Json & _json ) const {
		SaveString( _json, "name", m_name );
		SaveBool( _json, "compute_aabb", m_computeAABB );

		Json& jComponents = _json["components"];{
			for ( int componentIndex = 0; componentIndex < m_components.size(); componentIndex++ ) {
				Json& jComponent_i = jComponents[componentIndex]; {
					Component * component = m_components[componentIndex];
					component->Save( jComponent_i );
				}				
			}
		}
		Json& jchilds = _json["childs"]; {
			// Count childs to save
			unsigned childIndex = 0;				
			for ( int gameobjectIndex = 0; gameobjectIndex < m_childs.size(); gameobjectIndex++ ) {
				Gameobject * gameobject = m_childs[gameobjectIndex];
				if ( gameobject->HasFlag( NOT_SAVED ) == false ) {
					Json& jchild_i = jchilds[childIndex]; {
						gameobject->Save( jchild_i );
					}
					++childIndex;
				}
			}
		}
		return true;
	}
}