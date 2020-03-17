#include "scene/systems/fanDrawDebug.hpp"

#include "scene/components/fanBounds.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "render/fanRendererDebug.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugBounds::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugBounds::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const Bounds& bounds = _world.GetComponent<Bounds>( entityID );
			const SceneNode& node = _world.GetComponent<SceneNode>( entityID );
			RendererDebug::Get().DebugAABB( bounds.aabb, Color::Red );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugNormals::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugNormals::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );

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
					RendererDebug::Get().DebugLine( position, position + 0.1f * normal, Color::Green );
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugWireframe::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugWireframe::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );

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
					RendererDebug::Get().DebugLine( v0, v1, Color::Yellow );
					RendererDebug::Get().DebugLine( v1, v2, Color::Yellow );
					RendererDebug::Get().DebugLine( v2, v0, Color::Yellow );
				}				
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugHull::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugHull::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );

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

						RendererDebug::Get().DebugLine( worldVec0, worldVec1, color );
						RendererDebug::Get().DebugLine( worldVec1, worldVec2, color );
						RendererDebug::Get().DebugLine( worldVec2, worldVec0, color );

					}
				}

			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugPointLights::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<PointLight>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugPointLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const PointLight& light = _world.GetComponent<PointLight>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );

			const float lightRange = PointLight::GetLightRange( light );
			if( lightRange > 0 )
			{
				RendererDebug::Get().DebugSphere( transform.transform, lightRange, 2, light.diffuse );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugDirectionalLights::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<DirectionalLight>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugDirectionalLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		for( EntityID entityID : _entities )
		{
			const DirectionalLight& light = _world.GetComponent<DirectionalLight>( entityID );
			const Transform& transform = _world.GetComponent<Transform>( entityID );

			const btVector3 pos = transform.GetPosition();
			const btVector3 dir = transform.Forward();
			const btVector3 up = transform.Up();
			const btVector3 left = transform.Left();
			const float length = 2.f;
			const float radius = 0.5f;
			const Color color = Color::Yellow;
			btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
			{
				const btVector3 offset = offsets[offsetIndex];
				RendererDebug::Get().DebugLine( pos + offset, pos + offset + length * dir, color );
			}
			RendererDebug::Get().DebugSphere( transform.transform, radius, 0, color );
		}
	}
}