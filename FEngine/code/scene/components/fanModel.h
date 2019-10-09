#pragma once

#include "scene/components/fanComponent.h"
#include "renderer/fanMesh.h"
#include "core/ressources/fanRessourcePtr.h"

namespace fan
{
	class Mesh;
	class AABB;
	class Gameobject;

	//================================================================================================================================
	//================================================================================================================================
	class Model : public Component
	{
	public:
		static Signal< Model * >				onRegisterModel;
		static Signal< Model * >				onUnRegisterModel;
		static Signal< Model *, std::string  >	onModelSetPath;

		Model();

		AABB ComputeAABB() const;

		// ISerializable
		bool			Load(std::istream& _in) override;
		bool			Save(std::ostream& _out, const int _indentLevel) const override;
		void			SetMesh(Mesh * _mesh);
		Mesh *			GetMesh() { return m_mesh; }
		const Mesh *	GetMesh() const { return m_mesh; }

		int		GetRenderID() const { return m_renderID; }
		void	SetRenderID(const int _renderID) { m_renderID = _renderID; }

		void OnGui() override;
		bool IsUnique()	const override { return true; }

		DECLARE_EDITOR_COMPONENT(Model)
		DECLARE_TYPE_INFO(Model);

	protected:
		void OnDetach() override;

	private:
		Mesh * m_mesh;
		int m_renderID = -1;

		// Editor
		std::fs::path m_pathBuffer;

	};
}