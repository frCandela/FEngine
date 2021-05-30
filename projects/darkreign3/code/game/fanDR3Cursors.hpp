#pragma once

#include <cstdint>
#include "engine/resources/fanCursor.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    class Resources;
    struct Time;
    struct SelectionStatus;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    enum DR3Cursor
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DR3Cursors
    {
        void Load( Resources& _resources );
        static DR3Cursor GetCurrentCursor( const Fixed _delta, const Time& _time,  const SelectionStatus& _selectionStatus );

        static const constexpr char * sIconsPath = "images/cursors/cursors2.png";
        ResourcePtr<Cursor> mCursors[ Count ];
    };
}