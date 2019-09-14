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
		class Entity;

		//================================================================================================================================
		//================================================================================================================================
		class Model : public Component
		{
		public:
			static fan::Signal< Model * > onRegisterModel;
			static fan::Signal< Model * > onUnRegisterModel;

			Model();

			shape::AABB ComputeAABB() const;

			// ISerializable
			bool				Load(std::istream& _in) override;
			bool				Save(std::ostream& _out, const int _indentLevel) const override;
			void				SetMesh(fan::Mesh * _mesh);
			fan::Mesh *			GetMesh() { return m_mesh; }
			const fan::Mesh *	GetMesh() const { return m_mesh; }

			int		GetRenderID() const { return m_renderID; }
			void	SetRenderID(const int _renderID) { m_renderID = _renderID; }

			void OnGui() override;
			bool IsUnique()	const override { return true; }

			DECLARE_EDITOR_COMPONENT(Model)
			DECLARE_TYPE_INFO(Model);

		protected:
			void OnDetach() override;

		private:
			fan::Mesh * m_mesh;
			int m_renderID = -1;

			// Editor
			std::experimental::filesystem::path m_pathBuffer;

		};
	}
}