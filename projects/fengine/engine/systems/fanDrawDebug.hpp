#include "core/ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanBoxShape.hpp"
#include "engine/components/fanSphereShape.hpp"
#include "engine/components/ui/fanUITransform.hpp"
#include "engine/components/ui/fanUIRenderer.hpp"
#include "engine/components/ui/fanUILayout.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/fanSceneTags.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"

namespace fan
{
	class EcsWorld;
    struct DirectionalLight;

	//========================================================================================================
	// Draw the bounds of all scene nodes 
	//========================================================================================================
	struct SDrawDebugBounds : EcsSystem
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
				_world.GetSingleton<RenderDebug>().DebugAABB( bounds.mAabb, Color::sRed );
			}
		}
	};

	//========================================================================================================
	// Draw the normals of all models 
	//========================================================================================================
	struct SDrawDebugNormals : EcsSystem
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

				if( *meshRenderer.mMesh != nullptr )
				{
					const glm::mat4  modelMat = transform.GetModelMatrix();
					const glm::mat4  normalMat = transform.GetNormalMatrix();
					const std::vector<uint32_t>& indices = meshRenderer.mMesh->mIndices;
					const std::vector<Vertex>& vertices = meshRenderer.mMesh->mVertices;

					for( int index = 0; index < (int)indices.size(); index++ )
					{
						const Vertex& vertex = vertices[indices[index]];
						const btVector3 position = ToBullet( modelMat * glm::vec4( vertex.mPos, 1.f ) );
						const btVector3 normal = ToBullet( normalMat * glm::vec4( vertex.mNormal, 1.f ) );
                        RenderDebug & renderDebug = _world.GetSingleton<RenderDebug>();
                        renderDebug.DebugLine( position, position + 0.1f * normal, Color::sGreen );
					}
				}
			}
		}
	};

	//========================================================================================================
	// Draw all the models in wireframe
	//========================================================================================================
	struct SDrawDebugWireframe : EcsSystem
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
				if( *meshRenderer.mMesh == nullptr ) { continue; }

				const glm::mat4  modelMat = transform.GetModelMatrix();
				const std::vector<uint32_t>& indices = meshRenderer.mMesh->mIndices;
				const std::vector<Vertex>& vertices = meshRenderer.mMesh->mVertices;

				for( int index = 0; index < (int)indices.size() / 3; index++ )
                {
                    const btVector3 v0 = ToBullet(modelMat * glm::vec4( vertices[indices[3 * index + 0]].mPos, 1.f ) );
                    const btVector3 v1 = ToBullet(modelMat * glm::vec4( vertices[indices[3 * index + 1]].mPos, 1.f ) );
                    const btVector3 v2 = ToBullet(modelMat * glm::vec4( vertices[indices[3 * index + 2]].mPos, 1.f ) );
					_world.GetSingleton<RenderDebug>().DebugLine( v0, v1, Color::sYellow );
					_world.GetSingleton<RenderDebug>().DebugLine( v1, v2, Color::sYellow );
					_world.GetSingleton<RenderDebug>().DebugLine( v2, v0, Color::sYellow );
				}

			}
		}
	};

	//========================================================================================================
	// Draw all the models convex hull in wireframe
	//========================================================================================================
	struct SDrawDebugHull : EcsSystem
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

				if( *meshRenderer.mMesh == nullptr ){ continue; }

				const ConvexHull& hull = meshRenderer.mMesh->mConvexHull;
				if( hull.mVertices.empty() ){ continue; }

				const glm::mat4  modelMat = transform.GetModelMatrix();

				Color color = Color::sCyan;
				for( unsigned polyIndex = 0; polyIndex < hull.mIndices.size() / 3; polyIndex++ )
				{
					const int index0 = hull.mIndices[3 * polyIndex + 0];
					const int index1 = hull.mIndices[3 * polyIndex + 1];
					const int index2 = hull.mIndices[3 * polyIndex + 2];
					const btVector3 vec0 = hull.mVertices[index0];
					const btVector3 vec1 = hull.mVertices[index1];
					const btVector3 vec2 = hull.mVertices[index2];
					const btVector3 worldVec0 = ToBullet( modelMat * glm::vec4( vec0[0], vec0[1], vec0[2], 1.f ) );
					const btVector3 worldVec1 = ToBullet( modelMat * glm::vec4( vec1[0], vec1[1], vec1[2], 1.f ) );
					const btVector3 worldVec2 = ToBullet( modelMat * glm::vec4( vec2[0], vec2[1], vec2[2], 1.f ) );

					_world.GetSingleton<RenderDebug>().DebugLine( worldVec0, worldVec1, color );
					_world.GetSingleton<RenderDebug>().DebugLine( worldVec1, worldVec2, color );
					_world.GetSingleton<RenderDebug>().DebugLine( worldVec2, worldVec0, color );
				}
			}
		}
	};

	//========================================================================================================
	// Draw all the point lights gizmos
	//========================================================================================================
	struct SDrawDebugPointLights : EcsSystem
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

        static void DrawPointLight( RenderDebug& _renderDebug,
                                    const Transform& _transform,
                                    const PointLight& _light )
		{
			const float lightRange = PointLight::GetLightRange( _light );
			if( lightRange > 0 )
			{
				_renderDebug.DebugSphere( _transform.GetPosition(), lightRange, _light.mDiffuse );
			}
		}
	};

	//========================================================================================================
	// Draw all the directional lights gizmos
	//========================================================================================================
	struct SDrawDebugDirectionalLights : EcsSystem
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

        static void DrawDirectionalLight( RenderDebug& _renderDebug,
                                          const Transform& _transform,
                                          const DirectionalLight& /*_light*/ )
		{
			const btVector3 pos = _transform.GetPosition();
			const btVector3 dir = _transform.Forward();
			const btVector3 up = _transform.Up();
			const btVector3 left = _transform.Left();
			const float length = 2.f;
			const float radius = 0.5f;
			const Color color = Color::sYellow;
            btVector3 offsets[5] = { btVector3_Zero,
                                     radius * up,
                                     -radius * up,
                                     radius * left,
                                     -radius * left };
			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
			{
				const btVector3 offset = offsets[offsetIndex];
				_renderDebug.DebugLine( pos + offset, pos + offset + length * dir, color, false );
			}
			_renderDebug.DebugIcoSphere( _transform.mTransform, radius, 0, color, false );
		}
	};

	//========================================================================================================
	// @todo split this in two systems for BoxShape & SphereShape
	// Draw physics shapes in wireframe (box, sphere, etc )
	//========================================================================================================
	struct SDrawDebugCollisionShapes : EcsSystem
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
                _world.GetSingleton<RenderDebug>().DebugCube( transform.mTransform, 0.5f * shape.GetScaling(), Color::sGreen, false );
            }

			// sphere shape
			if( _world.HasComponent<SphereShape>( _entity ) )
			{
                const SphereShape& shape = _world.GetComponent<SphereShape>( _entity );
                _world.GetSingleton<RenderDebug>().DebugSphere( transform.GetPosition(), shape.GetRadius(), Color::sGreen, false );
            }
		}
	};


    //========================================================================================================
    // Draw physics sphere collider in wireframe
    //========================================================================================================
    struct SDrawDebugFxSphereColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() | _world.GetSignature<FxSphereCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<FxTransform>();
            auto sphereIt    = _view.begin<FxSphereCollider>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++sphereIt )
            {
                Draw( *sphereIt, *transformIt, renderDebug );
            }
        }

        static void Draw( const FxSphereCollider& _sphere, const FxTransform& _transform, RenderDebug& _renderDebug )
        {
            _renderDebug.DebugSphere( Math::ToBullet( _transform.mPosition + _transform.TransformDirection(_sphere.mOffset) ), _sphere.mRadius.ToFloat(), Color::sGreen, false );
        }
    };

    //========================================================================================================
    // Draw physics box collider in wireframe
    //========================================================================================================
    struct SDrawDebugFxBoxColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() | _world.GetSignature<FxBoxCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<FxTransform>();
            auto boxIt    = _view.begin<FxBoxCollider>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++boxIt )
            {
                Draw( *boxIt, *transformIt, renderDebug );
            }
        }

        static void Draw( const FxBoxCollider& _box, const FxTransform& _transform, RenderDebug& _renderDebug )
        {
            btTransform btTrans( Math::ToBullet(_transform.mRotation), Math::ToBullet(_transform.mPosition));
            _renderDebug.DebugCube(btTrans, Math::ToBullet(_box.mHalfExtents), Color::sGreen, false );
        }
    };

    //========================================================================================================
    //========================================================================================================
    struct SDrawDebugUiBounds : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() | _world.GetSignature<TagUIVisible>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<UITransform>();
            for( ; transformIt != _view.end<UITransform>(); ++transformIt )
            {
                UITransform transform = *transformIt;
                const glm::ivec2& pos = transform.mPosition;
                const glm::ivec2& size = transform.mSize;
                renderDebug.DebugQuad2D( pos, size, Color::sGreen );
            }
        }
    };

}