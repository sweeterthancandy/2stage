#include "dsl_compiler.h"
#include "placeholders.h"

#include "test_ctx_fixture.h"

#include <gtest/gtest.h>


namespace{
        using namespace dsl_compiler;
        using namespace dsl_compiler::placeholders;

        using DslTest = BasicCtxFixture<struct tag_test_dsl>;

        TEST_F( DslTest, DslIfTrueTest ){
                auto stmt = if_( true_ )( _1 = 1, _2 = 2, _3 = 3 );
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );
                assert_eq( _3, 3 );
        }
        TEST_F( DslTest, DslIfFalseTest ){
                auto stmt = if_( false_ )( _1 = 1, _2 = 2 );
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 0 );
                assert_eq( _2, 0 );
                assert_eq( _3, 0 );
        }
        TEST_F( DslTest, DslForTest ){
                auto stmt = for_( _1 = 0, _1 != 10, _1 += 1 )( _2 += 2, _3 += 3 );
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 10 );
                assert_eq( _2, 20 );
                assert_eq( _3, 30 );
        }
        TEST_F( DslTest, DslNestedIfTest){
                auto stmt = if_( true_ )( _1 = 1, if_( true)( _2 = 2, _3 = 3 ) );
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );
                assert_eq( _3, 3 );
        }
        TEST_F( DslTest, DslNestedForTest){
                auto stmt = for_( _1 = 0, _1 != 10, _1 += 1 )
                            (
                                for_( _2 = 0, _2 != 10, _2 += 1 )
                                ( 
                                        _3 += 3
                                      , _4 += 4
                                )
                              , _5 += 5
                            )
                ;
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 10 );
                assert_eq( _2, 10 );
                assert_eq( _3, 300 );
                assert_eq( _4, 400 );
                assert_eq( _5, 50 );
        }
        TEST_F( DslTest, DslWhile ){
                auto prog = compile(
                        while_( _1 != 10 )
                        (
                                _1 += 1
                              , _2 += 2
                        )
                );
                prog.execute(ctx);
                assert_eq( _1, 10 );
                assert_eq( _2, 20 );
        }
        TEST_F( DslTest, Comma){

                auto stmt = ( _1 = 1, _2 = 2) ;
                
                auto prog = compile( stmt );
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );

        }
        TEST_F( DslTest, IfSquare){
                auto stmt = if_( true_) [ _1 = 1, _2 = 2 ];
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );
        }
        TEST_F( DslTest, ForSquare){
                auto stmt = for_( _1 = 0, _1 != 10, _1 += 1)
                            [
                                _2 += 2
                              , _3 += 3
                            ];
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 10 );
                assert_eq( _2, 20 );
                assert_eq( _3, 30 );
        }
        TEST_F( DslTest, DslNestedForSquare){
                auto stmt = for_( _1 = 0, _1 != 10, _1 += 1 )
                            [
                                for_( _2 = 0, _2 != 10, _2 += 1 )
                                [ 
                                        _3 += 3
                                      , _4 += 4
                                ]
                              , _5 += 5
                            ]
                ;
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 10 );
                assert_eq( _2, 10 );
                assert_eq( _3, 300 );
                assert_eq( _4, 400 );
                assert_eq( _5, 50 );
        }
        TEST_F( DslTest, DslWhileSquare){
                using a = decltype( detail_::make_stmt_( _1 = 1 ) );
                using b = decltype( detail_::make_stmt_( if_( true_ )[ _2 = 2 ] ) );
                boost::ignore_unused<a,b>();
                auto stmt = while_(true_)
                            [
                                _1 = 1
                              , if_( true_ )[ _2 = 2 ]
                              , _3 = 3
                              , if_( false_ )[ _4 = 4 ]
                              , break_
                            ]
                ;
                auto prog = compile(stmt);
                prog.execute(ctx);
                assert_eq( _1, 1 );
                assert_eq( _2, 2 );
                assert_eq( _3, 3 );
                assert_eq( _4, 0 );
        }
        TEST_F( DslTest, DslIfElseTrue ){
                auto prog = compile(
                        if_( true_ )
                        [ _1 = true_ ]
                        .else_
                        [ _1 = false_ ]
                );
                prog.execute(ctx);
                assert_eq( _1, true);
        }
        TEST_F( DslTest, DslIfElseFalse ){
                auto prog = compile(
                        if_( false_ )
                        [ _1 = true_ ]
                        .else_
                        [ _1 = false_ ]
                );
                prog.execute(ctx);
                assert_eq( _1, false);
        }
        TEST_F( DslTest, DslForContinue ){

                auto prog = compile(
                        for_(
                                _1 = 0,
                                _1 != 10,
                                _1 += 1
                        )
                        [
                                continue_
                        ]
                );
                prog.execute(ctx);
                assert_eq( _1, 10);
        }
        TEST_F( DslTest, DslForContinueDeep ){

                auto prog = compile(
                        for_(
                                _1 = 0,
                                _1 != 10,
                                _1 += 1
                        )
                        [
                                if_( true_ )[
                                        continue_
                                ] ,
                                _2 = 1
                        ]
                );
                prog.execute(ctx);
                assert_eq( _1, 10);
                assert_eq( _2, 0);
        }
        TEST_F( DslTest, DslForContinueDeeper ){

                auto prog = compile(
                        for_(
                                _1 = 0,
                                _1 != 10,
                                _1 += 1
                        )
                        [
                                if_( true_ )[
                                        if_( true_ )[
                                                continue_
                                        ]
                                ] ,
                                _2 = 1
                        ]
                );
                prog.execute(ctx);
                assert_eq( _1, 10);
                assert_eq( _2, 0);
        }
        TEST_F( DslTest, DslNestedForContinue ){

                auto prog = compile(
                        for_(
                                _1 = 0,
                                _1 != 10,
                                _1 += 1
                        )
                        [
                                for_( _2 =0, _2 != 10, _2 += 1 )
                                [
                                        if_( true_ )[
                                                continue_
                                        ]
                                ]
                        ]
                );
                prog.execute(ctx);
                assert_eq( _1, 10);
                assert_eq( _2, 10);
        }
        TEST_F( DslTest, DslNestedForComplex ){

                auto prog = compile(
                        for_(
                                _1 = 0,
                                _1 != 10,
                                _1 += 1
                        )
                        [
                                for_( _2 =0, _2 != 10, _2 += 1 )
                                [
                                        _3 += 1,
                                        if_( _1 + _2 == 1 )[ // (1,0), (0,1)
                                                continue_
                                        ] ,
                                        _4 += 1
                                ]
                        ]
                );
                prog.execute(ctx);
                assert_eq( _1, 10);
                assert_eq( _2, 10);
                assert_eq( _3, 100);
                assert_eq( _4, 98);
        }
        TEST_F( DslTest, DslNestedForComplexPlace ){

                DSL_COMPILER_make_placeholder( i );
                DSL_COMPILER_make_placeholder( j );
                DSL_COMPILER_make_placeholder( count );
                DSL_COMPILER_make_placeholder( other );

                auto prog = compile(
                        stmts(
                                count = 0
                              , other = 0
                              ,
                                for_(
                                        i = 0,
                                        i != 10,
                                        i += 1
                                )
                                [
                                        for_( j =0, j != 10, j += 1 )
                                        [
                                                count += 1,
                                                if_( i + j == 1 )[ // (1,0), (0,1)
                                                        continue_
                                                ] ,
                                                other += 1
                                        ]
                                ]
                        )
                );
                prog.execute(ctx);
                assert_eq( i, 10);
                assert_eq( j, 10);
                assert_eq( count, 100);
                assert_eq( other, 98);
        }
        TEST_F( DslTest, DslNestedForContinueAnother ){
                
                DSL_COMPILER_make_placeholder( i );
                DSL_COMPILER_make_placeholder( j );
                DSL_COMPILER_make_placeholder( even );
                DSL_COMPILER_make_placeholder( count );

                auto prog = compile(
                        for_(
                                exprs(i = 0, even = 0, count = 0 ),
                                i != 10,
                                i += 1
                        )
                        [
                                _1 += 1,
                                for_( j =0, j != 10, j += 1 )
                                [
                                        count += 1,
                                        if_( ( i + j * 10 ) % 2 != 0 )
                                        [
                                                continue_
                                        ] ,
                                        even += 1
                                ] ,
                                _2 += 1
                        ]
                );
                prog.execute(ctx);
                assert_eq( count, 100);
                assert_eq( even, 50);
                assert_eq( _1, 10);
                assert_eq( _2, 10);
        }
}
