#include "dsl_compiler.h"
#include "placeholders.h"
#include "test_ctx_fixture.h"

#include <boost/range/algorithm.hpp>

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using PrimeTest = BasicCtxFixture<struct tag_test_stmt>;

        TEST_F( PrimeTest, First100 ){

                auto max_prime = 100;

                auto code = 
                        for_( _1 = 3 , _1 <= max_prime , _1 += 2)
                        (
                                _3 = 0
                              , for_( _2 = 3, _2*_2 <= _1 , _2 += 2 )
                                (
                                        if_( _1 % _2 == 0 )
                                        (
                                                _3 = 1
                                              , break_ 
                                        )
                                )
                              , if_( _3 != 1 )
                                (
                                        push(_1)
                                )
                        )
                ;
                auto prog = compile(code);

                prog.execute(ctx);
        
                std::vector<int> known = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
                std::vector<int> result;
                boost::for_each( ctx.get_stack(), [&result](auto _){ result.emplace_back(static_cast<int>(_));});
                ASSERT_EQ( boost::equal( known , result ), true );
        }
        TEST_F( PrimeTest, First100Square ){

                auto max_prime = 100;

                DSL_COMPILER_make_placeholder( i );
                DSL_COMPILER_make_placeholder( j );
                DSL_COMPILER_make_placeholder( flag );

                auto code = 
                        for_( i = 3 , i <= max_prime , i += 1)
                        [
                                if_( i % 2 == 0 )
                                [
                                        continue_
                                ]
                              , flag = 0
                              , for_( j = 3, j*j <= i , j += 2 )
                                [
                                        if_( i % j == 0 )
                                        [
                                                flag = 1
                                              , break_ 
                                        ]
                                ]
                              , if_( flag != 1 )
                                [
                                        push(i)
                                ]
                        ]
                ;
                auto prog = compile(code);

                prog.execute(ctx);
        
                std::vector<int> known = {3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
                std::vector<int> result;
                boost::for_each( ctx.get_stack(), [&result](auto _){ result.emplace_back(static_cast<int>(_));});
                ASSERT_EQ( boost::equal( known , result ), true );
        }

}


