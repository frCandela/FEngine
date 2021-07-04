#pragma once

#include "engine/game/fanIGame.hpp"
#include "game/fanDR3Cursors.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //                     _\|/_
    //                     (O O)
    //       --------oOO----(_)---------------
    //
    //             If you can read this
    //            then Dark Reign 2 code
    //            have been copy pasted
    //
    //       ----------------------oOO--------
    //                    |__|__|
    //                     || ||
    //                    ooO Ooo
    // ==================================================================================================================================================================================================
    struct DarkReign3 : IGame
    {
        void Init() override;
        void Start() override;
        void Stop() override;
        void PreStep( const Fixed _delta ) override;
        void Step( const Fixed _delta ) override;
        void OnGui() override;
        EcsWorld& GetWorld() override { return mWorld; }

        void OnEditorUseGameCamera();
        void OnTogglePause();
        void OnPause();
        void OnResume();
        void OnQuit();

        EcsWorld mWorld;
        DR3Cursors mCursors;

        bool mPaused;
    };
}
