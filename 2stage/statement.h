#pragma once

#include "expr.h"

#include "tag_maker.h"
#include "context.h"
#include "void_t.h"

DSL_COMPILER_make_tags(
        (if_)(if_else)(while_)(for_)
        (break_)
        (continue_)
        (stmts)
        (expr)
        (push)
)

namespace dsl_compiler{

        // this is the actual statement
        using statement_impl_t = boost::make_recursive_variant<
               std::tuple< tag::if_, expression, boost::recursive_variant_>
             , std::tuple< tag::if_else, expression, boost::recursive_variant_, boost::recursive_variant_>
             , std::tuple< tag::while_, expression, boost::recursive_variant_>
             , std::tuple< tag::for_, expression, expression, expression, boost::recursive_variant_>
             , std::tuple< tag::break_>
             , std::tuple< tag::continue_>
             , std::tuple< tag::stmts, std::vector< boost::recursive_variant_> >
             , std::tuple< tag::expr, expression >
             , std::tuple< tag::push, expression >
        >::type;

        // this is the pretty dsl object, the only reason for this 
        // to to imbue the statement with my own operators
        struct statement{

                statement(std::tuple< tag::if_, expression, statement_impl_t> const& arg):impl_(arg){}
                statement(std::tuple< tag::if_else, expression, statement_impl_t, statement_impl_t> const& arg):impl_(arg){}
                statement(std::tuple< tag::while_, expression, statement_impl_t> const& arg):impl_(arg){}
                statement(std::tuple< tag::for_, expression, expression, expression, statement_impl_t> const& arg):impl_(arg){}
                statement(std::tuple< tag::break_> const& arg):impl_(arg){}
                statement(std::tuple< tag::continue_> const& arg):impl_(arg){}
                statement(std::tuple< tag::stmts, std::vector< statement_impl_t> > const& arg):impl_(arg){}
                statement(std::tuple< tag::expr, expression > const& arg):impl_(arg){}
                statement(std::tuple< tag::push, expression > const& arg):impl_(arg){}
                //statement(statement_impl_t impl):impl_(std::move(impl)){}
                statement(statement const& stmt)=default;
                statement(statement&& stmt)=default;

                statement_impl_t to_stmt_impl_t_()const{ return impl_; }
        private:
                statement_impl_t impl_;
        };

        namespace detail_{
                

                template< class Expr >
                statement_impl_t make_stmt_impl_(Expr const& expr, void_t<decltype( make_expr_( std::declval<Expr&>())) >*_=nullptr){
                        boost::ignore_unused(_);
                        return std::make_tuple(
                                tag::_expr
                              , make_expr_(expr)
                        );
                }
                template< class Stmt >
                statement_impl_t make_stmt_impl_(Stmt const& stmt, std::enable_if_t< std::is_same< std::decay_t<Stmt>, statement_impl_t>::value>*_=nullptr){
                        boost::ignore_unused(_);
                        return stmt;
                }
                template< class Stmt >
                statement_impl_t make_stmt_impl_(Stmt const& stmt, void_t<decltype( std::declval<Stmt&>().to_stmt_impl_t_())>*_=nullptr){
                        boost::ignore_unused(_);
                        auto aux{stmt.to_stmt_impl_t_()};
                        return statement_impl_t{aux};
                }

                template< class Expr_Or_Stmt >
                statement make_stmt_(Expr_Or_Stmt const& arg, void_t<decltype( make_stmt_impl_( std::declval<Expr_Or_Stmt&>())) >*_=nullptr){
                        boost::ignore_unused(_);
                        return statement{ make_stmt_impl_(arg) };
                }
        }


        template<
                class Cond
              , class Stmt
              , class = void_t<
                        decltype( detail_::make_expr_( std::declval<Cond&>() ) )
                      , decltype( detail_::make_stmt_impl_( std::declval<Stmt&>() ) )
                >
        >
        statement if_(Cond const& cond, Stmt const& stmt){
                return statement{std::make_tuple(
                        tag::_if_
                      , detail_::make_expr_( cond )
                      , detail_::make_stmt_impl_( stmt )
                )};
        }
        template<
                class Cond
              , class TrueStmt
              , class FalseStmt
              , class = void_t<
                        decltype( detail_::make_expr_( std::declval<Cond&>() ) )
                      , decltype( detail_::make_stmt_impl_( std::declval<TrueStmt&>() ) )
                      , decltype( detail_::make_stmt_impl_( std::declval<FalseStmt&>() ) )
                >
        >
        statement if_else(Cond const& cond, TrueStmt const& true_stmt, FalseStmt const& false_stmt){
                return std::make_tuple(
                        tag::_if_else
                      , detail_::make_expr_( cond )
                      , detail_::make_stmt_impl_( true_stmt )
                      , detail_::make_stmt_impl_( false_stmt )
                );
        }
        
