#include "FEngine.h"

#include "bullet3/btBulletDynamicsCommon.h"

int main()
{
	btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();;
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);;
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;;
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -20, 0));

	FEngine app;

	try 
	{
		app.Run();
	}
	catch (const std::runtime_error& e) 
	{
		std::cerr << e.what() << std::endl;
	}

	int zob;
	std::cin >> zob;
	return EXIT_SUCCESS;
}