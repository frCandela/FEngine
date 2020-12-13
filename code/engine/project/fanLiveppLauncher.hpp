#pragma warning( push )
#pragma warning( disable : 4005 ) // macro redefinition
#include "LivePP/API/LPP_API.h"
#pragma warning( pop )

#include "engine/project/fanIProjectContainer.hpp"

namespace fan
{
    //========================================================================================================
    // Runs the engine and connects it to the Live++ hot reload
    //========================================================================================================
    class LPPLauncher
    {
    public:
        LPPLauncher()
        {
            mLivePP = lpp::lppLoadAndRegister( L"middleware/LivePP/", "fanEngine" );
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