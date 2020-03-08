#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/singletonComponents/fanEcsSunLightMesh.hpp"
#include "core/meta/fanTuple.hpp"

namespace fan
{
	//================================
	//================================
	using ecsSingletonComponentsList = meta::TypeList
		< 	ecsSunLightMesh_s
		, ecsCameraPosition_s
		, ecsParticlesMesh_s
		>;

	//================================================================================================================================
	// Contains all ecs singleton components in a continuous tuple
	// There should be one EcsSingletonComponentContainer per EcsManager
	// Singleton components are accessible from systems
	//================================================================================================================================
	class ecsSingletonComponents
	{
	public:
		template < typename _ComponentType >
		_ComponentType& GetComponent() { return m_components.Get<_ComponentType>(); }

	private:
		meta::Tuple<ecsSingletonComponentsList> m_components;
	};
}