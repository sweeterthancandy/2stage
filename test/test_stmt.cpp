#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>


namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using StmtTest = BasicCtxFixture<struct tag_test_stmt>;

        TEST_F( StmtTest, BasicTest ){

                auto e = _1 = 1;

                auto stmt = expr(e);
                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1 , 1 );
                assert_eq( _2 , 0 );

        }
        
        TEST_F( StmtTest, IfTest ){

                auto stmt = if_( 
                        true_
                      , _1 = 1
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1 , 1 );
        }
        TEST_F( StmtTest, IfTestNested ){

                auto stmt = if_( 
                        _1 == 0
                      , if_(
                                _2 == 0
                              , _3 = 1
                        )
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _3 , 1 );

        }
        TEST_F( StmtTest, StmtsTest){
                auto prog = compile( stmts( _1 = 1, _2 = 2 ) );
                prog.execute( ctx );
                assert_eq( _1 , 1 );
                assert_eq( _2 , 2 );
        }
        TEST_F( StmtTest, WhileTest){

                auto stmt = while_( 
                        _1 != 10
                      , stmts( _2 += 2, _1 += 1 )
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _2 , 20 );

        }
        
        TEST_F( StmtTest, ForTest){

                auto stmt = for_( 
                        _1 = 0
                      , _1 != 10
                      , _1 += 1
                      , _2 += 2
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1 , 10 );
                assert_eq( _2 , 20 );

        }
        
        TEST_F( StmtTest, ForNestedTest){

                auto stmt = for_( 
                        _1 = 0
                      , _1 != 10
                      , _1 += 1
                      , for_(
                                _2 = 0
                              , _2 != 10
                              , _2 += 1
                              , _3 += 1
                        )
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1 , 10 );
                assert_eq( _2 , 10 );
                assert_eq( _3 , 100 );

        }

        
        TEST_F( StmtTest, BreakTest){

                auto stmt = for_( 
                        _1 = 0
                      , _1 != 10
                      , _1 += 1
                      , break_
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1, 0 );
        }
        TEST_F( StmtTest, WhileContinueTest){
                
                auto stmt = 
                        while_( true_,
                                stmts(
                                        if_( _1 == 2 , break_ )
                                      , _1 += 1
                                      , continue_
                                      , _2 = 1
                                )
                        );

                auto prog = compile(stmt);
                prog.execute( ctx );
                assert_eq( _1, 2 );
                assert_eq( _2, 0 );

        }
        TEST_F( StmtTest, ForContinueTest){
                
                auto stmt = for_(
                        _1 = 0
                      , _1 != 2
                      , _1 += 1
                      , stmts(
                              _2 = _1
                            , continue_
                            , _3 = 1
                        )
                );

                auto prog = compile(stmt);
                prog.execute( ctx );
                assert_eq( _1, 2 );
                assert_eq( _2, 1 );
                assert_eq( _3, 0 );

        }

        TEST_F( StmtTest, IfElseTrue ){
                auto prog = compile(
                        if_else(
                                true_
                              , _1 = true_
                              , _1 = false_
                        )
                );
                prog.execute(ctx);
                assert_eq( _1, true);
        }
        TEST_F( StmtTest, IfElseFalse ){
                auto prog = compile(
                        if_else(
                                false_
                              , _1 = true_
                              , _1 = false_
                        )
                );
                prog.execute(ctx);
                assert_eq( _1, false);
        }
        
        TEST_F( StmtTest, BreakAdvTest){

                auto stmt = for_( 
                        _1 = 0
                      , _1 != 10
                      , _1 += 1
                      , if_(  _1 == 5, break_ )
                );

                auto prog = compile(stmt);
                prog.execute( ctx );

                assert_eq( _1, 5 );
        }

        TEST_F( StmtTest, PushTest){
                auto stmt = push( lit(23) );
                auto prog = compile(stmt);
                prog.execute(ctx);

                auto const& stack = ctx.get_stack();
                ASSERT_EQ( stack.size(), 1 );
                ASSERT_EQ( static_cast<int>(stack[0]), 23 );
        }
        TEST_F( StmtTest, IfTrueTest ){
                auto stmt = if_( true_ , stmts( _1 = 1, _2 = 2, _3 = 3 ) );
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );
                assert_eq( _3, 3 );
        }
        
        TEST_F( StmtTest, return_ ){
                auto stmt = stmts( _1 = 3, _2 = 2, return_(_1), _2 = 1 );
                auto prog = compile(stmt);
                prog.execute(ctx);
                prog.debug();
                auto ret = ctx.get_return();
                assert_eq( _1, 3 );
                assert_eq( _2, 2 );
                ASSERT_EQ( 3, static_cast<size_t>(ret) );
        }

}
