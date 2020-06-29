#include "ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Draw the bounds of all scene nodes 
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto boundsIt = _view.begin<Bounds>();
			auto sceneNodeIt = _view.begin<SceneNode>();
			for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt )
			{
				const Bounds& bounds = *boundsIt;
				_world.GetSingleton<RenderDebug>().DebugAABB( bounds.aabb, Color::Red );
			}
		}
	};

	//==============================================================================================================================================================
	// Draw the normals of all models 
	//==============================================================================================================================================================
	struct S_DrawDebugNormals : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto meshRendererIt = _view.begin<MeshRenderer>();
			auto transformIt = _view.begin<Transform>();
			for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
			{
				const MeshRenderer& meshRenderer = *meshRendererIt;
				const Transform& transform = *transformIt;

				if( *meshRenderer.mesh != nullptr )
				{
					const glm::mat4  modelMat = transform.GetModelMatrix();
					const glm::mat4  normalMat = transform.GetNormalMatrix();
					const std::vector<uint32_t>& indices = meshRenderer.mesh->GetIndices();
					const std::vector<Vertex>& vertices = meshRenderer.mesh->GetVertices();

					for( int index = 0; index < indices.size(); index++ )
					{
						const Vertex& vertex = vertices[indices[index]];
						const btVector3 position = ToBullet( modelMat * glm::vec4( vertex.pos, 1.f ) );
						const btVector3 normal = ToBullet( normalMat * glm::vec4( vertex.normal, 1.f ) );
						_world.GetSingleton<RenderDebug>().DebugLine( position, position + 0.1f * normal, Color::Green );
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Draw all the models in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugWireframe : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto meshRendererIt = _view.begin<MeshRenderer>();
			auto transformIt = _view.begin<Transform>();
			for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
			{
				const MeshRenderer& meshRenderer = *meshRendererIt;
				const Transform& transform = *transformIt;

				if( *meshRenderer.mesh != nullptr )
				{
					const glm::mat4  modelMat = transform.GetModelMatrix();
					const std::vector<uint32_t>& indices = meshRenderer.mesh->GetIndices();
					const std::vector<Vertex>& vertices = meshRenderer.mesh->GetVertices();

					for( int index = 0; index < indices.size() / 3; index++ )
					{
						const btVector3 v0 = ToBullet( modelMat * glm::vec4( vertices[indices[3 * index + 0]].pos, 1.f ) );
						const btVector3 v1 = ToBullet( modelMat * glm::vec4( vertices[indices[3 * index + 1]].pos, 1.f ) );
						const btVector3 v2 = ToBullet( modelMat * glm::vec4( vertices[indices[3 * index + 2]].pos, 1.f ) );
						_world.GetSingleton<RenderDebug>().DebugLine( v0, v1, Color::Yellow );
						_world.GetSingleton<RenderDebug>().DebugLine( v1, v2, Color::Yellow );
						_world.GetSingleton<RenderDebug>().DebugLine( v2, v0, Color::Yellow );
					}
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Draw all the models convex hull in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugHull : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto meshRendererIt = _view.begin<MeshRenderer>();
			auto transformIt = _view.begin<Transform>();
			for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
			{
				const MeshRenderer& meshRenderer = *meshRendererIt;
				const Transform& transform = *transformIt;

				if( *meshRenderer.mesh != nullptr )
				{
					const ConvexHull& hull = meshRenderer.mesh->GetHull();
					const std::vector<btVector3>& vertices = hull.GetVertices();
					const std::vector<uint32_t>& indices = hull.GetIndices();
					if( !vertices.empty() )
					{
						const glm::mat4  modelMat = transform.GetModelMatrix();

						Color color = Color::Cyan;
						for( unsigned polyIndex = 0; polyIndex < indices.size() / 3; polyIndex++ )
						{
							const int index0 = indices[3 * polyIndex + 0];
							const int index1 = indices[3 * polyIndex + 1];
							const int index2 = indices[3 * polyIndex + 2];
							const btVector3 vec0 = vertices[index0];
							const btVector3 vec1 = vertices[index1];
							const btVector3 vec2 = vertices[index2];
							const btVector3 worldVec0 = ToBullet( modelMat * glm::vec4( vec0[0], vec0[1], vec0[2], 1.f ) );
							const btVector3 worldVec1 = ToBullet( modelMat * glm::vec4( vec1[0], vec1[1], vec1[2], 1.f ) );
							const btVector3 worldVec2 = ToBullet( modelMat * glm::vec4( vec2[0], vec2[1], vec2[2], 1.f ) );

							_world.GetSingleton<RenderDebug>().DebugLine( worldVec0, worldVec1, color );
							_world.GetSingleton<RenderDebug>().DebugLine( worldVec1, worldVec2, color );
							_world.GetSingleton<RenderDebug>().DebugLine( worldVec2, worldVec0, color );
						}
					}

				}
			}
		}
	};


	struct Transform;
	struct PointLight;
	//==============================================================================================================================================================
	// Draw all the point lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugPointLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<PointLight>() | _world.GetSignature<Transform>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto lightIt = _view.begin<PointLight>();
			auto transformIt = _view.begin<Transform>();
			RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
			for( ; lightIt != _view.end<PointLight>(); ++lightIt, ++transformIt )
			{
				const PointLight& light = *lightIt;
				const Transform& transform = *transformIt;
				DrawPointLight( renderDebug, transform, light );
			}
		}

		static void DrawPointLight( RenderDebug& _renderDebug, const Transform& _transform, const PointLight& _light )
		{
			const float lightRange = PointLight::GetLightRange( _light );
			if( lightRange > 0 )
			{
				_renderDebug.DebugSphere( _transform.transform, lightRange, _light.diffuse );
			}
		}
	};


	struct DirectionalLight;
	//==============================================================================================================================================================
	// Draw all the directional lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugDirectionalLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<DirectionalLight>() | _world.GetSignature<Transform>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto lightIt = _view.begin<DirectionalLight>();
			auto transformIt = _view.begin<Transform>();
			RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
			for( ; lightIt != _view.end<DirectionalLight>(); ++lightIt, ++transformIt )
			{
				const DirectionalLight& light = *lightIt;
				const Transform& transform = *transformIt;
				DrawDirectionalLight( renderDebug, transform, light );
			}
		}

		static void DrawDirectionalLight( RenderDebug& _renderDebug, const Transform& _transform, const DirectionalLight& /*_light*/ )
		{
			const btVector3 pos = _transform.GetPosition();
			const btVector3 dir = _transform.Forward();
			const btVector3 up = _transform.Up();
			const btVector3 left = _transform.Left();
			const float length = 2.f;
			const float radius = 0.5f;
			const Color color = Color::Yellow;
			btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
			{
				const btVector3 offset = offsets[offsetIndex];
				_renderDebug.DebugLine( pos + offset, pos + offset + length * dir, color, false );
			}
			_renderDebug.DebugIcoSphere( _transform.transform, radius, 0, color, false );
		}
	};

	//==============================================================================================================================================================
	// @todo split this in two systems for BoxShape & SphereShape
	// Draw physics shapes in wireframe (box, sphere, etc )
	//==============================================================================================================================================================
	struct S_DrawDebugCollisionShapes : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<Transform>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			auto transformIt = _view.begin<Transform>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt )
			{
				const EcsEntity entity = transformIt.GetEntity();
				DrawCollisionShape( _world, entity );
			}
		}

		static void DrawCollisionShape( EcsWorld& _world, EcsEntity _entity )
		{
			if( !_world.HasComponent<Transform>( _entity ) ) { return; }

			const Transform& transform = _world.GetComponent<Transform>( _entity );

			// box shape
			if( _world.HasComponent<BoxShape>( _entity ) )
			{
				const BoxShape& shape = _world.GetComponent<BoxShape>( _entity );
				_world.GetSingleton<RenderDebug>().DebugCube( transform.transform, 0.5f * shape.GetScaling(), Color::Green, false );
			}

			// sphere shape
			if( _world.HasComponent<SphereShape>( _entity ) )
			{
				const SphereShape& shape = _world.GetComponent<SphereShape>( _entity );
				_world.GetSingleton<RenderDebug>().DebugSphere( transform.transform, shape.GetRadius(), Color::Green, false );
			}
		}
	};
}