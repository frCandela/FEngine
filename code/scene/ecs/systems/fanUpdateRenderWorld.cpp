#include "scene/ecs/systems/fanUpdateRenderWorld.hpp"

#include "scene/ecs/singletonComponents/fanRenderWorld.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanMaterial2.hpp"
#include "scene/ecs/components/fanTransformUI.hpp"
#include "scene/ecs/components/fanUIRenderer.hpp"
#include "scene/ecs/components/fanPointLight2.hpp"
#include "scene/ecs/components/fanDirectionalLight2.hpp"

#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	Signature S_UpdateRenderWorldModels::GetSignature( const EcsWorld& _world )
	{
		return	 _world.GetSignature<MeshRenderer2>()
			| _world.GetSignature<SceneNode>()
			| _world.GetSignature<Transform2>()
			| _world.GetSignature<Material2>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldModels::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.drawData.clear();

		// get all mesh and adds them to the render world
		for( EntityID id : _entities )
		{
			MeshRenderer2& meshRenderer = _world.GetComponent<MeshRenderer2>( id );
			if( meshRenderer.mesh.IsValid() )
			{
				// drawMesh data;
				Material2& material = _world.GetComponent<Material2>( id );
				Transform2& transform = _world.GetComponent<Transform2>( id );

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
		return _world.GetSignature<UIRenderer2>()
			| _world.GetSignature<UITransform2>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldUI::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.uiDrawData.clear();

		// get all mesh and adds them to the render world
		for( EntityID id : _entities )
		{
			UIRenderer2& renderer = _world.GetComponent<UIRenderer2>( id );
			UITransform2& transform = _world.GetComponent<UITransform2>( id );

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
		return	_world.GetSignature<Transform2>()
			| _world.GetSignature<PointLight2>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldPointLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.pointLights.clear();

		for( EntityID id : _entities )
		{
			// light data
			Transform2& transform = _world.GetComponent<Transform2>( id );
			PointLight2& light = _world.GetComponent<PointLight2>( id );

			DrawPointLight pointLight;
			pointLight.position = glm::vec4( ToGLM( transform.GetPosition() ), 1.f );
			pointLight.diffuse = light.diffuse.ToGLM();
			pointLight.specular = light.specular.ToGLM();
			pointLight.ambiant = light.ambiant.ToGLM();
			pointLight.constant = light.attenuation[PointLight2::CONSTANT];
			pointLight.linear = light.attenuation[PointLight2::LINEAR];
			pointLight.quadratic = light.attenuation[PointLight2::QUADRATIC];

			renderWorld.pointLights.push_back( pointLight );
		}
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	Signature S_UpdateRenderWorldDirectionalLights::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Transform2>()
			| _world.GetSignature<DirectionalLight2>();
	}

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	void S_UpdateRenderWorldDirectionalLights::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
		renderWorld.directionalLights.clear();

		for( EntityID id : _entities )
		{
			// light data
			Transform2& transform = _world.GetComponent<Transform2>( id );
			DirectionalLight2& directionalLight = _world.GetComponent<DirectionalLight2>( id );

			DrawDirectionalLight light;
			light.direction = glm::vec4( ToGLM( transform.Forward() ), 1 );
			light.ambiant = directionalLight.ambiant.ToGLM();
			light.diffuse = directionalLight.diffuse.ToGLM();
			light.specular = directionalLight.specular.ToGLM();

			renderWorld.directionalLights.push_back( light );
		}
	}
}
