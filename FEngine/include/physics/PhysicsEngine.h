#pragma once

#include "bullet3/btBulletDynamicsCommon.h"

class PhysicsEngine
{
public:
	PhysicsEngine()
	{
		//Instanciates a dynamicsWorld 
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}

	~PhysicsEngine()
	{
		delete(collisionConfiguration);
		delete(dispatcher);
		delete(overlappingPairCache);
		delete(solver);
		delete(dynamicsWorld);
	}

	btDefaultCollisionConfiguration * collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

private:

};




