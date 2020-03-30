#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	// SunLight is a mesh generated from the position/scale of the planets to simulate light 
	// -subAngle is the minimal angle between two radial segments of the mesh
	//================================================================================================================================
	class SunLight : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		Mesh  mesh;
		float subAngle;
		float radius;
		bool  debugDraw;

		void AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 );
	};
}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SunLight::OnGui()
// 	{
// // 		// Get singleton components 
// // 		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();
// // 
// // 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
// // 		{
// // 			// Mesh generation
// // 			ImGui::Checkbox( "debug draw", &sunLight.debugDraw );
// // 			ImGui::DragFloat( "radius", &sunLight.radius, 0.1f );
// // 			ImGui::DragFloat( "sub-angle", &sunLight.subAngle, 1.f, 1.f, 180.f );
// // 		} ImGui::PopItemWidth();
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool SunLight::Load( const Json& _json )
// 	{
// // 		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();
// // 
// // 		Actor::Load( _json );
// // 		Serializable::LoadFloat( _json, "radius", sunLight.radius );
// // 		Serializable::LoadFloat( _json, "sub_angle", sunLight.subAngle );
// 
// 		return true;
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	bool SunLight::Save( Json& _json ) const
// 	{
// // 		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();
// // 
// // 		Actor::Save( _json );
// // 		Serializable::SaveFloat( _json, "radius", sunLight.radius );
// // 		Serializable::SaveFloat( _json, "sub_angle", sunLight.subAngle );
// 		return true;
// 	}
