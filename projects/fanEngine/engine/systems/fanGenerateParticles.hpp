#include "core/ecs/fanEcsSystem.hpp"
#include "core/time/fanProfiler.hpp"
#include "engine/components/fanParticle.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Generate a mesh for all particles in the world
    //==================================================================================================================================================================================================
    struct SGenerateParticles : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Particle>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
        {
            SCOPED_PROFILE( S_GenParticles );
            if( _delta == 0.f ){ return; }

            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
            Mesh       & mesh        = *renderWorld.mParticlesMesh;
            mesh.mVertices.resize( _view.Size() * 3 );
            const float size = 0.05f;

            for( auto particleIt = _view.begin<Particle>(); particleIt != _view.end<Particle>(); ++particleIt )
            {
                Particle& particle = *particleIt;

                glm::vec3 color = particle.mColor.ToGLM3();
                // pos, normal, color, uv;
                mesh.mVertices.push_back( { particle.mPosition + glm::vec3( -size, 0.0f, -size ),
                                            glm::vec3( 0.f, 1.f, 0.f ),
                                            color,
                                            glm::vec2( -0.5f, -0.5f ) } );
                mesh.mVertices.push_back( { particle.mPosition + glm::vec3( 0, 0.0f, size ),
                                            glm::vec3( 0.f, 1.f, 0.f ),
                                            color,
                                            glm::vec2( -0.5f, -0.5f ) } );
                mesh.mVertices.push_back( { particle.mPosition + glm::vec3( size, 0.0f, -size ),
                                            glm::vec3( 0.f, 1.f, 0.f ),
                                            color,
                                            glm::vec2( -0.5f, -0.5f ) } );
            }
            mesh.LoadFromVertices();
        }
    };
}