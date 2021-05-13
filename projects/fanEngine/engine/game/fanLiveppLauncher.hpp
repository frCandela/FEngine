#ifdef FAN_LIVEPP

#include "fanDisableWarnings.hpp"

WARNINGS_LIVEPP()
#include "LivePP/API/LPP_API.h"

WARNINGS_POP()

#include "engine/game/fanPlayerData.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Runs the engine and connects it to the Live++ hot reload
    //==================================================================================================================================================================================================
    class LPPLauncher
    {
    public:
        LPPLauncher( const std::string& _appName, Signal<>& _onLPPSync ) : mOnLPPSync( _onLPPSync )
        {
            mLivePP = lpp::lppLoadAndRegister( FAN_LIVEPP_PATH, _appName.c_str() );
            lpp::lppEnableAllCallingModulesSync( mLivePP );
            lpp::lppInstallExceptionHandler( mLivePP );
            mOnLPPSync.Connect( &LPPLauncher::OnSync, this );
        }

        ~LPPLauncher()
        {
            mOnLPPSync.Disconnect( (size_t)this );
            ::FreeLibrary( mLivePP );
        }

    private:
        void OnSync() { lpp::lppSyncPoint( mLivePP ); }

        HMODULE mLivePP;
        Signal<>& mOnLPPSync;
    };
}
#endif