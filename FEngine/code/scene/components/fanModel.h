#pragma once

#include "scene/components/fanComponent.h"
#include "core/ressources/fanMesh.h"
#include "core/ressources/fanRessourcePtr.h"

namespace shape {class AABB;}
namespace ressource { class Mesh;  }

namespace scene
{
	class Gameobject;

	class Model : public Component
	{
	public:

		Model();
		~Model();

		shape::AABB ComputeAABB() const;
		bool		IsUnique()	const override { return true; }

		// ISerializable
		void				Load(std::istream& _in) override;
		void				Save(std::ostream& _out) override;
		void				SetMesh(ressource::Mesh * _mesh);
		ressource::Mesh *	GetMesh() { return m_mesh; }
		const ressource::Mesh *	GetMesh() const  { return m_mesh; }

		static util::Signal< Model * > onRegisterModel;

		DECLARE_TYPE_INFO(Model);
	private:
		void Initialize() override;

		ressource::Mesh * m_mesh;
	};
}