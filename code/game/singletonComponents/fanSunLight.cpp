#include "game/singletonComponents/fanSunLight.hpp"

#include "render/fanRendererDebug.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( SunLight );

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::SUN16;
		_info.init = &SunLight::Init;
		_info.onGui = &SunLight::OnGui;
		_info.save = &SunLight::Save;
		_info.load = &SunLight::Load;
		_info.name = "sun light";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		SunLight& sunLight = static_cast<SunLight&>( _component );

		sunLight.subAngle = 45.f;
		sunLight.radius = 100.f;

		if( !sunLight.mesh.GetVertices().empty() )
		{
			sunLight.mesh = Mesh();
		}
		sunLight.mesh.SetHostVisible( true );
		sunLight.mesh.SetOptimizeVertices( false );
		sunLight.mesh.SetAutoUpdateHull( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		SunLight& sunLight = static_cast<SunLight&>( _component );
		
		ImGui::Indent();ImGui::Indent();
		{
			ImGui::DragFloat( "sub angle", &sunLight.subAngle, 1.f, 0.f, 90.f );
			ImGui::DragFloat( "radius", &sunLight.radius, 1.f, 1.f, 1000.f );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void SunLight::Save( const EcsSingleton& _component, Json& _json )
	{
		const SunLight& sunLight = static_cast<const SunLight&>( _component );
		Serializable::SaveFloat( _json, "sub_angle", sunLight.subAngle );
		Serializable::SaveFloat( _json, "radius", sunLight.radius );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::Load( EcsSingleton& _component, const Json& _json )
	{
		SunLight& sunLight = static_cast<SunLight&>( _component );
		Serializable::LoadFloat( _json, "sub_angle", sunLight.subAngle );
		Serializable::LoadFloat( _json, "radius", sunLight.radius );
	}

	//================================================================================================================================
	// Helper : Generates a triangle that represents a segment of a circle of radius m_radius
	//================================================================================================================================
	void SunLight::AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 )
	{
		const glm::vec3 normal( 0.f, 1.f, 0.f );
		const glm::vec3 color( 1.f, 1.f, 1.f );
		const glm::vec3 center( 0.f, 0.f, 0.f );
		const glm::vec2 centerUV( 0.5f, 0.5f );

		glm::vec3 p1( _v0[0], 0.f, _v0[2] );
		glm::vec3 p2( _v1[0], 0.f, _v1[2] );
		glm::vec2 uv1( _v0[0], _v0[2] );
		glm::vec2 uv2( _v1[0], _v1[2] );

		uv1 = 0.5f * uv1 / radius + glm::vec2( 0.5f, 0.5f );
		uv2 = 0.5f * uv2 / radius + glm::vec2( 0.5f, 0.5f );

		_vertices.push_back( { center,	normal, color,centerUV } );
		_vertices.push_back( { p1,		normal, color, uv1 } );
		_vertices.push_back( { p2,		normal, color, uv2 } );
	}
}