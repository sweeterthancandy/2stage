

                   2stage - The exprimental c++ dsl
                   --------------------------------


This was a little project with was the result of several other projects
that I done. Firsty by using boost-variant with C++11 lambdas, tree 
manipulation is incredibly easy, and I can playing around with writing
small expression dls using boost variant. Secondly, with C++11 extentions
to C++03 sfinae semantics, and using std::void_t/std::enable_if_t, meta-
programming is much simpler/produtive.
        Using C++14 is pretty straight forward for a modern programmer
to host arthmetic expression in C++, similar to boost-proto/boost-lambda.
Consider C++03 boost-lambda magic.

        std::for_each( v.begin(), vec.end(), std::cout << if_( _1 ==0, _1, _1 + 2) );

This creates the expresion tree
                
                     
                     binary-operator("<<")
                         /      \
                     /              \
           variable(std::cout)           if
                                        / |  \ 
                                   /      |     \
                              /           |         \
                         /                |            \
             binary-operator("==")   placeholder(0)  binary-opoerator("+")
                /       \                                    /       \          
            /            \                               /            \         
  placeholder(1)       constant(0)             placeholder(1)       constant(2) 
  
        

Now this kind of expression is simple to evaulate, it could be expressed
as a DAG, and it wasn't too much of a challage. However this is just
a representing a statement, what I really want to do is express a 
program, with nested 'break'-ing from 'for' loops. below is an example
of this


Therer's no simple way to evaluate this using the techniques for evaulating
expressions. Thus this project is called '2stage', because it first 
compiles the expression tree into a representation like assembaly like
a real compiler.

        
        +-----------------+        +------------------+         +-----------+
        | C++ expression  | =====> |Intermediate form | ======> | Assembaly |
        +-----------------+        +------------------+         +-----------+


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
















        

