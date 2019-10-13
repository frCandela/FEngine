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

		void			SetPath( std::string _path );
		void			SetMesh(Mesh * _mesh);
		Mesh *			GetMesh();
		const Mesh *	GetMesh() const;

		int		GetRenderID() const;
		void	SetRenderID(const int _renderID);

		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(Model)
		DECLARE_TYPE_INFO(Model);

	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		// Editor
		std::fs::path m_pathBuffer;

		ecsModel * GetEcsModel() const;
	};
}