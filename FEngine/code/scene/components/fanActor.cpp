#include "fanGlobalIncludes.h"

#include "scene/components/fanActor.h"
#include "scene/components/fanComponent.h"
#include "scene/fanEntity.h"
#include "core/fanSignal.h"
#include "fanEngine.h"

namespace fan
{
	Signal< Actor * > Actor::onActorAttach;
	Signal< Actor * > Actor::onActorDetach;

	//================================================================================================================================
	//================================================================================================================================
	void Actor::OnAttach() {
		onActorAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Actor::OnDetach() {
		onActorDetach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Actor::OnGui() {
		Component::OnGui();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Actor::Load(std::istream& _in) {
		if (!ReadSegmentHeader(_in, "isEnabled:")) { return false; }
		if (!ReadBool(_in, m_isEnabled)) { return false; }
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	bool Actor::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "isEnabled: " << BoolToSting(m_isEnabled) << std::endl;
		return true;
	}
}