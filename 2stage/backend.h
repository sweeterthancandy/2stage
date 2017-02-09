#pragma once

#include <tuple>
#include <string>
#include <vector>

#include <boost/format.hpp>

#include "statement.h"
#include "tag_maker.h"


DSL_COMPILER_make_tags(
        (goto_)
        (goto_if)
)

namespace dsl_compiler{

        struct backend_statement{

                using impl_t = boost::variant<
                       std::tuple< tag::goto_if, expression, size_t>
                     , std::tuple< tag::goto_, size_t >
                     , std::tuple< tag::expr, expression >
                     , std::tuple< tag::push, expression >
                     , std::tuple< tag::return_, size_t, expression >
                >;

                struct to_string_ : boost::static_visitor<std::string>
                {
                       result_type operator()(std::tuple< tag::goto_if, expression, size_t> const& arg)const{
                               return str(boost::format("goto_if(%s,%s)") % std::get<1>(arg) % std::get<2>(arg));
                       }
                       result_type operator()(std::tuple< tag::goto_, size_t > const& arg)const{
                               return str(boost::format("goto_(%s)") % std::get<1>(arg));
                       }
                       result_type operator()(std::tuple< tag::expr, expression > const& arg)const{
                               return str(boost::format("expr(%s)") % std::get<1>(arg) );
                       }
                       result_type operator()(std::tuple< tag::push, expression > const& arg)const{
                               return str(boost::format("push(%s)") % std::get<1>(arg) );
                       }
                       result_type operator()(std::tuple< tag::return_, size_t, expression > const& arg)const{
                               return str(boost::format("return_(%s,%s)") % std::get<1>(arg) % std::get<2>(arg));
                       }
                };

                std::string to_string()const{ return boost::apply_visitor( to_string_(), impl_ ); }
        private:
                impl_t impl_;
        };

        struct execution_context{
                void set(size_t offset){ offset_ = offset; }
                void next(){ ++offset_; }
                auto get()const{ return offset_; }
                void debug()const{ std::cout << "offset_ = " << offset_ << "\n"; }
        private:
                size_t offset_{0};
        };

        struct runner_ : boost::static_visitor<>
        {
               explicit runner_(
                       context& ctx
                     , execution_context& exe_ctx)
                       : ctx_(ctx), exe_ctx_(exe_ctx)
               {}

               void operator()(std::tuple< tag::goto_if, expression, size_t> const& arg)const{
                       if( static_cast<bool>(std::get<1>(arg).eval(ctx_))){
                               exe_ctx_.set( std::get<2>(arg) );
                       } else{
                               exe_ctx_.next();
                       }
               }
               void operator()(std::tuple< tag::goto_, size_t > const& arg)const{
                       exe_ctx_.set( std::get<1>(arg) );
               }
               void operator()(std::tuple< tag::expr, expression > const& arg)const{
                       std::get<1>(arg).eval(ctx_);
                       exe_ctx_.next();
               }
               void operator()(std::tuple< tag::push, expression > const& arg)const{
                       auto result = std::get<1>(arg).eval(ctx_);
                       ctx_.push( std::move( result ) );
                       exe_ctx_.next();
               }
               void operator()(std::tuple<tag::return_, size_t, expression> const& arg)const{
                       auto result = std::get<2>(arg).eval(ctx_);
                       ctx_.set_return(result);
                       exe_ctx_.set( std::get<1>(arg) );
               }
        private:
               context& ctx_;
               execution_context& exe_ctx_;
        };



        struct program{
                explicit program( std::vector<backend_statement::impl_t> const& prog):prog_(prog){}

                void execute( context& ctx){
                        execution_context exe_ctx;
                        runner_ runner(ctx,exe_ctx);

                        for(;;){
                                assert( exe_ctx.get() <= prog_.size() && "preconditon failed");
                                if( exe_ctx.get() == prog_.size() )
                                        break;
                                boost::apply_visitor( runner, prog_[ exe_ctx.get()]);
                        }
                }

                void debug(){
                        std::cout << "----------------\n";
                        for(size_t idx{0};idx != prog_.size(); ++idx){
                                std::cout
                                        << boost::format("%2i  ") % idx 
                                        << boost::apply_visitor( backend_statement::to_string_(), prog_[idx] )
                                        << "\n";
                        }
                        std::cout << "----------------\n";
                }
        private:
                std::vector< backend_statement::impl_t > prog_;
        };



}
