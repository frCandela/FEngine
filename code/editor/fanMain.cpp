#include "LivePP/API/LPP_API.h"
#include "editor/fanEditor.hpp"

//================================================================================================================================
// Runs the engine and connects it to the Live++ hot reload
//================================================================================================================================
class LPPMain {
public:

	LPPMain()
	{
		livePP = lpp::lppLoadAndRegister(L"deps/LivePP/", "AGroupName");
		lpp::lppEnableAllCallingModulesSync(livePP);		
		engine.onLPPSynch.Connect(&LPPMain::OnSynch, this);

		engine.Run();

		::FreeLibrary(livePP);
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


