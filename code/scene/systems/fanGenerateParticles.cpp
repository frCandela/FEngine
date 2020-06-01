#include "scene/systems/fanGenerateParticles.hpp"

#include "render/fanMesh.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/time/fanProfiler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_GenerateParticles::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Particle>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_GenerateParticles::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		SCOPED_PROFILE( S_GenParticles );
		if( _delta == 0.f ) { return; }

		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		std::vector<Vertex> vertices;
		vertices.resize( _view.Size() * 3 );
		const float size = 0.05f;
		
		for( auto particleIt = _view.begin<Particle>(); particleIt != _view.end<Particle>(); ++particleIt )
		{
			Particle& particle = *particleIt;

			glm::vec3 color = particle.color.ToGLM3();
			// pos, normal, color, uv;
			vertices.push_back( { particle.position + glm::vec3( -size,  0.0f, -size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
			vertices.push_back( { particle.position + glm::vec3( 0, 0.0f, size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
			vertices.push_back( { particle.position + glm::vec3( size, 0.0f, -size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
		}
		renderWorld.particlesMesh.LoadFromVertices( vertices );
	}
}