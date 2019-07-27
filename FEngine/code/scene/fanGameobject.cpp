#include "fanIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanMesh.h"
#include "scene/components/fanTransform.h"
#include "core/fanSignal.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Gameobject::Gameobject(const std::string _name) :
		m_name(_name)
		, m_flags( Flag::NONE )
	{
		onComponentModified.Connect(&Gameobject::OnComponentModified, this);
		onComponentDeleted.Connect(&Gameobject::OnComponentDeleted, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject::~Gameobject() {
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			m_components[componentIndex]->Delete();
		} m_components.clear();
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
				onComponentDeleted.Emmit( m_components[componentIndex] );
				m_components[componentIndex]->Delete();
				m_components.erase(m_components.begin() + componentIndex);
				return true;
			}
		}
		return false;
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::AddComponent(scene::Component * _component) {
		_component->m_gameobject = this;
		_component->Initialize();
		m_components.push_back(_component);
		onComponentCreated.Emmit(_component);
		onComponentModified.Emmit(_component);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::ComputeAABB() {

		const scene::Mesh * mesh = GetComponent< scene::Mesh >();
		if ( mesh != nullptr && mesh->IsBeingDeleted() == false && mesh->GetIndices().size() > 0) {
			m_aabb = mesh->ComputeAABB();
		} else {
			const btVector3 origin = GetComponent< scene::Transform >()->GetPosition();
			const float size = 0.05f;
			m_aabb = shape::AABB( origin - size * btVector3::One(), origin + size * btVector3::One() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::OnComponentModified(scene::Component * _component) {	
		if (_component->IsType<scene::Transform>() == true || _component->IsType<scene::Mesh>()) {
			ComputeAABB();
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::OnComponentDeleted(scene::Component * _component) {
		if ( _component->IsType<scene::Mesh>()) {
			ComputeAABB();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::Load(std::istream& _in) {
		std::string buffer;
		_in >> buffer;
		while (buffer != "end") {			

			// Get component id
			uint32_t componentID;
			_in >> componentID;

			std::cout << "\tComponent: " << buffer << std::endl;

			// instanciate component
			scene::Component * component = TypeInfo::Instantiate<Component>(componentID);
			AddComponent(component);
			component->Load(_in);
			_in >> buffer; // skip component name
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Gameobject::Save(std::ostream& _out) {
		_out << "gameobject " << m_name << std::endl;
		for (int componentIndex = 0; componentIndex < m_components.size() ; componentIndex++) {
			_out << "\t" << m_components[componentIndex]->GetName() << " ";
			_out << m_components[componentIndex]->GetType() << std::endl;
			m_components[componentIndex]->Save(_out);
		}
		_out << "end" << std::endl;
	}
}