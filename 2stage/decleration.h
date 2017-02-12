#pragma once

#include "statement.h"

namespace dsl_compiler{


        template<class Return_Type>
        struct function{
                explicit function(statement stmt):stmt_{std::move(stmt)}{}

                template<class... Args>
                Return_Type operator()(Args&&... args){
                        auto tmp{ stmt_(std::forward<Args>(args)...)};
                        return static_cast<Return_Type>(tmp);
                }
                statement_impl_t to_stmt_impl_t_()const{ return stmt_.to_stmt_impl_t_(); }
        private:
                statement stmt_;
        };


}
