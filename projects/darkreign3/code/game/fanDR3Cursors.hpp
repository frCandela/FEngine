#pragma once

#include <cstdint>
#include "engine/resources/fanCursor.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
    class Resources;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DR3Cursors
    {
        enum
        {
            Move1 = 0,
            Move2,
            Move3,
            Mouse0,
            Mouse1,
            RallyPoint,
            Nope,
            Attack1,
            Attack2,
            Attack3,
            Attack4,
            Count
        };

        ResourcePtr<Cursor> mCursors[ Count ];
        static const constexpr char * sIconsPath = "images/cursors/cursors2.png";

        void Load( Resources& _resources );
    };
}