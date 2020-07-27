#pragma  once

#include <string>
#include <map>
#include <functional>

namespace fan
{
    class Money;
    class Bank;

    //========================================================================================================
    //========================================================================================================
    class Expression
    {
    public:
        virtual Money Reduce( const Bank& _bank, const std::string& _currency ) const = 0;
    };

    class Sum;

    //========================================================================================================
    //========================================================================================================
    class Money : public Expression
    {
    public:
        Money( int _amount, const std::string _currency ) :
                mAmount( _amount ),
                mCurrency( _currency ) {}

        int Amount() const              { return mAmount; }
        std::string Currency() const    { return mCurrency; }
        Money Reduce( const Bank& _bank,const std::string& _currency ) const override;


        bool operator==( const Money& _money ) const
        {
            return mAmount == _money.mAmount && mCurrency == _money.mCurrency;
        }
        Money operator*( const int& _multiplier ) const { return Money( _multiplier * mAmount, mCurrency ); }
        Sum operator+( const Money& _other ) const;

        static Money Franc( const int _amount ) { return Money( _amount, "CHF" ); }
        static Money Dollar( const int _amount ) { return Money( _amount, "USD" ); }

    protected:
        int         mAmount;
        std::string mCurrency;
    };

    //========================================================================================================
    //========================================================================================================
    class Sum : public Expression
    {
    public:
        Sum( const Money& _first,const Money& _second ) :
                mFirst( _first ),
                mSecond( _second )
        {}

        Money Reduce( const Bank& _bank,const std::string& _currency ) const override;

        Money mFirst;
        Money mSecond;
    };

    //========================================================================================================
    //========================================================================================================
    class Bank
    {
    public:
        struct CurrencyPair
        {
            std::string mCurrency1;
            std::string mCurrency2;
            size_t Hash() const { return std::hash<std::string> {}( mCurrency1 + mCurrency2 ); }
            bool operator<( const CurrencyPair& _other ) const
            {
                return Hash()< _other.Hash();
            }
        };

        void AddRate( const std::string _currency1, const std::string _currency2, int _rate )
        {
            mRates[{ _currency1, _currency2}] = _rate;
        }

        int GetRate( const std::string& _currency1, const std::string& _currency2 ) const
        {
            return mRates.at( { _currency1, _currency2 } );
        }

        Money Reduce( const Expression& _expression, const std::string& _currency ) const
        {
            return _expression.Reduce( *this, _currency );
        }

    private:
        std::map<CurrencyPair,int> mRates;
    };
}


