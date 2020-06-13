#include "scene/systems/fanUpdateRenderWorld.hpp"

#include "fanGLM.hpp"
#include "core/math/fanMathUtils.hpp"
#include "ecs/fanEcsWorld.hpp"
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
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	EcsSignature S_UpdateRenderWorldModels::GetSignature( const EcsWorld& _world )
	{
		return	 _world.GetSignature<MeshRenderer>()
			| _world.GetSignature<SceneNode>()
			| _world.GetSignature<Transform>()
			| _world.GetSignature<Material>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldModels::Run( EcsWorld& _world, const EcsView& _view ) 
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.drawData.clear();

		auto meshRendererIt = _view.begin<MeshRenderer>();
		auto transformIt = _view.begin<Transform>();
		auto materialIt = _view.begin<Material>();
		// get all mesh and adds them to the render world
		for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt, ++materialIt )
		{
			MeshRenderer& meshRenderer = *meshRendererIt;
			Transform& transform = *transformIt;
			Material& material = *materialIt;

			if( meshRenderer.mesh.IsValid() )
			{
				// drawMesh data;
				DrawMesh data;
				data.mesh = *meshRenderer.mesh;
				data.modelMatrix = transform.GetModelMatrix();
				data.normalMatrix = transform.GetNormalMatrix();
				data.textureIndex = material.texture.IsValid() ? material.texture->GetRenderID() : 0;
				data.color = material.color.ToGLM();
				data.shininess = material.shininess;

				renderWorld.drawData.push_back( data );
			}
		}
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	EcsSignature S_UpdateRenderWorldUI::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<UIRenderer>()
			| _world.GetSignature<TransformUI>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldUI::Run( EcsWorld& _world, const EcsView& _view ) 
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.uiDrawData.clear();

		auto rendererIt = _view.begin<UIRenderer>();
		auto transformIt = _view.begin<TransformUI>();
		// get all mesh and adds them to the render world
		for( ; rendererIt != _view.end<UIRenderer>(); ++rendererIt, ++transformIt )
		{
			UIRenderer& renderer = *rendererIt;
			const TransformUI& transform = *transformIt;

			if( renderer.GetTexture() == nullptr ) { continue; }

			const glm::vec2 textureSize = renderer.GetTexture()->GetSize();
			const glm::vec2 imageRatio = textureSize / renderWorld.targetSize;
			const glm::vec2 positionRatio = 2.f * transform.position / renderWorld.targetSize;

			DrawUIMesh data;
			data.mesh = &renderer.uiMesh;
			data.scale = transform.scale * imageRatio;
			data.position = positionRatio - glm::vec2( 1, 1 );
			data.color = renderer.color.ToGLM();
			data.textureIndex = renderer.GetTexture() != nullptr ? renderer.GetTexture()->GetRenderID() : 0;

			renderWorld.uiDrawData.push_back( data );
		}
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	EcsSignature S_UpdateRenderWorldPointLights::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>()
			| _world.GetSignature<PointLight>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldPointLights::Run( EcsWorld& _world, const EcsView& _view ) 
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.pointLights.clear();

		auto transformIt = _view.begin<Transform>();
		auto lightIt = _view.begin<PointLight>();
		for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
		{
			const Transform& transform = *transformIt;
			PointLight& light = *lightIt;

			DrawPointLight pointLight;
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

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	EcsSignature S_UpdateRenderWorldDirectionalLights::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>()
			| _world.GetSignature<DirectionalLight>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldDirectionalLights::Run( EcsWorld& _world, const EcsView& _view ) 
	{
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
		renderWorld.directionalLights.clear();

		auto transformIt = _view.begin<Transform>();
		auto lightIt = _view.begin<DirectionalLight>();
		for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
		{
			const Transform& transform = *transformIt;
			DirectionalLight& directionalLight = *lightIt;

			DrawDirectionalLight light;
			light.direction = glm::vec4( ToGLM( transform.Forward() ), 1 );
			light.ambiant = directionalLight.ambiant.ToGLM();
			light.diffuse = directionalLight.diffuse.ToGLM();
			light.specular = directionalLight.specular.ToGLM();

			renderWorld.directionalLights.push_back( light );
		}
	}
}
