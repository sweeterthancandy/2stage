#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ImbueTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F( ImbueTest, simple){

                if_(false_)[
                        _1 = 1
                ]
        }

}
