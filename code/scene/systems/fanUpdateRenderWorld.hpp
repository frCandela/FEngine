#include "ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"

namespace fan
{
	//========================================================================================================
	// Update the render world rendered meshes
	//========================================================================================================
	struct SUpdateRenderWorldModels : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	 _world.GetSignature<MeshRenderer>()
				| _world.GetSignature<SceneNode>()
				| _world.GetSignature<Transform>()
				| _world.GetSignature<Material>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
			renderWorld.drawData.clear();

			auto meshRendererIt = _view.begin<MeshRenderer>();
			auto transformIt = _view.begin<Transform>();
			auto materialIt = _view.begin<Material>();
			// get all mesh and adds them to the render world
            for( ; meshRendererIt != _view.end<MeshRenderer>();
                   ++meshRendererIt, ++transformIt, ++materialIt )
			{
				MeshRenderer& meshRenderer = *meshRendererIt;
				Transform& transform = *transformIt;
				Material& material = *materialIt;

				if( meshRenderer.mMesh.IsValid() )
				{
					// drawMesh data;
					RenderDataModel data;
					data.mesh = *meshRenderer.mMesh;
					data.modelMatrix = transform.GetModelMatrix();
					data.normalMatrix = transform.GetNormalMatrix();
					data.textureIndex = material.mTexture.IsValid() ? material.mTexture->mIndex : 0;
					data.color = material.mColor.ToGLM();
					data.shininess = material.mShininess;

					renderWorld.drawData.push_back( data );
				}
			}
		}
	};

	//========================================================================================================
    // Get all ui mesh and adds them to the render world
	//========================================================================================================
	struct 	SUpdateRenderWorldUI : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<UIRenderer>()
				| _world.GetSignature<UITransform>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
			renderWorld.uiDrawData.clear();

			auto rendererIt = _view.begin<UIRenderer>();
			auto transformIt = _view.begin<UITransform>();

			for( ; rendererIt != _view.end<UIRenderer>(); ++rendererIt, ++transformIt )
			{
				UIRenderer       & renderer  = *rendererIt;
				const UITransform& transform = *transformIt;

                if( renderer.GetTexture() == nullptr ||
                    !renderer.mVisible ||
                    renderer.mMesh2D == nullptr )
				{
				    continue;
				}

                const glm::vec2 size = glm::vec2( transform.mSize.x, transform.mSize.y );
                const glm::vec2 pos = glm::vec2( transform.mPosition.x, transform.mPosition.y );

				RenderDataMesh2D data;
				data.mesh = renderer.mMesh2D;
                data.position = pos / renderWorld.mTargetSize * 2.f - glm::vec2( 1.f, 1.f );
                data.scale = size / renderWorld.mTargetSize;
				data.color = renderer.mColor.ToGLM();
				data.textureIndex = renderer.GetTexture() != nullptr ? renderer.GetTexture()->mIndex : 0;
				data.mDepth = renderer.mDepth;
				renderWorld.uiDrawData.push_back( data );
			}

			auto sortFunc = []( RenderDataMesh2D& _a, RenderDataMesh2D& _b) {
                return _a.mDepth > _b.mDepth;
            };
			std::sort( renderWorld.uiDrawData.begin(), renderWorld.uiDrawData.end(), sortFunc );
		}
	};

	//========================================================================================================
	// Update the render world point lights
	//========================================================================================================
	struct S_UpdateRenderWorldPointLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>()
				| _world.GetSignature<PointLight>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
			renderWorld.pointLights.clear();

			auto transformIt = _view.begin<Transform>();
			auto lightIt = _view.begin<PointLight>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
			{
				const Transform& transform = *transformIt;
				PointLight& light = *lightIt;

				RenderDataPointLight pointLight;
				pointLight.position = glm::vec4( ToGLM( transform.GetPosition() ), 1.f );
				pointLight.diffuse = light.mDiffuse.ToGLM();
				pointLight.specular = light.mSpecular.ToGLM();
				pointLight.ambiant = light.mAmbiant.ToGLM();
				pointLight.constant = light.mAttenuation[PointLight::Constant];
				pointLight.linear = light.mAttenuation[PointLight::Linear];
				pointLight.quadratic = light.mAttenuation[PointLight::Quadratic];

				renderWorld.pointLights.push_back( pointLight );
			}
		}
	};

	//========================================================================================================
	// Update the render world directional lights
	//========================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>()
				| _world.GetSignature<DirectionalLight>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
			renderWorld.directionalLights.clear();

			auto transformIt = _view.begin<Transform>();
			auto lightIt = _view.begin<DirectionalLight>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
			{
				const Transform& transform = *transformIt;
				DirectionalLight& directionalLight = *lightIt;

				RenderDataDirectionalLight light;
				light.direction = glm::vec4( ToGLM( transform.Forward() ), 1 );
				light.ambiant = directionalLight.mAmbiant.ToGLM();
				light.diffuse = directionalLight.mDiffuse.ToGLM();
				light.specular = directionalLight.mSpecular.ToGLM();

				renderWorld.directionalLights.push_back( light );
			}
		}
	};
}