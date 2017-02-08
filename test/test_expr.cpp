#include "expr.h"
#include "expr_dsl.h"
#include "eval.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>


namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ExprTest = BasicCtxFixture<struct tag_test_expr>;

        TEST_F( ExprTest, AssignTest ){

                auto expr = _1 = _2 = 1;

                eval(expr, ctx );

                assert_eq( _1, 1);
                assert_eq( _2, 1);
        }
        TEST_F( ExprTest, LocalAssignTest ){

                struct _a{};
                struct _b{};

                auto a = placeholder<_a>();
                auto b = placeholder<_b>();

                auto expr = a = b = 1;

                eval(expr, ctx );

                assert_eq( a, 1);
                assert_eq( b, 1);
        }
        
        
        TEST_F( ExprTest, SimpleLiteral ){

                assert_eq( lit(1), 1 );
                assert_eq( lit(34), 34 );
                
                assert_eq( lit(1) + 1  ,  2 );

        }
        TEST_F( ExprTest, Modulus ){

                assert_eq( lit(4) % 3 , 1 );
                assert_eq( lit(23) % 2 , 1 );
                assert_eq( lit(24) % 2 , 0 );
        }


}

