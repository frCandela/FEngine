#include "fanIncludes.h"

#include "fanEngine.h"
#include "vulkan/vkRenderer.h"
#include "vulkan/pipelines/vkDebugPipeline.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "util/fbx/fanFbxImporter.h"
#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanRenderWindow.h"	
#include "editor/windows/fanSceneWindow.h"	
#include "editor/windows/fanInspectorWindow.h"	
#include "editor/windows/fanPreferencesWindow.h"	
#include "editor/components/fanFPSCamera.h"		
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMesh.h"
#include "scene/components/fanActor.h"

namespace fan {
	Engine * Engine::ms_engine = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Engine::Engine() :
		m_applicationShouldExit(false) {

		m_editorGrid.isVisible = true;
		m_editorGrid.color = glm::vec4(0.161f, 0.290f, 0.8f, 0.478f);
		m_editorGrid.linesCount = 100;
		m_editorGrid.spacing = 1.f;		

		ms_engine = this;

		m_mainMenuBar = new editor::MainMenuBar();
		m_renderWindow = new editor::RenderWindow();
		m_sceneWindow = new editor::SceneWindow();
		m_inspectorWindow = new editor::InspectorWindow();
		m_preferencesWindow = new editor::PreferencesWindow();

		m_renderer = new vk::Renderer({ 1280,720 });
		m_scene = new scene::Scene("mainScene");

		scene::Gameobject * cameraGameobject = m_scene->CreateGameobject("editor_camera");
		cameraGameobject->SetRemovable(false);
		scene::Transform * camTrans = cameraGameobject->AddComponent<scene::Transform>();
		camTrans->SetPosition(glm::vec3(0, 0, -2));
		scene::Camera * cameraComponent = cameraGameobject->AddComponent<scene::Camera>();
		cameraComponent->SetRemovable(false);
		m_renderer->SetMainCamera(cameraComponent);
		scene::FPSCamera * editorCamera = cameraGameobject->AddComponent<scene::FPSCamera>();
		editorCamera->SetRemovable(false);

// 		scene::Gameobject * cube = m_scene->CreateGameobject("cube");
// 		cube->AddComponent<scene::Transform>();
// 		scene::Mesh * mesh = cube->AddComponent<scene::Mesh>();
// 
// 		util::FBXImporter importer;
// 		importer.LoadScene("content/models/test/cube.fbx");
// 		if (importer.GetMesh(*mesh) == true) {
// 			m_renderer->AddMesh(mesh);
// 		}
// 
// 		cube->GetComponent<scene::Transform>();		
	}

	//================================================================================================================================
	//================================================================================================================================
	Engine::~Engine() {
		delete m_mainMenuBar;
		delete m_renderWindow;
		delete m_sceneWindow;

		delete m_renderer;
		delete m_scene;
	}

	//================================================================================================================================
	//================================================================================================================================

