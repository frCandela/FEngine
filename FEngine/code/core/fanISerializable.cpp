#include "fanGlobalIncludes.h"

#include "fanISerializable.h"

namespace fan {
	const std::string ISerializable::GetIndentation(const int _indentLevel) {
		std::stringstream ss;
		for (int indentIndex = 0; indentIndex < _indentLevel; indentIndex++) {
			ss << "    ";
		} 
		return ss.str();
	}
}