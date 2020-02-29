#include "fanEntityWorld.hpp"

namespace fan
{
	ComponentID PositionComponent::s_typeID = 0;
	ComponentID ColorComponent::s_typeID = 0;

	const char* PositionComponent::s_typeName = "Position";
	const char* ColorComponent::s_typeName = "Color";

	SingletonComponentID sc_sunLight::s_typeID = -1;
	const char* sc_sunLight::s_typeName = "sunlight";
}
