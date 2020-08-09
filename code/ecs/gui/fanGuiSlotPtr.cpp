#include "ecs/gui/fanGuiSlotPtr.hpp"
#include "scene/components/fanSceneNode.hpp"
#include <sstream>

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    void FanGuiSlotPtr::GenerateFrom( fan::SlotPtr& _ptr )
    {
        if( _ptr.IsValid() )
        {
            mEntity = mWorld.GetEntity( _ptr.Data().mHandle );
            mSceneNodeName = mWorld.GetComponent<fan::SceneNode>( mEntity ).name;
            GenerateComponentInfos( mComponentsInfo );
            mComponentIndex = GetComponentIndex( _ptr );
            mComboComponentsStr = GenerateStrComboComponents();
            mComboSlotsStr = GenerateStrComboSlots();
            mSlotIndex = GetSlotIndex( _ptr );
        }
        else
        {
            mSceneNodeName         = "null";
            mComponentIndex        = 0;
            mSlotIndex          = 0;
            mComboComponentsStr = GetNullString();
            mComboSlotsStr      = GetNullString();
        }
    }

    //========================================================================================================
    //========================================================================================================
    std::string FanGuiSlotPtr::GetNullString()
    {
        std::string str;
        str = "null00";
        str[str.size() - 1] = '\0';
        str[str.size() - 2] = '\0';
        return str;
    }

    //========================================================================================================
    //========================================================================================================
    void FanGuiSlotPtr::GenerateComponentInfos( std::vector<const fan::EcsComponentInfo*>& _infos ) const
    {
        _infos.clear();
        _infos.reserve( mWorld.NumComponents() );
        for( int i = 0; i < mWorld.NumComponents(); ++i )
        {
            if( mWorld.IndexedHasComponent( mEntity, i ) )
            {
                const fan::EcsComponentInfo& info = mWorld.IndexedGetComponentInfo( i );
                _infos.push_back( &info );
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    std::string FanGuiSlotPtr::GenerateStrComboComponents() const
    {
        std::stringstream ssComponentsCombo;
        for( const fan::EcsComponentInfo* info : mComponentsInfo )
        {
            ssComponentsCombo << info->name << '\0';
        }
        ssComponentsCombo << '\0';
        return ssComponentsCombo.str();
    }

    //========================================================================================================
    //========================================================================================================
    std::string FanGuiSlotPtr::GenerateStrComboSlots() const
    {
        std::stringstream ssSlots;
        if( !mComponentsInfo[mComponentIndex]->mSlots.empty() )
        {
            for( int i = 0; i < mComponentsInfo[mComponentIndex]->mSlots.size(); i++ )
            {
                const fan::SlotBase* slot = mComponentsInfo[mComponentIndex]->mSlots[i];
                ssSlots << slot->mName << '\0';
            }
            ssSlots << '\0';
            return ssSlots.str();
        }
        return GetNullString();
    }

    //========================================================================================================
    //========================================================================================================
    int  FanGuiSlotPtr::GetComponentIndex( const fan::SlotPtr& _ptr ) const
    {
        // Get components
        for( int i = 0; i < mComponentsInfo.size(); ++i )
        {
            const fan::EcsComponentInfo* info = mComponentsInfo[i];
            if( info->type == _ptr.Data().mComponentType )
            {
                return i;
            }
        }
        return -1;
    }

    //========================================================================================================
    //========================================================================================================
    int FanGuiSlotPtr::GetSlotIndex( const fan::SlotPtr& _ptr ) const
    {
        for( int i = 0; i < mComponentsInfo[mComponentIndex]->mSlots.size(); i++ )
        {
            const fan::SlotBase* slot = mComponentsInfo[mComponentIndex]->mSlots[i];
            if( _ptr.Data().mSlot == slot )
            {
                return i;
            }
        }
        return 0;
    }
}