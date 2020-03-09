#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanEntityWorld.hpp"
#include "scene/ecs/fanTag.hpp"

#include "scene/ecs/singletonComponents/fanRenderWorld.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanMaterial2.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldModels : System
	{
		static Signature GetSignature( const EntityWorld& _world )
		{
 			return	 _world.GetSignature<MeshRenderer2>()
					|_world.GetSignature<SceneNode>()
					|_world.GetSignature<Transform2>()
					|_world.GetSignature<Material2>();
		}
		static void Run( EntityWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
			renderWorld.drawData.clear();

			// get all mesh and adds them to the render world
			for ( EntityID id : _entities )
			{
				Entity& entity = _world.GetEntity( id );
				MeshRenderer2& meshRenderer = _world.GetComponent<MeshRenderer2>( id );
				if( meshRenderer.mesh.IsValid() )
				{
					// drawMesh data;
					Material2& material = _world.GetComponent<Material2>( id );
					Transform2& transform =_world.GetComponent<Transform2>( id );

					DrawMesh data;
 					data.mesh = * meshRenderer.mesh;
					data.modelMatrix = transform.GetModelMatrix();
					data.normalMatrix = transform.GetNormalMatrix();
					data.textureIndex = material.texture.IsValid() ? material.texture->GetRenderID() : 0;
					data.color = material.color.ToGLM();
					data.shininess = material.shininess;		

					renderWorld.drawData.push_back( data );
				}
			}
		}
	};

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateRenderWorldPointLights : System
	{
		static Signature GetSignature( const EntityWorld& _world )
		{
			return	_world.GetSignature<Transform2>()
				|   _world.GetSignature<PointLight2>();
		}

		static void Run( EntityWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
			renderWorld.pointLights.clear();

			for( EntityID id : _entities )
			{
				// light data
				Entity& entity = _world.GetEntity( id );
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
	};

	//==============================================================================================================================================================
//==============================================================================================================================================================
	struct S_UpdateRenderWorldDirectionalLights : System
	{
		static Signature GetSignature( const EntityWorld& _world )
		{
			return	_world.GetSignature<Transform2>()
				  | _world.GetSignature<DirectionalLight2>();
		}

		static void Run( EntityWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();
			renderWorld.directionalLights.clear();

			for( EntityID id : _entities )
			{
				// light data
				Entity& entity = _world.GetEntity( id );
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
	};
	// 
// 		Camera& mainCamera = m_currentScene->GetMainCamera();
// 		const std::vector < DirectionalLight* > directionalLights = m_currentScene->GetDirectionalLights();
// 		const std::vector < PointLight* >		pointLights = m_currentScene->GetPointLights();
// 		const std::vector < MeshRenderer* >		meshRenderers = m_currentScene->GetMeshRenderers();
// 
// 		// Camera
// 		Transform& cameraTransform = mainCamera.GetGameobject().GetTransform();
// 		mainCamera.SetAspectRatio( m_gameWindow->GetAspectRatio() );
// 		m_renderer->SetMainCamera( mainCamera.GetProjection(), mainCamera.GetView(), ToGLM( cameraTransform.GetPosition() ) );
// 
// 		// Point lights		
// 		for ( int lightIndex = 0; lightIndex < pointLights.size(); lightIndex++ )
// 		{
// 			const PointLight* light = pointLights[ lightIndex ];
// 			const Transform& lightTransform = light->GetGameobject().GetTransform();
// 			m_renderer->SetPointLight(
// 				lightIndex,
// 				ToGLM( lightTransform.GetPosition() ),
// 				light->GetDiffuse().ToGLM(),
// 				light->GetSpecular().ToGLM(),
// 				light->GetAmbiant().ToGLM(),
// 				light->GetAttenuation()
// 			);
// 		}	m_renderer->SetNumPointLights( static_cast< uint32_t >( pointLights.size() ) );
// 
// 		// Directional lights		
// 		for ( int lightIndex = 0; lightIndex < directionalLights.size(); lightIndex++ )
// 		{
// 			const DirectionalLight* light = directionalLights[ lightIndex ];
// 			const Transform& lightTransform = light->GetGameobject().GetTransform();
// 			m_renderer->SetDirectionalLight(
// 				lightIndex
// 				, glm::vec4( ToGLM( lightTransform.Forward() ), 1 )
// 				, light->GetAmbiant().ToGLM()
// 				, light->GetDiffuse().ToGLM()
// 				, light->GetSpecular().ToGLM()
// 			);
// 		} m_renderer->SetNumDirectionalLights( static_cast< uint32_t >( directionalLights.size() ) );
// 
// 		// Transforms, mesh, materials
// 		std::vector<DrawMesh> drawData;
// 		drawData.reserve( meshRenderers.size() + 1 );
// 		for ( int modelIndex = 0; modelIndex < meshRenderers.size(); modelIndex++ )
// 		{
// 			DrawMesh data;
// 			MeshRenderer* meshRenderer = meshRenderers[ modelIndex ];
// 			Transform& transform = meshRenderer->GetGameobject().GetTransform();
// 			Material* material = meshRenderer->GetGameobject().GetComponent<Material>();
// 
// 			// Mesh
// 			data.mesh = meshRenderer->GetMesh();
// 
// 			if ( data.mesh == nullptr )
// 			{
// 				continue;
// 			}
// 
// 			// Transform
// 			data.modelMatrix = transform.GetModelMatrix();
// 			data.normalMatrix = transform.GetNormalMatrix();
// 
// 			// Materials
// 			if ( material != nullptr )
// 			{
// 				const uint32_t textureIndex = material->GetTexture() != nullptr ? material->GetTexture()->GetRenderID() : 0;
// 				data.color = material->GetColor().ToGLM();
// 				data.shininess = material->GetShininess();
// 				data.textureIndex = textureIndex;
// 			}
// 			else
// 			{
// 				data.color = Color::White.ToGLM();
// 				data.shininess = 1;
// 				data.textureIndex = 0;
// 			}
// 			
// 		}
// 
// 		// particles
// 		DrawMesh particlesDrawData;
// 		particlesDrawData.mesh = m_currentScene->GetEcsManager().GetSingletonComponents().GetComponent<ecsParticlesMesh_s>().mesh;
// 		particlesDrawData.modelMatrix = glm::mat4( 1.f );
// 		particlesDrawData.normalMatrix = glm::mat4( 1.f );
// 		particlesDrawData.color = glm::vec4( 1.f, 1.f, 1.f, 1.f );
// 		particlesDrawData.shininess = 1;
// 		particlesDrawData.textureIndex = 1;// HACK -> 1 is white texture by default
// 		drawData.push_back( particlesDrawData );
// 
// 		m_renderer->SetDrawData( drawData );
// 
// 		// UI
// 		std::vector< UIMeshRenderer* > uiRenderers = m_clientScene->FindComponentsOfType<UIMeshRenderer>();
// 		std::vector<DrawUIMesh> uiDrawData;
// 		uiDrawData.reserve( uiRenderers.size() );
// 		for ( int meshIndex = 0; meshIndex < uiRenderers.size(); meshIndex++ )
// 		{
// 			UIMeshRenderer* meshRenderer = uiRenderers[ meshIndex ];
// 			if ( meshRenderer->IsVisible() && meshRenderer->GetMesh()->GetVertices().size() > 0 )
// 			{
// 				Transform& transform = meshRenderer->GetGameobject().GetTransform();
// 				Texture* texture = meshRenderer->GetTexture();
// 
// 				DrawUIMesh uiMesh;
// 				uiMesh.mesh = meshRenderer->GetMesh();
// 				uiMesh.scale = { transform.GetScale().x(), transform.GetScale().y() };
// 				uiMesh.position = { transform.GetPosition().x(), transform.GetPosition().y() };
// 				uiMesh.color = meshRenderer->GetColor().ToGLM();
// 				uiMesh.textureIndex = texture != nullptr ? texture->GetRenderID() : 0;
// 				uiDrawData.push_back( uiMesh );
// 			}
// 		}
// 		m_renderer->SetUIDrawData( uiDrawData );
}