#pragma once

#include "scene/components/fanComponent.h"
#include "core/ressources/fanRessourcePtr.h"

namespace fan
{
	class Mesh;
	class AABB;
	class Gameobject;
	struct ecsMesh;

	//================================================================================================================================
	//================================================================================================================================
	class Model : public Component
	{
	public:
		static Signal< Model * >				onRegisterModel;
		static Signal< Model * >				onUnRegisterModel;
		static Signal< Model *, std::string  >	onModelSetPath;

		void			SetPath( std::string _path );
		void			SetMesh(Mesh * _mesh);
		Mesh *			GetMesh();
		const Mesh *	GetMesh() const;
		ConvexHull &	GetConvexHull() const;

		int		GetRenderID() const;
		void	SetRenderID(const int _renderID);

		void SetAutoUpdateHull( const bool _autoUpdateHull ) { m_autoUpdateHull = _autoUpdateHull; }

		void OnGui() override;
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::MODEL; };

		DECLARE_EDITOR_COMPONENT(Model)
		DECLARE_TYPE_INFO(Model, Component );

	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		// Editor
		std::fs::path m_pathBuffer;

		bool m_autoUpdateHull = true;

		ecsMesh *		const m_mesh = nullptr;
		ecsConvexHull * const m_convexHull = nullptr;
	};
}