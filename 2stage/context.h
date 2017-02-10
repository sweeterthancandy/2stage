#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <typeinfo>
#include <typeindex>
#include <utility>

#include <boost/range/algorithm.hpp>

#include <boost/type_index.hpp>

#include "funamental.h"

namespace dsl_compiler{

        template<size_t N>
        struct tag_dsl_compiler_arg{};


        struct context{

        private:
                template<size_t Idx>
                void push_args_(){}
                template<size_t Idx, class Arg, class... Left>
                void push_args_(Arg&& arg, Left&&... left){
                        assign( boost::typeindex::type_id< tag_dsl_compiler_arg<Idx> >(), std::forward<Arg>(arg));
                        push_args_<Idx+1>(std::forward<Left>(left)...);
                }
        public:
                template<class... Args>
                context(Args&&... args)
                        :return_(0)
                {
                        push_args_<1>(std::forward<Args>(args)...);
                }
                decltype(auto) get(boost::typeindex::type_index const& id){
                        if( m_.count( id ) == 0 ){
                                BOOST_THROW_EXCEPTION(std::domain_error("register doesn't exist!"));
                        }
                        return (m_.find(id)->second);
                }
                void assign( boost::typeindex::type_index const& id, funamental_t const& val){
                        if( m_.count( id ) == 0 ){
                                m_.insert(std::make_pair(id,val));
                        } else {
                                m_.find(id)->second = val;
                        }
                }
                void push(funamental_t const& val){
                        stack_.emplace_back( val );
                }
                void set_return(funamental_t const& val){
                        return_ = val;
                }
                int get_return()const{ return return_; }
                void debug()const{
                        std::cout << "{";
                        boost::for_each( m_, [&](auto&& p){ 
                                std::cout << "(" << p.first.pretty_name() << " => " << p.second  << "), ";
                        });
                        std::cout << "}";

                        std::cout << "{";
                        boost::for_each( stack_, [&](auto&& p){ 
                                std::cout << p << ", ";
                        });
                        std::cout << "}";
                        std::cout << "\n";
                }

                decltype(auto) get_stack()const{return(stack_);}

                void push_scope(){}
                void pop_scope(){}

        private:
                std::map<boost::typeindex::type_index,funamental_t> m_;
                std::vector<funamental_t> stack_;
                std::vector<funamental_t> args_;
                funamental_t return_;
        };
}
