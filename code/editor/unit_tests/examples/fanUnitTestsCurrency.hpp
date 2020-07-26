#pragma once

#include <vector>
#include <string>
#include <exception>
#include "fanCurrency.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
#define TEST_ASSERT( _condition )               \
        if( ! (_condition) ) {                  \
            __debugbreak();                     \
            throw std::exception("test failed");                \
        }

    //========================================================================================================
    //========================================================================================================
    struct UnitTestDisplayResult
    {
        std::string     mName;
        ImGui::IconType mIcon;
        ImVec4          mColor;
    };

    //========================================================================================================
    //========================================================================================================
    class UnitTestsCurrency
    {
    public:
        struct UnitTest
        {
            using TestMethod = void ( UnitTestsCurrency::* )();
            TestMethod  mMethod;
            std::string mName;
        };

        static std::vector<UnitTestDisplayResult> TestAll();

        static std::vector<UnitTest> GetTests()
        {
            return {
                    { &UnitTestsCurrency::TestEquality,                 "equality"        },
                    { &UnitTestsCurrency::TestCurrency,                 "currency"        },
                    { &UnitTestsCurrency::TestMultiplication,           "multiplication"  },
                    { &UnitTestsCurrency::TestAddition,                 "addition"        },
                    { &UnitTestsCurrency::TestReduceSum,                "reduce sum"      },
                    { &UnitTestsCurrency::TestReduceMoney,              "reduce money"    },
                    { &UnitTestsCurrency::TestReduceDifferentCurrency,  "reduce money different currency" },
                    { &UnitTestsCurrency::TestMixedAddition,            "mixed addition" },
            };
        }

        void TestMultiplication()
        {
            TEST_ASSERT( Money::Dollar( 5 ) * 2 == Money::Dollar( 10 ) );
            TEST_ASSERT( Money::Dollar( 5 ) * 3 == Money::Dollar( 15 ) );
            TEST_ASSERT( Money::Franc( 5 ) * 2 == Money::Franc( 10 ) );
            TEST_ASSERT( Money::Franc( 5 ) * 3 == Money::Franc( 15 ) );
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

        void TestCurrency()
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