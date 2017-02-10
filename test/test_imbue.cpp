#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ImbueTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F(ImbueTest, simple){

                ASSERT_EQ( 0, (int)return_(0)() );

                auto prog = if_( true_ )
                           [
                                return_(12)
                           ]
                           .else_
                           [
                                return_(23)
                           ];
                ASSERT_EQ( 12, (int)prog(12) );
        }
        
        TEST_F(ImbueTest, loop){
                auto prog = (
                        _2 = 1,
                        for_( _1 = 0, _1 != 10, _1+=1)
                        [
                                _2 *= 2
                        ],
                        return_(_2)
                        );
                ASSERT_EQ( 1024, (int)prog());
        }
        
        TEST_F(ImbueTest, args){
                ASSERT_EQ( 24, (int)return_(_arg1)(24) );
                ASSERT_EQ( 19, (int)return_(_arg1)(24 - 5) );
                ASSERT_TRUE( (int)return_( _arg1 == _arg2)(12,12) );
                ASSERT_FALSE( (int)return_( _arg1 == _arg2)(12,-1) );

                EXPECT_ANY_THROW( return_(_arg1)() );
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
                ASSERT_TRUE( (int)prog(0) );
        }
}
