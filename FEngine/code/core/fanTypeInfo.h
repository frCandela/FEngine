#pragma once

//================================================================================================================================
// TypeInfo class
//
// Allows instantiation of a class from an uint32_t id
// The class must define two macros :
// DECLARE_TYPE_INFO to add the necessary runtime type_info to the class
// REGISTER_TYPE_INFO in the cpp file to register the class to the following TypeInfo
// Abstract classes can be registered using DECLARE_ABSTRACT_TYPE_INFO & REGISTER_ABSTRACT_TYPE_INFO
// Trying to instantiate an abstract class will return nullptr
//
//		Example :
//		class MyClass {
//		public:
//			DECLARE_TYPE_INFO(MyClass)
//		};
//		REGISTER_TYPE_INFO(MyClass)	// In cpp file
//================================================================================================================================
class TypeInfo {
public:

	static uint32_t Register( const uint32_t _key, std::function<void*()> _constructor )	{
		assert( m_constructors().find(_key) == m_constructors().end());
		m_constructors()[_key] = _constructor;
		return _key;
	}

	template<typename T >
	static T * Instantiate(const uint32_t _id){
		return static_cast<T*> (m_constructors()[_id]());
	}

private:
	static std::map<uint32_t, std::function<void*()>> & m_constructors();
};


//================================================================================================================================
//================================================================================================================================
#define DECLARE_TYPE_INFO_BASE( _name )						\
	public:													\
	virtual uint32_t GetType() const{ return s_typeID; }	\
	virtual const char * GetName() const{ return s_name; }	\
	template<typename T >									\
	bool IsType() {	return T::s_typeID == GetType(); }		\
	static const char * s_name;								\
	static const uint32_t s_typeID;							\


//================================================================================================================================
//================================================================================================================================
#define DECLARE_ABSTRACT_TYPE_INFO( _name )					\
	DECLARE_TYPE_INFO_BASE( _name )							\
	static void * NewInstance(){ return nullptr;}	

//================================================================================================================================
//================================================================================================================================
#define DECLARE_TYPE_INFO( _name )						\
	DECLARE_TYPE_INFO_BASE( _name )						\
	static void * NewInstance(){ return new _name();}		


//================================================================================================================================
//================================================================================================================================
#define REGISTER_TYPE_INFO( _name )															\
	const uint32_t _name::s_typeID = TypeInfo::Register( SSID(#_name), _name::NewInstance );\
	const char * _name::s_name = #_name;	

//================================================================================================================================
//================================================================================================================================
#define REGISTER_ABSTRACT_TYPE_INFO( _name )															\
	const uint32_t _name::s_typeID = SSID(#_name);\
	const char * _name::s_name = #_name;	