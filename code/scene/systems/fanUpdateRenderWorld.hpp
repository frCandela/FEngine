#include "core/ecs/fanEcsSystem.hpp"
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
#include "scene/fanSceneTags.hpp"

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
					data.mMesh         = *meshRenderer.mMesh;
					data.mModelMatrix  = transform.GetModelMatrix();
					data.mNormalMatrix = transform.GetNormalMatrix();
					data.mTextureIndex = material.mTexture.IsValid() ? material.mTexture->mIndex : 0;
					data.mColor        = material.mColor.ToGLM();
					data.mShininess    = material.mShininess;

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
				| _world.GetSignature<UITransform>()
				| _world.GetSignature<TagUIVisible>();
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

                if( renderer.GetTexture() == nullptr || renderer.mMesh2D == nullptr )
				{
				    continue;
				}

                const glm::vec2 size = glm::vec2( transform.mSize.x, transform.mSize.y );
                const glm::vec2 pos = glm::vec2( transform.mPosition.x, transform.mPosition.y );

				RenderDataMesh2D data;
				data.mMesh         = renderer.mMesh2D;
                data.mPosition     = pos / renderWorld.mTargetSize * 2.f - glm::vec2( 1.f, 1.f );
                data.mScale        = size / renderWorld.mTargetSize;
				data.mColor        = renderer.mColor.ToGLM();
				data.mTextureIndex = renderer.GetTexture() != nullptr ? renderer.GetTexture()->mIndex : 0;
				data.mDepth        = renderer.mDepth;
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
	struct SUpdateRenderWorldPointLights : EcsSystem
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
				pointLight.mPosition  = glm::vec4( ToGLM( transform.GetPosition() ), 1.f );
				pointLight.mDiffuse   = light.mDiffuse.ToGLM();
				pointLight.mSpecular  = light.mSpecular.ToGLM();
				pointLight.mAmbiant   = light.mAmbiant.ToGLM();
				pointLight.mConstant  = light.mAttenuation[PointLight::Constant];
				pointLight.mLinear    = light.mAttenuation[PointLight::Linear];
				pointLight.mQuadratic = light.mAttenuation[PointLight::Quadratic];

				renderWorld.pointLights.push_back( pointLight );
			}
		}
	};

	//========================================================================================================
	// Update the render world directional lights
	//========================================================================================================
	struct SUpdateRenderWorldDirectionalLights : EcsSystem
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
				light.mDirection = glm::vec4( ToGLM( transform.Forward() ), 1 );
				light.mAmbiant   = directionalLight.mAmbiant.ToGLM();
				light.mDiffuse   = directionalLight.mDiffuse.ToGLM();
				light.mSpecular  = directionalLight.mSpecular.ToGLM();

				renderWorld.directionalLights.push_back( light );
			}
		}
	};
}