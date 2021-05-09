#pragma once

namespace fan
{
    //==================================================================================================================================================================================================
    // Base class for resources ( Mesh, textures etc.)
    //==================================================================================================================================================================================================
    class Resource
    {
    public:
        bool IsReferenced() const { return mRefCount > 0; }
        int GetRefCount() const { return mRefCount; }

        void IncreaseRefCount() { mRefCount++; }
        void DecreaseRefCount() { mRefCount--; }
    private:
        int mRefCount = 0;
    };
}