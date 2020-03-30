#include "scene/systems/fanUpdateRenderWorld.hpp"

#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/components/ui/fanUIRenderer.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	Signature S_UpdateRenderWorldModels::GetSignature( const EcsWorld& _world )
	{
		return	 _world.GetSignature<MeshRenderer>()
			| _world.GetSignature<SceneNode>()
			| _world.GetSignature<Transform>()
			| _world.GetSignature<Material>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldModels::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.drawData.clear();

		// get all mesh and adds them to the render world
		for( EntityID id : _entities )
		{
			MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( id );
			if( meshRenderer.mesh.IsValid() )
			{
				// drawMesh data;
				Material& material = _world.GetComponent<Material>( id );
				Transform& transform = _world.GetComponent<Transform>( id );

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
	Signature S_UpdateRenderWorldUI::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<UIRenderer>()
			| _world.GetSignature<TransformUI>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldUI::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.uiDrawData.clear();

		// get all mesh and adds them to the render world
		for( EntityID id : _entities )
		{
			UIRenderer& renderer = _world.GetComponent<UIRenderer>( id );
			TransformUI& transform = _world.GetComponent<TransformUI>( id );

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
	Signature S_UpdateRenderWorldPointLights::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>()
			| _world.GetSignature<PointLight>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldPointLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.pointLights.clear();

		for( EntityID id : _entities )
		{
			// light data
			Transform& transform = _world.GetComponent<Transform>( id );
			PointLight& light = _world.GetComponent<PointLight>( id );

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
	Signature S_UpdateRenderWorldDirectionalLights::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform>()
			| _world.GetSignature<DirectionalLight>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldDirectionalLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.directionalLights.clear();

		for( EntityID id : _entities )
		{
			// light data
			Transform& transform = _world.GetComponent<Transform>( id );
			DirectionalLight& directionalLight = _world.GetComponent<DirectionalLight>( id );

			DrawDirectionalLight light;
			light.direction = glm::vec4( ToGLM( transform.Forward() ), 1 );
			light.ambiant = directionalLight.ambiant.ToGLM();
			light.diffuse = directionalLight.diffuse.ToGLM();
			light.specular = directionalLight.specular.ToGLM();

			renderWorld.directionalLights.push_back( light );
		}
	}
}
