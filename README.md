This is a small dsl, similar to boost-lambda or boost-pheonix, but instead compiles
into a flat statement code, which allows for the break_ expression, before
is an example


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

output

         0    tag_expr(assign(register_(main::tag_i),literal(3)))
         1    tag_placeholder(3)
         2    tag_goto_if(logical_not(less_equal(register_(main::tag_i),literal(100))),4)
         3    tag_goto_if(logical_not(equal_to(modulus(register_(main::tag_i),literal(2)),literal(0))),5)
         4    tag_goto(2)
         5    tag_placeholder(5)
         6    tag_expr(assign(register_(main::tag_flag),literal(0)))
         7    tag_expr(assign(register_(main::tag_j),literal(3)))
         8    tag_placeholder(8)
         9    tag_goto_if(logical_not(less_equal(multiplies(register_(main::tag_j),register_(main::tag_j)),register_(main::tag_i))),9)
        10    tag_goto_if(logical_not(equal_to(modulus(register_(main::tag_i),register_(main::tag_j)),literal(0))),10)
        11    tag_expr(assign(register_(main::tag_flag),literal(1)))
        12    tag_goto(6)
        13    tag_placeholder(10)
        14    tag_placeholder(7)
        15    tag_expr(assign(register_(main::tag_j),plus(register_(main::tag_j),literal(2))))
        16    tag_goto(8)
        17    tag_placeholder(9)
        18    tag_placeholder(6)
        19    tag_goto_if(logical_not(not_equal_to(register_(main::tag_flag),literal(1))),11)
        20    tag_push(register_(main::tag_i))
        21    tag_placeholder(11)
        22    tag_placeholder(2)
        23    tag_expr(assign(register_(main::tag_i),plus(register_(main::tag_i),literal(1))))
        24    tag_goto(3)
        25    tag_placeholder(4)
        26    tag_placeholder(1)
        ----------------
         0  expr(assign(register_(main::tag_i),literal(3)))
         1  goto_if(logical_not(less_equal(register_(main::tag_i),literal(100))),16)
         2  goto_if(logical_not(equal_to(modulus(register_(main::tag_i),literal(2)),literal(0))),4)
         3  goto_(14)
         4  expr(assign(register_(main::tag_flag),literal(0)))
         5  expr(assign(register_(main::tag_j),literal(3)))
         6  goto_if(logical_not(less_equal(multiplies(register_(main::tag_j),register_(main::tag_j)),register_(main::tag_i))),12)
         7  goto_if(logical_not(equal_to(modulus(register_(main::tag_i),register_(main::tag_j)),literal(0))),10)
         8  expr(assign(register_(main::tag_flag),literal(1)))
         9  goto_(12)
        10  expr(assign(register_(main::tag_j),plus(register_(main::tag_j),literal(2))))
        11  goto_(6)
        12  goto_if(logical_not(not_equal_to(register_(main::tag_flag),literal(1))),14)
        13  push(register_(main::tag_i))
        14  expr(assign(register_(main::tag_i),plus(register_(main::tag_i),literal(1))))
        15  goto_(1)
        ----------------
        {(main::tag_i => 101), (main::tag_j => 3), (main::tag_flag => 1), }{3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, }
