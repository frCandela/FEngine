#include "fanUnitTestsCurrency.hpp"

namespace fan {
    //========================================================================================================
    //========================================================================================================
    std::vector<UnitTestDisplayResult> UnitTestsCurrency::TestAll()
    {
        std::vector<UnitTest> tests = GetTests();
        std::vector<UnitTestDisplayResult> results;
        results.reserve( tests.size() );
        for( const UnitTest& test : tests )
        {
            bool success = true;
            try
            {
                UnitTestsCurrency testObject;
                ( ( testObject ).*( test.mMethod ) )();
            }
            catch( ... )
            {
                success = false;
            }

            results.push_back( {
                    test.mName,
                    ( success ? ImGui::IconType::CHECK_SUCCESS16 : ImGui::IconType::CHECK_FAILED16 ),
                    ( success ? ImVec4( 0, 1, 0, 1 ) : ImVec4( 1, 0, 0, 1 ) )
            } );
        }
        return results;
    }
}