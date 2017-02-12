#pragma once

#include "statement.h"

namespace dsl_compiler{

        template<class Return_Type>
        struct function{
                explicit function(statement stmt):stmt_{std::move(stmt)}{}
                template<class... Args, class = void_t< decltype(funamental_t{std::declval<Args>()})...> >
                Return_Type operator()(Args&&... args){
                        auto tmp{ stmt_(std::forward<Args>(args)...)};
                        return static_cast<Return_Type>(tmp);
                }
        private:
                statement stmt_;
        };


}
