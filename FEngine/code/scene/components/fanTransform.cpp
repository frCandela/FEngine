#include "fanIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"

namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Transform::Transform() :
		m_rotation	( glm::vec3(0, 0, 0))
		, m_position( glm::vec3(0, 0, 0))
		, m_scale	( glm::vec3(1, 1, 1)) {
		SetRemovable(false);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition(glm::vec3 newPosition)
	{
		m_position = newPosition;

// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
// 		if (rb)	{
// 			rb->SetPosition(newPosition);
// 		}

		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetScale(glm::vec3 newScale)
	{
		m_scale = newScale;
		GetGameobject()->onComponentModified.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotation(glm::vec3 newRotation)
	{
		m_rotation = newRotation;

// 		Rigidbody* rb = GetGameobject()->GetComponent<Rigidbody>();
// 		if (rb)	{
// 			rb->SetRotation(newRotation);
// 		}
		GetGameobject()->onComponentModified.Emmit(this);
	}
}