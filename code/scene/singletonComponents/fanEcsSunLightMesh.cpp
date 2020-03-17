#include "scene/singletonComponents/fanECSSunLightMesh.hpp"
#include "render/fanRendererDebug.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ecsSunLightMesh_s::ecsSunLightMesh_s()
	{
		subAngle = 45.f;
		radius = 100.f;
		debugDraw = false;
		mesh = new Mesh();

		mesh->LoadFromFile( "SolarEruptionMesh" );
		mesh->SetHostVisible( true );
		mesh->SetOptimizeVertices( false );
		mesh->SetAutoUpdateHull( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsSunLightMesh_s::~ecsSunLightMesh_s()
	{
		delete mesh;
	}

	//================================================================================================================================
	// Helper : Generates a triangle that represents a segment of a circle of radius m_radius
	//================================================================================================================================
	void ecsSunLightMesh_s::AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 )
	{
		const glm::vec3 normal( 0.f, 1.f, 0.f );
		const glm::vec3 color( 1.f, 1.f, 1.f );
		const glm::vec3 center( 0.f, 0.f, 0.f );
		const glm::vec2 centerUV( 0.5f, 0.5f );

		glm::vec3 p1( _v0[ 0 ], 0.f, _v0[ 2 ] );
		glm::vec3 p2( _v1[ 0 ], 0.f, _v1[ 2 ] );
		glm::vec2 uv1( _v0[ 0 ], _v0[ 2 ] );
		glm::vec2 uv2( _v1[ 0 ], _v1[ 2 ] );

		uv1 = 0.5f * uv1 / radius + glm::vec2( 0.5f, 0.5f );
		uv2 = 0.5f * uv2 / radius + glm::vec2( 0.5f, 0.5f );

		_vertices.push_back( { center,	normal, color,centerUV } );
		_vertices.push_back( { p1,		normal, color, uv1 } );
		_vertices.push_back( { p2,		normal, color, uv2 } );


		if ( debugDraw )
		{

			const Color debugColor( 1.f, 1.f, 0.f, 0.3f );
			RendererDebug::Get().DebugTriangle( btVector3::Zero(), _v0, _v1, debugColor );
			RendererDebug::Get().DebugLine( btVector3::Zero(), _v0, Color::Green );
		}
	}
}