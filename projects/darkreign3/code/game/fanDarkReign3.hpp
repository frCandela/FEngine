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
        void Step( const Fixed _delta ) override;
        void Render() override;
        void OnGui() override;
        EcsWorld& GetWorld() override { return mWorld; }

        void StepLoadTerrain();

        EcsWorld mWorld;

        DR3Cursors mCursors;
    };
}
