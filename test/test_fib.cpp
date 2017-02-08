#include "dsl_compiler.h"
#include "placeholders.h"
#include "test_ctx_fixture.h"

#include <boost/range/algorithm.hpp>

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using FibTest = BasicCtxFixture<struct tag_test_fib>;

        TEST_F( FibTest, First20 ){

                auto max_fib = 20;

                struct tag_lag1{};
                struct tag_lag2{};
                struct tag_n{};

                auto lag1 = placeholder<tag_lag1>();
                auto lag2 = placeholder<tag_lag2>();
                auto n = placeholder<tag_n>();

                auto stmt = (
                        lag1 = 1
                      , lag2 = 1
                      , push(lag1)
                      , push(lag2)
                      , for_( n = 2, n < max_fib , n += 1 )
                        (
                                _1 = lag1 + lag2
                              , lag1 = lag2
                              , lag2 = _1
                              , push(lag2)
                        )
                );

                auto prog = compile(stmt);

                prog.execute(ctx);

                std::vector<int> known = {1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597,2584,4181,6765};
                std::vector<int> result;
                boost::for_each( ctx.get_stack(), [&result](auto _){ result.emplace_back(static_cast<int>(_));});
                ASSERT_EQ( boost::equal( known , result ), true );
        }

}


