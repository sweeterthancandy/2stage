#pragma once

#include "tag_maker.h"

#include <boost/type_index.hpp>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/contains.hpp>

#include <boost/type_index.hpp>

#include <boost/variant.hpp>
#include <boost/core/ignore_unused.hpp>

#include <utility>
#include <sstream>

#include "context.h"
#include "void_t.h"

DSL_COMPILER_make_tags(
        // terminals
        (literal)(register_)
        // unary operators
        (unary_plus)(negate)
        // binary operators
        (multiplies)(divides)(plus)(minus)(modulus)
        // logical operators
        (less)(greater)(less_equal)(greater_equal)(equal_to)(not_equal_to)

        // logical unary operator
        (logical_not)
        // assignment
        (assign)
        // 
        (exprs)
        // not an expression
        (nos)
)




namespace dsl_compiler{
        #define DSL_COMPILER_arithmetic_binary_ops \
              (( multiplies)(*))\
              (( divides)(/))\
              (( plus)(+))\
              (( minus)(-))\
              (( modulus)(%))

        #define DSL_COMPILER_logical_binary_ops \
              (( less)(<))\
              (( less_equal)(<=))\
              (( greater)(>))\
              (( greater_equal)(>=))\
              (( equal_to)(==))\
              (( not_equal_to)(!=))

        namespace detail_{
                struct get_type_index : boost::static_visitor<boost::typeindex::type_index>
                {
                        template<class T>
                        [[noreturn]]
                        result_type operator()(T&&)const{
                                BOOST_THROW_EXCEPTION(std::domain_error("not a l-value"));
                        }
                        result_type operator()(std::tuple<tag::register_,boost::typeindex::type_index> const& arg)const{
                                return std::get<1>(arg);
                        }

                };

                template<class Placeholder>
                auto cast_to_type_index(Placeholder const& p){
                        return boost::apply_visitor( get_type_index(), p );
                }
        }

                
        struct expression{

                using impl_t = boost::make_recursive_variant<
                        std::tuple< tag::literal, funamental_t>
                      , std::tuple< tag::register_, boost::typeindex::type_index>

                      , std::tuple< tag::unary_plus, boost::recursive_variant_ >
                      , std::tuple< tag::negate, boost::recursive_variant_ >
                      , std::tuple< tag::logical_not, boost::recursive_variant_ >
        
                        #define DSL_COMPILER_def_biop_tuple(z,u,e) \
                                , std::tuple< tag:: BOOST_PP_SEQ_ELEM(0,e) , boost::recursive_variant_ , boost::recursive_variant_ >
                      
                        BOOST_PP_SEQ_FOR_EACH(
                                 
                                DSL_COMPILER_def_biop_tuple
                              ,
                              , DSL_COMPILER_arithmetic_binary_ops 
                                DSL_COMPILER_logical_binary_ops 
                        )

                        #undef DSL_COMPILER_def_biop_tuple

                      , std::tuple< tag::assign, boost::recursive_variant_ , boost::recursive_variant_ >

                      , std::tuple< tag::exprs, std::vector< boost::recursive_variant_> >

                      , std::tuple< tag::nos >
                >::type;
                
        public:

                enum nos_t{ nos };

                explicit expression(nos_t):impl_{std::make_tuple(tag::_nos)}{}

                explicit expression(std::tuple<tag::literal, funamental_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::register_, boost::typeindex::type_index> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::unary_plus, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::negate, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::logical_not, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::multiplies, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::divides, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::plus, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::minus, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::less, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::less_equal, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::greater, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::greater_equal, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::equal_to, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::not_equal_to, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::assign, impl_t, impl_t> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::exprs, std::vector<impl_t>> const &arg) : impl_{arg} {}
                explicit expression(std::tuple<tag::nos> const &arg) : impl_{arg} {}
                explicit expression( impl_t const& impl) : impl_{impl} {}

                expression( expression const& that):impl_{that.impl_}{}

                template<class T, class _ = void_t< decltype( std::declval<T&>().operator expression()) > >
                expression( T const& val):impl_{val.get()}{}

