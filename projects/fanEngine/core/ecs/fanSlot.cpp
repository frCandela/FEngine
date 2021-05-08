#include "core/ecs/fanSlot.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace  fan
{
    //========================================================================================================
    //========================================================================================================
    SlotPtr::SlotPtr()
    {
        mCallData = new SlotCallData();
    }

    //========================================================================================================
    //========================================================================================================
    SlotPtr::~SlotPtr()
    {
        delete mCallData;
        mCallData = nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Clear()
    {
        mCallData->mHandle        = 0;
        mCallData->mType = 0;
        mCallData->mSlot          = nullptr;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::Init( EcsWorld& _world, int _argsType )
    {
        mWorld = &_world;
        mArgsType = _argsType;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetComponentSlot( EcsHandle _handle, uint32_t _componentType, SlotBase* _slot )
    {
        fanAssert( mWorld->SafeGetComponentInfo( _componentType ) != nullptr );
        mCallData->mHandle = _handle;
        mCallData->mType = _componentType;
        mCallData->mSlot = _slot;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetComponentSlot( EcsHandle _handle, uint32_t _componentType, const std::string& _slotName )
    {
        SlotBase* slot = nullptr;
        if( _componentType != 0 && !_slotName.empty() )
        {
            const EcsComponentInfo* info = mWorld->SafeGetComponentInfo( _componentType );
            if( info != nullptr )
            {
                for( SlotBase* slotBase : info->mSlots )
                {
                    if( slotBase->mName == _slotName )
                    {
                        slot = slotBase;
                        break;
                    }
                }
            }
        }
        SetComponentSlot( _handle, _componentType, slot );
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetSingletonSlot( uint32_t _singletonType, SlotBase* _slot )
    {
        fanAssert( _singletonType == 0 || mWorld->SafeGetSingletonInfo( _singletonType ) != nullptr );
        mCallData->mHandle = 0;
        mCallData->mType = _singletonType;
        mCallData->mSlot = _slot;
    }

    //========================================================================================================
    //========================================================================================================
    void SlotPtr::SetSingletonSlot( uint32_t _singletonType, const std::string& _slotName )
    {
        SlotBase* slot = nullptr;
        if( _singletonType != 0 && !_slotName.empty() )
        {
            const EcsSingletonInfo* info = mWorld->SafeGetSingletonInfo( _singletonType );
            if( info != nullptr )
            {
                for( SlotBase* slotBase : info->mSlots )
                {
                    if( slotBase->mName == _slotName )
                    {
                        slot = slotBase;
                        break;
                    }
                }
            }
        }
        SetSingletonSlot( _singletonType, slot );
    }
}