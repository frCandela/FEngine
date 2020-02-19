#include "scene/ecs/fanECSComponents.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan 
{
	const char * ecsTranform::s_name					= "Tranform    ";
	const char * ecsPosition::s_name					= "Position    ";
	const char * ecsRotation::s_name					= "Rotation    ";
	const char * ecsScaling::s_name						= "Scaling     ";
	const char * ecsMovement::s_name					= "Movement    ";
	const char * ecsParticle::s_name					= "Particle    ";
	const char * ecsSunlightParticleOcclusion::s_name	= "SLPOccluder ";
	const char * ecsAABB::s_name						= "AABB        ";
	const char * ecsMesh::s_name						= "Mesh        ";
	const char * ecsPointLight::s_name					= "PointLight  ";
	const char * ecsDirLight::s_name					= "DirLight    ";
	const char * ecsMaterial::s_name					= "Material    ";
	const char * ecsPlanet::s_name						= "Planet      ";
	const char * ecsRigidbody::s_name					= "Rigidbody   ";
	const char * ecsMotionState::s_name					= "MotionState ";	
	const char * ecsSphereShape::s_name					= "SphereShape ";
	const char * ecsBoxShape::s_name					= "BoxShape    ";
	const char * ecsFlags::s_name						= "Flags       ";
	const char * ecsEditorFlags::s_name					= "EditorFlags ";
	const char * ecsBullet::s_name						= "Bullet      ";
	const char * ecsGameobject::s_name					= "Gameobject  ";


	//================================================================================================================================
	// @todo "mesh" shouldn't be a reference, it is that way because we don't want to include MeshPtr in this header
	// The same problem applies to ecsMaterial and it's "texture"
	// This will be fixed when the ecs is made dynamics and components types can be added from other libs than the core lib (game, scene )
	//================================================================================================================================
	ecsMesh::ecsMesh() : mesh( *new MeshPtr() ) {}
	ecsMesh::~ecsMesh() { delete& mesh; }
	void ecsMesh::Init()
	{
		mesh = nullptr;
		renderID = -1;
	}
	void ecsMesh::Clear() { mesh = nullptr; }

	//================================================================================================================================
	// see ecsMesh comment
	//================================================================================================================================
	ecsMaterial::ecsMaterial(): texture( *new TexturePtr() ) {}
	ecsMaterial::~ecsMaterial(){ delete& texture; }
	void ecsMaterial::Init()
	{
		texture = nullptr;
		shininess = 1;
		color = Color::White;
	}
	void ecsMaterial::Clear() { texture = nullptr; }
}