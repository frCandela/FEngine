#pragma once

#include "scene/components/fanComponent.h"
#include "core/ressources/fanMesh.h"


namespace shape {class AABB;}
namespace ressource { class Mesh;  }

namespace scene
{
	class Gameobject;

	class Model : public Component
	{
	public:
		friend class RessourceManager;
		Model();
		~Model();

		shape::AABB ComputeAABB() const;
		bool			IsUnique()	const override { return true; }

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		ressource::RessourcePtr< ressource::Mesh > mesh;

		DECLARE_ABSTRACT_TYPE_INFO(Model);
	private:
		void Initialize() override;
	};
}