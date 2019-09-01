#pragma once

#include "scene/components/fanComponent.h"
#include "renderer/fanMesh.h"
#include "core/ressources/fanRessourcePtr.h"

namespace fan
{
	class Mesh;
	namespace shape { class AABB; }
	namespace scene
	{
		class Gameobject;

		//================================================================================================================================
		//================================================================================================================================
		class Model : public Component
		{
		public:
			static fan::Signal< Model * > onRegisterModel;
			static fan::Signal< Model * > onUnRegisterModel;

			Model();

			shape::AABB ComputeAABB() const;
			bool		IsUnique()	const override { return true; }

			// ISerializable
			void					Load(std::istream& _in) override;
			void					Save(std::ostream& _out) override;
			void					SetMesh(fan::Mesh * _mesh);
			fan::Mesh *		GetMesh() { return m_mesh; }
			const fan::Mesh *	GetMesh() const { return m_mesh; }

			int		GetRenderID() const { return m_renderID; }
			void	SetRenderID(const int _renderID) { m_renderID = _renderID; }

			DECLARE_TYPE_INFO(Model);
		private:
			void Initialize() override;
			void Delete() override;

			fan::Mesh * m_mesh;
			int m_renderID = -1;
		};
	}
}