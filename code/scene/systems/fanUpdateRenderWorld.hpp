#include "ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
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

				if( meshRenderer.mesh.IsValid() )
				{
					// drawMesh data;
					RenderDataModel data;
					data.mesh = *meshRenderer.mesh;
					data.modelMatrix = transform.GetModelMatrix();
					data.normalMatrix = transform.GetNormalMatrix();
					data.textureIndex = material.texture.IsValid() ? material.texture->mIndex : 0;
					data.color = material.color.ToGLM();
					data.shininess = material.shininess;

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
				| _world.GetSignature<TransformUI>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view )
		{
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
			renderWorld.uiDrawData.clear();

			auto rendererIt = _view.begin<UIRenderer>();
			auto transformIt = _view.begin<TransformUI>();

			for( ; rendererIt != _view.end<UIRenderer>(); ++rendererIt, ++transformIt )
			{
				UIRenderer& renderer = *rendererIt;
				const TransformUI& transform = *transformIt;

				if( renderer.GetTexture() == nullptr ) { continue; }

                const glm::vec2 size = glm::vec2( transform.mSize.x, transform.mSize.y );
                const glm::vec2 pos = glm::vec2( transform.mPosition.x, transform.mPosition.y );

				RenderDataUIMesh data;
				data.mesh = renderer.mUiMesh;
                data.position = pos / renderWorld.targetSize * 2.f - glm::vec2( 1.f, 1.f );
                data.scale = size / renderWorld.targetSize;
				data.color = renderer.color.ToGLM();
				data.textureIndex = renderer.GetTexture() != nullptr ? renderer.GetTexture()->mIndex : 0;
				renderWorld.uiDrawData.push_back( data );
			}
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
				pointLight.diffuse = light.diffuse.ToGLM();
				pointLight.specular = light.specular.ToGLM();
				pointLight.ambiant = light.ambiant.ToGLM();
				pointLight.constant = light.attenuation[PointLight::CONSTANT];
				pointLight.linear = light.attenuation[PointLight::LINEAR];
				pointLight.quadratic = light.attenuation[PointLight::QUADRATIC];

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
				light.ambiant = directionalLight.ambiant.ToGLM();
				light.diffuse = directionalLight.diffuse.ToGLM();
				light.specular = directionalLight.specular.ToGLM();

				renderWorld.directionalLights.push_back( light );
			}
		}
	};
}