#include "funamental.h"
#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{

        using namespace dsl_compiler;

        using FunamentalTest = BasicCtxFixture<struct tag_test_funamental>;

        TEST_F( FunamentalTest, Modulus ){
                funamental_t lp{23};
                funamental_t rp{4};

                ASSERT_EQ( static_cast<int>(lp % rp ), 3 );
        }
}


