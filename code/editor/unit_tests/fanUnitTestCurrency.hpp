#pragma once

#include "core/fanUnitTest.hpp"
#include "fanCurrency.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestCurrency : public UnitTest< UnitTestCurrency >
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return  {
                    { &UnitTestCurrency::TestEquality,                "equality"        },
                    { &UnitTestCurrency::TestCurrencyName,            "currency"        },
                    { &UnitTestCurrency::TestMultiplication,          "multiplication"  },
                    { &UnitTestCurrency::TestAddition,                "addition"        },
                    { &UnitTestCurrency::TestReduceSum,               "reduce sum"      },
                    { &UnitTestCurrency::TestReduceMoney,             "reduce money"    },
                    { &UnitTestCurrency::TestReduceDifferentCurrency, "reduce money different currency" },
                    { &UnitTestCurrency::TestMixedAddition,           "mixed addition" },
            };
        }

        void Create() override {}
        void Destroy() override {}

        void TestMultiplication()
        {
            TEST_ASSERT( Money::Dollar( 5 ) * 2 == Money::Dollar( 10 ) );
            TEST_ASSERT( Money::Dollar( 5 ) * 3 == Money::Dollar( 15 ) );
            TEST_ASSERT( Money::Franc( 5 ) * 2 == Money::Franc( 10 ) );
            TEST_ASSERT( Money::Franc( 5 ) * 3 == Money::Franc( 15 ) );
            TEST_ASSERT( false );
        }

        void TestAddition()
        {
            Money      five    = Money::Dollar( 5 );
            Sum sum  = five + five;
            TEST_ASSERT( sum.mFirst == five );
            TEST_ASSERT( sum.mSecond == five );
        }

        void TestReduceSum() {
            Money      five    = Money::Dollar( 5 );
            Sum result  = five + five;
            Bank       bank;
            Money      reduced = bank.Reduce( result, "USD" );
            TEST_ASSERT( Money::Dollar( 10 )  == reduced );
        }

        void TestReduceMoney() {
            Bank       bank;
            Money      five    = Money::Dollar( 5 );
            TEST_ASSERT( bank.Reduce( five, "USD" ) == Money::Dollar(5) );
        }

        void TestEquality()
        {
            TEST_ASSERT( Money::Dollar( 5 ) == Money::Dollar( 5 ) );
            TEST_ASSERT( !( Money::Dollar( 5 ) == Money::Dollar( 6 ) ) );
            TEST_ASSERT( !( Money::Franc( 5 ) == Money::Dollar( 5 ) ) );
        }

        void TestCurrencyName()
        {
            TEST_ASSERT( Money::Dollar(1).Currency() == "USD" );
            TEST_ASSERT( Money::Franc(1).Currency() == "CHF" );
        }

        void TestReduceDifferentCurrency()
        {
            Bank bank;
            bank.AddRate( "CHF", "USD", 2 );
            Money result = bank.Reduce( Money::Franc( 2 ), "USD" );
            TEST_ASSERT( Money::Dollar( 1 ) == result );
        }

        void TestMixedAddition()
        {
            Money fiveDollars = Money::Dollar( 5 );
            Money tenFrancs = Money::Franc( 10 );
            Bank  bank;
            bank.AddRate( "CHF", "USD", 2 );
            Money result = bank.Reduce( fiveDollars + tenFrancs , "USD" );
            TEST_ASSERT( Money::Dollar( 10 ) == result );
        }
    };
}