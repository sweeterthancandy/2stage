#pragma once

#include <map>
#include <vector>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <typeinfo>
#include <typeindex>

#include <boost/range/algorithm.hpp>

#include <boost/type_index.hpp>

#include "funamental.h"

namespace dsl_compiler{
        struct context{
                context():return_(0){}
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

                int get_return()const{ return return_; }
        private:
                std::map<boost::typeindex::type_index,funamental_t> m_;
                std::vector<funamental_t> stack_;
                funamental_t return_;
        };
}
