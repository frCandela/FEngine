#include "fanSunLight.hpp"

#include "engine/fanEngineSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SunLight::SetInfo( EcsSingletonInfo& _info )
	{
		_info.save   = &SunLight::Save;
		_info.load   = &SunLight::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void SunLight::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		SunLight& sunLight = static_cast<SunLight&>( _component );
		sunLight.mSubAngle                = 45.f;
		sunLight.mRadius                  = 100.f;
		sunLight.mMesh                    = new Mesh();
		sunLight.mMesh->mHostVisible      = true;
		sunLight.mMesh->mOptimizeVertices = false;
		sunLight.mMesh->mAutoUpdateHull   = false;
	}
	
	//========================================================================================================
	//========================================================================================================
	void SunLight::Save( const EcsSingleton& _component, Json& _json )
	{
		const SunLight& sunLight = static_cast<const SunLight&>( _component );
		Serializable::SaveFloat( _json, "sub_angle", sunLight.mSubAngle );
		Serializable::SaveFloat( _json, "radius", sunLight.mRadius );
	}

	//========================================================================================================
	//========================================================================================================
	void SunLight::Load( EcsSingleton& _component, const Json& _json )
	{
		SunLight& sunLight = static_cast<SunLight&>( _component );
		Serializable::LoadFloat( _json, "sub_angle", sunLight.mSubAngle );
		Serializable::LoadFloat( _json, "radius", sunLight.mRadius );
	}

	//========================================================================================================
	// Helper : Generates a triangle that represents a segment of a circle of radius m_radius
	//========================================================================================================
	void SunLight::AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1)
	{
		const glm::vec3 normal( 0.f, 1.f, 0.f );
		const glm::vec3 color( 1.f, 1.f, 1.f );
		const glm::vec3 center( 0.f, 0.f, 0.f );
		const glm::vec2 centerUV( 0.5f, 0.5f );

		glm::vec3 p1( _v0[0], 0.f, _v0[2] );
		glm::vec3 p2( _v1[0], 0.f, _v1[2] );
		glm::vec2 uv1( _v0[0], _v0[2] );
		glm::vec2 uv2( _v1[0], _v1[2] );

		uv1 = 0.5f * uv1 / mRadius + glm::vec2( 0.5f, 0.5f );
		uv2 = 0.5f * uv2 / mRadius + glm::vec2( 0.5f, 0.5f );

		_vertices.push_back( { center,	normal, color,centerUV } );
		_vertices.push_back( { p1,		normal, color, uv1 } );
		_vertices.push_back( { p2,		normal, color, uv2 } );
	}
}