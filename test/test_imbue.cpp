#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ImbueTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F(ImbueTest, simple){
                auto ret = if_( true_ )
                           [
                                return_(12)
                           ]
                           .else_
                           [
                                return_(23)
                           ]
                           ();
                ASSERT_EQ( 12,static_cast<int>(ret));
        }
        
        TEST_F(ImbueTest, loop){
                auto ret =
                        (_2 = 1,
                        for_( _1 = 0, _1 != 10, _1+=1)
                        [
                                _2 *= 2
                        ],
                        return_(_2)
                        )();
                ASSERT_EQ( 1024,static_cast<int>(ret));
        }
}
