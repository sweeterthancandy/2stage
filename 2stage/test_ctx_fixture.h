#pragma once

#include "context.h"
#include "placeholders.h"
#include <gtest/gtest.h>


        
// need to give a new tag to make it unique
template<class Tag>
struct BasicCtxFixture : ::testing::Test
{
        using scalar_t = size_t;
        static constexpr scalar_t numReg = 8;

        void SetUp()final{
                using namespace dsl_compiler::placeholders;
                ctx.assign( boost::typeindex::type_id< tag_dsl_compiler_placeholder_1>(), 0 );
                ctx.assign( boost::typeindex::type_id< tag_dsl_compiler_placeholder_2>(), 0 );
                ctx.assign( boost::typeindex::type_id< tag_dsl_compiler_placeholder_3>(), 0 );
                ctx.assign( boost::typeindex::type_id< tag_dsl_compiler_placeholder_4>(), 0 );
                ctx.assign( boost::typeindex::type_id< tag_dsl_compiler_placeholder_5>(), 0 );
        }
        void TearDown()final{
        }
protected:
        ///////////////////////////////
        // helper functions
        ///////////////////////////////
protected:
        template<class Expr>
        void assert_eq( Expr const& expr, size_t value){
                ASSERT_EQ( static_cast<size_t>(eval(expr, ctx )), value );
        }

        dsl_compiler::context ctx;
};
