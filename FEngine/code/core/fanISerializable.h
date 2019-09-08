#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class ISerializable {
	public:
		virtual void Load(std::istream& _in) = 0;
		virtual void Save(std::ostream& _out, const int _indentLevel) = 0;
		static const std::string GetIndentation(const int _indentLevel);
	};
}