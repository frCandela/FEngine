#pragma  once

#include "fanGlm.hpp"
#include "core/resources/fanResource.hpp"

struct GLFWcursor;

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Cursor : public Resource
    {
    FAN_RESOURCE( Cursor );

        void CreateStandard( const int _GLFW_cursorType );
        void Create( uint8_t* _pixels, const glm::ivec2 _size, const glm::ivec2 _hotSpot );
        void Destroy();

        GLFWcursor* mCursor = nullptr;
    };
}