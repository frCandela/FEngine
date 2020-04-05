#include "scene/systems/fanGenerateParticles.hpp"

#include "render/fanMesh.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_GenerateParticles::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Particle>() | _world.AliveSignature();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_GenerateParticles::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		std::vector<Vertex> vertices;
		vertices.resize( _entities.size() * 3 );
		const float size = 0.05f;

		for( EntityID entityID : _entities )
		{
			Particle& particle = _world.GetComponent<Particle>( entityID );
			glm::vec3 color = particle.color.ToGLM3();
			// pos, normal, color, uv;
			vertices.push_back( { particle.position + glm::vec3( -size,  0.0f, -size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
			vertices.push_back( { particle.position + glm::vec3( 0, 0.0f, size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
			vertices.push_back( { particle.position + glm::vec3( size, 0.0f, -size ), glm::vec3( 0.f, 1.f, 0.f ), color, glm::vec2( -0.5f, -0.5f ) } );
		}
		renderWorld.particlesMesh.LoadFromVertices( vertices );
	}
}