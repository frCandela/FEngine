#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "editor/fanEditor.hpp"

//================================================================================================================================
// Runs the engine and connects it to the Live++ hot reload
//================================================================================================================================
class LPPMain {
public:

	LPPMain()
	{
#ifndef NDEBUG
		livePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
		lpp::lppEnableAllCallingModulesSync( livePP );
		engine.onLPPSynch.Connect( &LPPMain::OnSynch, this );
		engine.Run();
		::FreeLibrary( livePP );
#else
		engine.Run();
#endif // !NDEBUG
	}

	void OnSynch() {
		lpp::lppSyncPoint(livePP);
	}

private:
	HMODULE livePP;
	fan::Engine engine;
};

int main()
{
	LPPMain main;
	return 0;
}


