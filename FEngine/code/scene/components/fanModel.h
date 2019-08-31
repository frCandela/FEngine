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
		static util::Signal< Model * > onRegisterModel;

		Model();
		~Model();

		shape::AABB ComputeAABB() const;
		bool		IsUnique()	const override { return true; }

		// ISerializable
		void					Load(std::istream& _in) override;
		void					Save(std::ostream& _out) override;
		void					SetMesh(ressource::Mesh * _mesh);
		ressource::Mesh *		GetMesh() { return m_mesh; }
		const ressource::Mesh *	GetMesh() const  { return m_mesh; }

		int		GetRenderID() const					{ return m_renderID; }
		void	SetRenderID(const int _renderID)	{ m_renderID = _renderID; }

		DECLARE_TYPE_INFO(Model);
	private:
		void Initialize() override;
		void Delete() override {};

		ressource::Mesh * m_mesh;
		int m_renderID = -1;
	};
}