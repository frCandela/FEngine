#pragma once

namespace fan {
	class ISerializable {
	public:
		virtual void Load(std::istream& _in) = 0;
		virtual void Save(std::ostream& _out) = 0;
	};
}