#pragma once
 
namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	template < typename TypeName > 
	class Singleton {
	public:
		static TypeName& GetInstance() { 
			static TypeName instance;
			return instance;
		};

		Singleton< TypeName>& operator= ( const Singleton< TypeName>& ) = delete;
		Singleton< TypeName> (const Singleton< TypeName>&) = delete;

	protected:
 		Singleton(){}
 		~Singleton(){};
	};
}