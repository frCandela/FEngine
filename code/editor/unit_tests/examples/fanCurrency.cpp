#include "fanCurrency.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    Sum Money::operator+( const Money& _other ) const
    {
        return Sum( *this, _other );
    }

    //========================================================================================================
    //========================================================================================================
    Money Money::Reduce( const Bank& _bank, const std::string& _currency ) const
    {
        if( _currency == mCurrency ) { return *this; }
        else
        {
            const int rate = _bank.GetRate( mCurrency, _currency );
            return Money( mAmount / rate, _currency );
        }
    }

    //========================================================================================================
    //========================================================================================================
    Money Sum::Reduce( const Bank& _bank, const std::string& _currency ) const
    {
        const Money first = _bank.Reduce( mFirst, _currency );
        const Money second  = _bank.Reduce( mSecond, _currency );

        return Money( first.Amount() + second.Amount(), _currency );
    }
}