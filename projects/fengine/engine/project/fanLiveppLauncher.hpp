#ifdef FAN_LIVEPP

#include "fanDisableWarnings.hpp"
WARNINGS_LIVEPP()
#include "LivePP/API/LPP_API.h"
WARNINGS_POP()

#include "engine/project/fanIProjectContainer.hpp"

namespace fan
{
    //==========================================================================================================================
    // Runs the engine and connects it to the Live++ hot reload
    //==========================================================================================================================
    class LPPLauncher
    {
    public:
        LPPLauncher ( const std::string& _appName )
        {
            mLivePP = lpp::lppLoadAndRegister( FAN_LIVEPP_PATH, _appName.c_str() );
            lpp::lppEnableAllCallingModulesSync( mLivePP );
        }

        ~LPPLauncher(){  ::FreeLibrary( mLivePP );}

        void Run( IProjectContainer& _projectContainer  )
        {
            _projectContainer.mOnLPPSynch.Connect( &LPPLauncher::OnSync, this );
            _projectContainer.Run();
            _projectContainer.mOnLPPSynch.Disconnect( (size_t)this );
        }

    private:
        void OnSync() { lpp::lppSyncPoint( mLivePP ); }

        HMODULE            mLivePP;
    };
}

#endif