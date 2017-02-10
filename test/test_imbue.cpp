#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>

namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using ImbueTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F(ImbueTest, simple){
                ASSERT_EQ( 12,static_cast<int>(( if_( true_)[
                                        return_(12)
                                ].else_[
                                        return_(23)
                                ]  )() ));
        }
}
