#pragma once

#include "scene/components/fanComponent.h"
#include "core/ressources/fanRessourcePtr.h"

namespace fan
{
	class Mesh;
	class AABB;
	class Gameobject;
	struct ecsModel;

	//================================================================================================================================
	//================================================================================================================================
	class Model : public Component
	{
	public:
		static Signal< Model * >				onRegisterModel;
		static Signal< Model * >				onUnRegisterModel;
		static Signal< Model *, std::string  >	onModelSetPath;

		AABB ComputeAABB() const;

		// ISerializable
		bool			Load(std::istream& _in) override;
		bool			Save(std::ostream& _out, const int _indentLevel) const override;
		void			SetPath( std::string _path );
		void			SetMesh(Mesh * _mesh);
		Mesh *			GetMesh();
		const Mesh *	GetMesh() const;

		int		GetRenderID() const;
		void	SetRenderID(const int _renderID);

		void OnGui() override;
		bool IsUnique()	const override { return true; }

		DECLARE_EDITOR_COMPONENT(Model)
		DECLARE_TYPE_INFO(Model);

	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		// Editor
		std::fs::path m_pathBuffer;

		ecsModel * GetEcsModel() const;
	};
}