#pragma once
 
namespace fan {
	//================================================================================================================================
	// Classes inheriting from singleton cannot have a constructor
	// Override Init to initialize a class inheriting from singleton
	//================================================================================================================================
	template < typename TypeName > 
	class Singleton {
	public:		
		static TypeName& Get() { 
			static TypeName instance;
			return instance;
		};

	protected:
		Singleton() {}
		Singleton< TypeName>& operator= (const Singleton< TypeName>&) = delete;
		Singleton< TypeName>(const Singleton< TypeName>&) = delete;
	};
}