#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class ISerializable {
	public:
		virtual bool Load(std::istream& _in) = 0;
		virtual bool Save(std::ostream& _out, const int _indentLevel) const = 0;
		static const std::string GetIndentation(const int _indentLevel);

	protected:
		static bool ReadEndToken		( std::istream& _in);
		static bool ReadStartToken		( std::istream& _in);
		static bool ReadSegmentHeader	( std::istream& _in);
		static bool ReadSegmentHeader	( std::istream& _in, const std::string& _expectedHeader );
		
		static bool ReadFloat			( std::istream& _in, float & _outValue);
		static bool ReadInteger			( std::istream& _in, int & _outValue);
		static bool ReadUnsigned		( std::istream& _in, uint32_t & _outValue);
		static bool ReadString			( std::istream& _in, std::string & _outStr );
		static bool ReadBool			( std::istream& _in, bool & _outBool );

		static std::string BoolToSting(const bool _bool) { return (_bool ? "true" : "false"); }
	};
}