        private:
                struct to_string_ : boost::static_visitor<std::string>{
                        template<class Tag, class... Args>
                        result_type operator()(std::tuple<Tag, Args...> const& args)const{
                                std::stringstream sstr;
                                sstr << Tag() << "(";
                                fold_args_(sstr,args,std::make_index_sequence<sizeof...(Args)>{});
                                sstr << ")";
                                return sstr.str();
                        }
                private:
                        template<class Tuple, size_t... Idx>
                        void fold_args_(std::stringstream& sstr, Tuple const& t, std::index_sequence<Idx...>)const{
                                using lro = int[];
                                (void)lro{0,( void( sstr << (Idx == 0 ? "": ",")), rec_(sstr,std::get<Idx + 1>(t)), 0 )...};
                        }
                        template<class T>
                        std::enable_if_t<
                                 std::is_same<impl_t, std::decay_t<T> >::value
                        > rec_(std::stringstream& sstr, T val)const{
                                sstr << boost::apply_visitor( *this, val);
                        }
                        template<class T>
                        std::enable_if_t<
                               ! std::is_same<impl_t, std::decay_t<T> >::value
                        > rec_(std::stringstream& sstr, T val)const{
                                sstr << val;
                        }
                        void rec_(std::stringstream& sstr, std::vector<impl_t> const& exprs)const{
                                for(size_t idx{0},sz{exprs.size()};idx!=sz;++idx){
                                        sstr << ( idx == 0 ? "" : ", " ) << boost::apply_visitor(*this,exprs[idx]);
                                }
                        }
                };

                struct eval_ : boost::static_visitor<funamental_t>
                {
                        explicit eval_(context& ctx):ctx_(ctx){}
                        result_type operator()(std::tuple< tag::literal, funamental_t> const& arg){
                                return std::get<1>(arg);
                        }
                        result_type operator()(std::tuple< tag::register_, boost::typeindex::type_index> const& arg){
                                return ctx_.get(std::get<1>(arg));
                        }
                        result_type operator()(std::tuple< tag::unary_plus, impl_t > const& arg){
                                return + boost::apply_visitor(*this,std::get<1>(arg));
                        }
                        result_type operator()(std::tuple< tag::negate, impl_t > const& arg){
                                return - boost::apply_visitor(*this,std::get<1>(arg));
                        }
                        result_type operator()(std::tuple< tag::logical_not, impl_t > const& arg){
                                return ! boost::apply_visitor(*this,std::get<1>(arg));
                        }
                        #define DSL_COMPILER_def_visitor_operator( z, n, e )                           \
                                result_type operator()( std::tuple<tag::BOOST_PP_SEQ_ELEM( 0, e ),     \
                                                                   impl_t, impl_t> const& arg ) {      \
                                        return boost::apply_visitor( *this, std::get<1>( arg ) )       \
                                            BOOST_PP_SEQ_ELEM( 1, e )                                  \
                                                boost::apply_visitor( *this, std::get<2>( arg ) );     \
                                }

                        BOOST_PP_SEQ_FOR_EACH(
                                DSL_COMPILER_def_visitor_operator
                              ,
                              , DSL_COMPILER_arithmetic_binary_ops
                                DSL_COMPILER_logical_binary_ops
                        )

                        #undef DSL_COMPILER_def_visitor_operator

                        result_type operator()(std::tuple< tag::assign, impl_t , impl_t > const& arg){
                                boost::typeindex::type_index id = detail_::cast_to_type_index(std::get<1>(arg));
                                funamental_t lp{boost::apply_visitor(*this,std::get<2>(arg))};
                                ctx_.assign(id,lp);
                                return ctx_.get(id);
                        }
                        result_type operator()(std::tuple< tag::exprs, std::vector< impl_t> > const& arg){
                                auto const& vec = std::get<1>(arg);
                                assert( vec.size() && "precondition failed (can't be empty)");
                                for(size_t i{0};i!=vec.size()-1;++i)
                                        boost::apply_visitor(*this,vec[i]);
                                return boost::apply_visitor(*this,vec.back());
                        }
                        // keep this for now
                        result_type operator()(std::tuple< tag::nos > const&){
                                return funamental_t{0}; 
                        }
                private:
                        context& ctx_;
                };

        public:
                funamental_t eval( context& ctx)const{
                        eval_ aux(ctx);
                        return boost::apply_visitor( aux, impl_ );
                }
                std::string to_string()const{
                        return boost::apply_visitor( to_string_(), impl_ );
                }
                friend std::ostream& operator<<(std::ostream& ostr, expression const& self){
                        return ostr << self.to_string();
                }
                decltype(auto) get_expr_impl_t_()const{return(impl_);}
        private:
                impl_t impl_;
        };

        using expr_impl_t = expression::impl_t;

        

}
