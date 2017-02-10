#pragma once

#include "statement.h"
#include "expr.h"
#include "backend.h"

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "compiler_context.h"

namespace dsl_compiler{

        namespace detail_{

                struct compiler_ : boost::static_visitor<void>
                {
                        /*
                         * for(init,cond,inc,stmt) ->
                         *
                         *      init
                         *      _1
                         *      if cond goto _2
                         *      stmt
                         *      inc
                         *      goto _1
                         *      _2
                         *
                         * if( cond , stmt ) ->
                         *      if ! cond goto _1
                         *      stmt
                         *      _1
                         *
                         * while( cond, stmt ) ->
                         *      _1
                         *      if ! cond goto _2
                         *      stmt
                         *      goto _1
                         *      _2
                         *
                         * break_ -> context sensitive
                         * continue_ -> context sensitive
                         *
                         *
                         *
                         */
                        template<class... Args>
                        void operator()(std::tuple<tag::if_, Args...> const& arg){
                                ctx_.begin_placeholder_context();
                                ctx_.emit_goto_if( not_( std::get<1>(arg)) ,0 );
                                boost::apply_visitor( *this, std::get<2>(arg) );
                                ctx_.placeholder(0);
                                ctx_.end_placeholder_context();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::if_else, Args...> const& arg){
                                ctx_.begin_placeholder_context();
                                // if( expr )
                                ctx_.emit_goto_if( not_(std::get<1>(arg)) ,0 );
                                //      true expr
                                boost::apply_visitor( *this, std::get<2>(arg) );
                                ctx_.emit_goto(1);
                                // else 
                                //      false expr
                                ctx_.placeholder(0);
                                boost::apply_visitor( *this, std::get<3>(arg) );
                                ctx_.placeholder(1);
                                ctx_.end_placeholder_context();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::while_, Args...> const& arg){
                                ctx_.begin_breakable_placeholder_context();
                                ctx_.placeholder(0);
                                ctx_.emit_goto_if( not_( std::get<1>(arg)) ,1 );
                                boost::apply_visitor( *this, std::get<2>(arg) );
                                ctx_.continue_placeholder();
                                ctx_.emit_goto( 0 );
                                ctx_.placeholder(1);
                                ctx_.break_placeholder();
                                ctx_.end_placeholder_context();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::for_, Args...> const& arg){
                                ctx_.begin_breakable_placeholder_context();
                                ctx_.emit_expr( std::get<1>(arg) );
                                ctx_.placeholder(0);
                                ctx_.emit_goto_if( not_( std::get<2>(arg)) ,1 );
                                boost::apply_visitor( *this, std::get<4>(arg) );
                                ctx_.continue_placeholder();
                                ctx_.emit_expr( std::get<3>(arg) );
                                ctx_.emit_goto( 0 );
                                ctx_.placeholder(1);
                                ctx_.break_placeholder();
                                ctx_.end_placeholder_context();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::break_, Args...> const& ){
                                ctx_.emit_break();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::continue_, Args...> const& ){
                                ctx_.emit_continue();
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::stmts, Args...> const& arg){
                                boost::for_each( std::get<1>(arg), boost::apply_visitor( *this ) );
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::expr, Args...> const& arg){
                                ctx_.emit_expr( std::get<1>(arg) );
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::push, Args...> const& arg){
                                ctx_.emit_push( std::get<1>(arg) );
                        }
                        template<class... Args>
                        void operator()(std::tuple<tag::return_, Args...> const& arg){
                                ctx_.emit_return( std::get<1>(arg) );
                        }
                        void debug()const{
                                ctx_.debug();
                        }
                        template<class Stmt, class = void_t<decltype( make_stmt_impl_( std::declval<Stmt&>() ) )> >
                        auto compile(Stmt const& stmt){
                                auto si{make_stmt_impl_(stmt)};
                                ctx_.begin_function_context();
                                boost::apply_visitor( *this , si);
                                ctx_.return_placeholder();
                                ctx_.end_placeholder_context();
                                return ctx_.compile();
                        }
                private:
                        compiler_contex ctx_;
                };
        }

        //inline
        //program compile( statement::impl_t const& stmt ){
                //detail_::compiler_ ctx;
                //boost::apply_visitor( ctx , stmt );
                //return ctx.compile();
        //}
        template<class Stmt, class = void_t< decltype( detail_::make_stmt_impl_(std::declval<Stmt&>()) ) > >
        program compile( Stmt const& stmt){
                auto _stmt = detail_::make_stmt_impl_(stmt);
                detail_::compiler_ ctx;
                auto tmp{ctx.compile(_stmt)};
                return tmp;
        }
        template<class Stmt, class = void_t< decltype( detail_::make_stmt_impl_(std::declval<Stmt&>()) ) > >
        program debug_compile( Stmt const& stmt ){
                auto _stmt = detail_::make_stmt_impl_(stmt);
                detail_::compiler_ ctx;
                auto tmp{ctx.compile(_stmt)};
                //boost::apply_visitor( ctx , _stmt );
                ctx.debug();
                return tmp;
        }
        
        template<class... Integer>
        funamental_t statement::operator()(Integer... values){
                auto prog = debug_compile(impl_);
                context ctx;
                prog.execute(ctx);
                return ctx.get_return();
        }
}
