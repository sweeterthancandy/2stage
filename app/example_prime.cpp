#include "dsl_compiler.h"
#include "placeholders.h"
#include "test_ctx_fixture.h"

int main(){
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;
 
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


        auto prog = debug_compile(code);
        prog.debug();
        context ctx;
        prog.execute(ctx);
        ctx.debug();

}
