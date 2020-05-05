#pragma once

#include "core/fanSingleton.hpp"

namespace fan
{
	class EditorGizmos;
	class RendererDebug;

	//================================================================================================================================
	// WARNING : these globals are not available in retail
	//================================================================================================================================
	class EditorDebug : public Singleton<EditorDebug>
	{
	public:
		friend class Singleton<EditorDebug>;

		void SetDebug( RendererDebug* _renderer, EditorGizmos* _gizmos )
		{
			m_renderer = _renderer;
			m_gizmos = _gizmos;
		}

		RendererDebug& Renderer() { return *Get().m_renderer; };
		EditorGizmos& Gizmos() { return *Get().m_gizmos; };

	private:
		RendererDebug* m_renderer;
		EditorGizmos* m_gizmos;
	};
}