        template<
                class Cond
              , class Stmt
              , class = void_t<
                        decltype( detail_::make_expr_( std::declval<Cond&>() ) )
                      , decltype( detail_::make_stmt_impl_( std::declval<Stmt&>() ) )
                >
        >
        statement while_(Cond const& cond, Stmt const& stmt){
                return std::make_tuple(
                        tag::_while_
                      , detail_::make_expr_( cond )
                      , detail_::make_stmt_impl_( stmt )
                );
        }

        template<
                class Expr
              , class = void_t< decltype( detail_::make_expr_( std::declval<Expr&>() ) ) >
        >
        statement expr( Expr const& _expr){
                return std::make_tuple(
                        tag::_expr
                      , detail_::make_expr_( _expr )
                );
        }

        template<class... Stmts
               , class = void_t< decltype( detail_::make_stmt_impl_( std::declval<Stmts&>() ) )... >
        >
        statement stmts(Stmts const&... _stmts){
                std::vector< statement_impl_t> vec{
                      detail_::make_stmt_impl_( _stmts )...
                };
                return statement{std::make_tuple(
                        tag::_stmts
                      , std::move(vec)
                )};
        }

        // ok I'm allowing 
        //              for_(<init>,<cond>,<inc>,<stmt>),
        // and
        //              for_(<init>,<cond>,<inc>)[<stmt>]
        // for now,
        //
        // XXX remove this one
        template<
                class Init
              , class Cond
              , class Inc
              , class Stmt
              , class = void_t<
                        decltype( detail_::make_expr_( std::declval<Init&>() ) )
                      , decltype( detail_::make_expr_( std::declval<Cond&>() ) )
                      , decltype( detail_::make_expr_( std::declval<Inc&>() ) )
                      , decltype( detail_::make_stmt_impl_( std::declval<Stmt&>() ) )
                >
        >
        statement for_(Init const& init
                             , Cond const& cond
                             , Inc const& inc
                             , Stmt const& stmt)
        {
                return statement{std::make_tuple(
                        tag::_for_
                      , detail_::make_expr_( init )
                      , detail_::make_expr_( cond )
                      , detail_::make_expr_( inc )
                      , detail_::make_stmt_impl_( stmt )
                )};
        }

        static statement break_{ std::make_tuple(tag::_break_) };
        static statement continue_{ std::make_tuple(tag::_continue_) };

        template<
                class Expr
              , class = void_t< decltype( detail_::make_expr_( std::declval<Expr&>() ) ) >
        >
        statement push( Expr const& expr){
                return statement{std::make_tuple(
                        tag::_push
                      , detail_::make_expr_( expr )
                )};
        }




        namespace detail_{

                /*
                   Consider the following dsl,
                  
                        if_(<expr0>)
                        [
                                <stmt0>
                        ],
                        if_(<expr1>)
                        [
                                <stmt1>
                        ]
                  
                   which creates the tree
                  
                                          Stmts
                                      /           \
                                 /                    \
                             if                      if
                          /      \              /           \
                       expr0    stmt0        expr1          stmt1
                  
                   But sytactically it's 
                  
                  
                   for this, need if_ to create a funct
                   
                        if_(<expr0>) [ <stmt0> ], if_(<expr1>) [ <stmt1> ]
                        |                      |  |                      |
                        ------------------------  ------------------------
                            statement                  statement
                  
                   I can get this in C++ by making 

                        struct if_{
                                ...
                                <proxy> operator()(...);
                        };
                        struct <proxy>{
                                ...
                                statement [](statement_t const&)const;
                        };

                        etc, so the statement

                                if_( _1 =0, _1 != 10 , ++_1)
                                [
                                        push(_1)
                                ]

                        has the type statement
                  
                  
                 */
                template<class F>
                struct expr_proxy_{
                        explicit expr_proxy_(F f):f_(f)
                        {}
                        template<class... Stmts
                              ,  class = void_t<decltype( stmts( std::declval<Stmts>()...) ) >
                        >
                        statement operator()(Stmts const&... _stmts)const{
                                return f_( stmts( _stmts...) );
                        }
                        template<class Stmt ,  class = void_t<decltype( stmts( std::declval<Stmt>()) ) > >
                        statement operator[](Stmt const& _stmt)const{
                                return this->operator()(_stmt);
                        }
                private:
                        F f_;
                };

