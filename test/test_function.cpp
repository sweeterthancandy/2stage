
#include "dsl_compiler.h"
#include "placeholders.h"
#include "decleration.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using FunctionTest = BasicCtxFixture<struct tag_test_function>;

        TEST_F( FunctionTest, simple){

                auto f0 = function<int>( return_(_arg1) );
                EXPECT_EQ( 1 , f0(1) );

                auto f1 = function<bool>( return_(_arg1 % 2 == 0) );
                EXPECT_TRUE( f1(0) );
                EXPECT_FALSE( f1(1) );
                EXPECT_FALSE( f1(-1) );
                EXPECT_TRUE( f1(2) );
        }

        TEST_F( FunctionTest, external ){
        }

}
