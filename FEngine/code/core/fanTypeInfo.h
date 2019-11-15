#pragma once

#include "core/fanSingleton.h"

namespace fan {
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
	class TypeInfo : public Singleton<TypeInfo> {
	private:


	public:
		friend class Singleton < TypeInfo>;		
		enum Flags{ NONE = 0, EDITOR_COMPONENT = 1 << 1, BLOUP = 1 << 2 };

		uint32_t Register(const uint32_t _key, std::function<void*()> _constructor, const uint32_t _flags = Flags::NONE );

		template<typename T >
		T * Instantiate(const uint32_t _id) {return static_cast<T*> ( m_data[_id].constructor());	}

 		template<typename T >
 		const T * GetInstance( const uint32_t _id )	{ return static_cast<T*> ( m_data[_id].instance ); }

		
		uint32_t GetFlags( const uint32_t _id ) { return m_data[_id].flags; }
		template<typename _Type >
		uint32_t GetFlags( ) { return m_data[_Type::GetType()].flags; }

		std::vector< const void * > GetInstancesWithFlags( const uint32_t _flags);

	protected:
		TypeInfo(){}

	private:
		struct TypeInfoData
		{
			std::function<void*( )> constructor;
			void* instance;
			uint32_t flags;
		};

		std::map<uint32_t, TypeInfoData> m_data;
	};


	//================================================================================================================================
	//================================================================================================================================
#define DECLARE_TYPE_INFO_BASE( _name, _parentType )		\
	public:													\
	virtual uint32_t GetType() const{ return s_typeID; }	\
	virtual const char * GetName() const{ return s_name; }	\
	template<typename T >									\
	bool IsType() {	return T::s_typeID == GetType(); }		\
	static const char * s_name;								\
	static const uint32_t s_typeID;							\
	using parentType = _parentType;							\


//================================================================================================================================
//================================================================================================================================
#define DECLARE_ABSTRACT_TYPE_INFO( _name, _parentType )	\
	DECLARE_TYPE_INFO_BASE( _name, _parentType )			\
	static void * NewInstance(){ return nullptr;}			\

//================================================================================================================================
//================================================================================================================================
#define DECLARE_TYPE_INFO( _name, _parentType )				\
	DECLARE_TYPE_INFO_BASE( _name, _parentType )			\
	static void * NewInstance(){ return new _name();}		\


//================================================================================================================================
//================================================================================================================================
#define REGISTER_TYPE_INFO( _name, _flags )															\
	const uint32_t _name::s_typeID = TypeInfo::Get().Register( SSID(#_name), _name::NewInstance, _flags );\
	const char * _name::s_name = #_name;	

//================================================================================================================================
//================================================================================================================================
#define REGISTER_ABSTRACT_TYPE_INFO( _name )															\
	const uint32_t _name::s_typeID = SSID(#_name);\
	const char * _name::s_name = #_name;	

}