                template<class F>
                auto make_expr_proxy_(F f){
                        return expr_proxy_<F>(f);
                }

                /*
                   Consider
                           if_(_1 = 0)
                           [
                                push(_1)
                           ]
                   or
                           if_(_1 = 0, _1 != 10, ++_1)
                           [
                                push(_1)
                           ]
                           .else_        <--- if_else_else_proxy::else_
                           [
                                _1 -= 1
                           ]

                  both of the above need to be a statement,
                  
                 */

                struct if_else_else_obj{
                        explicit if_else_else_obj(expression const& cond, statement const& stmt):cond_{cond},stmt_{stmt}{}
                        template<class... Stmts
                              ,  class = void_t<decltype( stmts( std::declval<Stmts>()...) ) >
                        >
                        auto operator()(Stmts const&... _stmts)const{
                                return if_else( cond_, stmt_, stmts( _stmts...) );
                        }
                        template<class Stmt, class = void_t<decltype( make_stmt_impl_( std::declval<Stmt&>() ) )> >
                        auto operator[](Stmt const& _stmt)const{
                                return if_else( cond_, stmt_,  detail_::make_stmt_impl_(_stmt));
                        }
                private:
                        expression cond_;
                        statement stmt_; 
                };


                struct if_else_else_proxy{
                        explicit if_else_else_proxy(expression const& cond, statement const& stmt):cond_{cond},stmt_{stmt},else_{cond,stmt_}{}

                        statement_impl_t to_stmt_impl_t_()const{
                                return make_stmt_impl_(if_( cond_, stmt_));
                        }

                private:
                        expression cond_;
                        statement stmt_; 
                public:
                        // This is the sweet dsl sugar
                        if_else_else_obj else_;
                };

                struct if_else_if_proxy{
                        explicit if_else_if_proxy(expression const& cond):cond_{cond}{}
                        template<class... Stmts
                              ,  class = void_t<decltype( stmts( std::declval<Stmts>()...) ) >
                        >
                        if_else_else_proxy operator()(Stmts const&... _stmts)const{
                                return if_else_else_proxy( cond_, stmts( _stmts...) );
                        }
                        template<class Stmt, class = void_t<decltype( stmts( std::declval<Stmt&>() ) )> >
                        if_else_else_proxy operator[](Stmt const& _stmt)const{
                                return if_else_else_proxy( cond_, stmts(_stmt));
                        }
                private:
                        expression cond_;
                };
        }
        template<
                class Init
              , class Cond
              , class Inc
              , class = void_t<
                        decltype( for_( std::declval<Init&>(), 
                                        std::declval<Cond&>(),
                                        std::declval<Init&>(),
                                        std::declval<statement&>()) )>
        >
        auto for_(Init const& init
                     , Cond const& cond
                     , Inc const& inc)
        {
                return detail_::make_expr_proxy_( [init,cond,inc](auto const& stmt){ return for_(init,cond,inc,stmt); });
        }

        template<
                class Cond
              , class = void_t< decltype( detail_::make_expr_( std::declval<Cond&>() ) ) >
        >
        auto if_( Cond const& cond)
        {
		return detail_::if_else_if_proxy( detail_::make_expr_( cond ) );
                //return detail_::make_expr_proxy_( [cond](auto const& stmt){ return if_(cond, stmt); });
        }
        template<
                class Cond
              , class = void_t< decltype( while_( std::declval<Cond&>(), std::declval<statement&>() ) ) >
        >
        auto while_( Cond const& cond)
        {
                return detail_::make_expr_proxy_( [cond](auto const& stmt){ return while_(cond, stmt); });
        }
        
        template<class Left, class Right
               , class = void_t<
                        decltype( stmts( std::declval<Left&>(), std::declval<Right&>() ) )
                >
        >
        statement operator,(Left const& left, Right const& right){
                return stmts(left,right);
        }

}
