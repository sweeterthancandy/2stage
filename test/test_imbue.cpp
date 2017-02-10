#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ImbueTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F(ImbueTest, simple){
                ASSERT_EQ( 0, static_cast<int>(( return_(0) )()));
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
        
        TEST_F(ImbueTest, args_identity){
                auto ret = 
                        (
                                return_( _arg1 )
                        )(24);
                ASSERT_EQ( 24,static_cast<int>(ret));
        }
        
        TEST_F(ImbueTest, args_single){
                auto ret = 
                        (
                                return_( _arg1 + 1 )
                        )(24);
                ASSERT_EQ( 25,static_cast<int>(ret));
        }
        TEST_F(ImbueTest, args_multiple){
                auto prog = 
                                if_( _arg1 % 2 == 0 )
                                [
                                        return_(true_)
                                ]
                                .else_
                                [
                                        return_(false_)
                                ];
                ASSERT_TRUE(  static_cast<bool>(prog(0)));
        }
}