	void Engine::Exit() {
		m_applicationShouldExit = true;
		std::cout << "Exit application" << std::endl;

		for (auto actor : m_startingActors) { m_stoppingActors.insert(actor); }
		m_startingActors.clear();
		for (auto actor : m_activeActors)	{ m_stoppingActors.insert(actor); }
		m_activeActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::Run()
	{

		btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher *dispatcher = new	btCollisionDispatcher(collisionConfiguration);
		btBroadphaseInterface *broadphase = new btDbvtBroadphase();
		btSequentialImpulseConstraintSolver *sequentialImpulseConstraintSolver = new btSequentialImpulseConstraintSolver;
		btDiscreteDynamicsWorld *world = new btDiscreteDynamicsWorld(dispatcher, broadphase, sequentialImpulseConstraintSolver, collisionConfiguration);

		world->setGravity(btVector3(0, -10, 0));

/*		btCollisionShape* shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));*/
// 
// 		btTransform transform;
// 		transform.setIdentity();
// 		transform.setOrigin(btVector3(0, 20, 0));
// 		btVector3 localInertia(0, 0, 0);
// 		btScalar mass = 0.5f;
// 		if (mass != 0 ) {
// 			shape->calculateLocalInertia(mass, localInertia);
// 		}
// 		btDefaultMotionState * motionState;
// 		motionState = new btDefaultMotionState(transform);
// 
// 		btRigidBody::btRigidBodyConstructionInfo boxRigidBodyConstructionInfo(mass, motionState, shape, localInertia);
// 		btRigidBody * body = new btRigidBody(boxRigidBodyConstructionInfo);
// 		world->addRigidBody(body);
// 		(void)body;

		//btCollisionShape* shapeSol = new btBoxShape(btVector3(10, 1, 10));	
		btVector3 normal(0, 1, 0);
		btCollisionShape* shapeSol = new btStaticPlaneShape(normal, 1.f);
		btVector3 localInertiaSol(0, 0, 0);
		btScalar mass = 0;
		btTransform transformSol;
		transformSol.setIdentity();
		transformSol.setOrigin(btVector3(0, -1, 0));
		btDefaultMotionState * motionStateSol = new btDefaultMotionState(transformSol);
		btRigidBody::btRigidBodyConstructionInfo solInfo(mass, motionStateSol, shapeSol, localInertiaSol);
		btRigidBody * bodySol = new btRigidBody(solInfo);
		(void)bodySol;
		world->addRigidBody(bodySol);

		const float physicsDelta = 0.02f;
		float physicsTime = 0.f;
		float lastUpdateTime = Time::ElapsedSinceStartup();
		while ( m_applicationShouldExit == false)
		{
			if (m_renderer->WindowIsOpen() == false) {
				Exit();
			}

			const float time = Time::ElapsedSinceStartup();
			const float updateDelta = time - lastUpdateTime;

			if (updateDelta > 1.f / Time::GetFPS()) {
				lastUpdateTime = time;
				physicsTime += updateDelta;
				
				
				while (physicsTime > updateDelta) {
					physicsTime -= updateDelta;
					//world->stepSimulation(updateDelta);
				}

				ActorStart();

				for( scene::Actor * actor : m_activeActors ) {
					actor->Update(updateDelta);
				}


				{


// 					btTransform trans;
// 					body->getMotionState()->getWorldTransform(trans);
// 					btVector3 origin = trans.getOrigin();
// 					float arr[3] = { origin.x(),origin.y(),origin.z() };
// 					ImGui::DragFloat3("toto", arr);

					static btVector3 from(0,10,0);
					float arr0[3] = { from.x(),from.y(),from.z() };
					if (ImGui::DragFloat3("from", arr0)) {
						from = btVector3(arr0[0], arr0[1], arr0[2]);
					}


					static btVector3 to(0,-10, 0);
					float arr2[3] = { to.x(),to.y(),to.z() };
					if (ImGui::DragFloat3("to", arr2)) {
						to = btVector3(arr2[0], arr2[1], arr2[2]);
					}

					btCollisionWorld::ClosestRayResultCallback callback(from, to);
					world->rayTest(from, to, callback);
					m_renderer->DebugLine(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(to.x(), to.y(), to.z()), glm::vec4(0, 1, 0, 1));
					bool result = callback.hasHit();
					ImGui::Checkbox("hit", &result);
					if (result) {
						const btVector3 pt = callback.m_hitPointWorld;
						glm::vec3 ptgl(pt.x(), pt.y(), pt.z());
						glm::vec4 color(1, 0, 0, 1);
						m_renderer->DebugLine(ptgl- glm::vec3(0.f, 0.05f, 0.f), ptgl + glm::vec3(0.f, 0.05f, 0.f), color);
						m_renderer->DebugLine(ptgl - glm::vec3( 0.05f, 0.f, 0.f), ptgl + glm::vec3(0.05f, 0.f, 0.f), color);
						m_renderer->DebugLine(ptgl - glm::vec3(0.f, 0.f, 0.05f), ptgl + glm::vec3(0.f, 0.f, 0.05f), color);

					}
				}




				DrawUI();
				DrawEditorGrid();
				m_renderer->DrawFrame();
				m_scene->EndFrame();

				ActorStop();
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::AddActor(scene::Actor * _actor) {

		if (m_startingActors.find(_actor) == m_startingActors.end()
			&& m_activeActors.find(_actor) == m_activeActors.end()
			&& m_stoppingActors.find(_actor) == m_stoppingActors.end()) {
			m_startingActors.insert(_actor);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::RemoveActor(scene::Actor * _actor) {
		auto it = m_activeActors.find(_actor);
		if (it != m_activeActors.end()) {
			m_activeActors.erase(_actor);
			m_stoppingActors.insert(_actor);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ActorStart() {
		for ( auto actor : m_startingActors ) {
			actor->Start();	
			m_activeActors.insert(actor);
		}
		m_startingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::ActorStop() {
		for (auto actor : m_stoppingActors) {
			actor->Stop();
			delete actor;
		}
		m_stoppingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawEditorGrid() {
		if (m_editorGrid.isVisible == true) {
			const float size = m_editorGrid.spacing;
			const int count = m_editorGrid.linesCount;

			for (int coord = -m_editorGrid.linesCount; coord <= m_editorGrid.linesCount; coord++) {
				m_renderer->DebugLine(glm::vec3(-count * size, 0.f, coord*size), glm::vec3(count*size, 0.f, coord*size), m_editorGrid.color);
				m_renderer->DebugLine(glm::vec3(coord*size, 0.f, -count * size), glm::vec3(coord*size, 0.f, count*size), m_editorGrid.color);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Engine::DrawUI() {
		m_mainMenuBar->Draw();
		m_renderWindow->Draw();
		m_sceneWindow->Draw();	
		m_inspectorWindow->Draw();
		m_preferencesWindow->Draw();
	}
}