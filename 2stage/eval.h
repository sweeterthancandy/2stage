#pragma once

#include "expr.h"

namespace dsl_compiler{
        template<class Expression, class Context
              , class = void_t<decltype( detail_::make_expr_( std::declval<Expression&>() ) )>
        >
        auto eval(Expression&& expr, Context&& ctx){
                return make_expr_(expr).eval(ctx);
        }
        template<class Expression
              , class = void_t<decltype( detail_::make_expr_( std::declval<Expression&>() ) )>
        >
        std::string to_string(Expression&& expr){
                return make_expr_(expr).to_string();
        }

}

