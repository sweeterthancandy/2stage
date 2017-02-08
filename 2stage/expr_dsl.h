#pragma once

#include "expr.h"
        
#define DSL_COMPILER_make_placeholder( x )      \
        struct tag_ ## x{};                     \
        auto x = placeholder< tag_ ## x >();

namespace dsl_compiler{
        namespace detail_{

                template<class T >
                expr_impl_t
                cast_to_expr_impl_t( T const& val,void_t< decltype( expr_impl_t{ std::make_tuple( tag::literal(), std::declval<T&>()) } )>*_=nullptr ){
                        boost::ignore_unused(_);
                        return std::make_tuple( tag::_literal, val );
                }
                template<class T>
                expr_impl_t
                cast_to_expr_impl_t( T const& val,  void_t<decltype(expr_impl_t(std::declval<T const&>()))>*_=nullptr){
                        boost::ignore_unused(_);
                        return expr_impl_t{val};
                }

                struct tag_r_value{};
                struct tag_l_value{};


                
                template<class T>
                expr_impl_t make_expr_impl_(T val, void_t<decltype( funamental_t{std::declval<T&>()} )>*_=nullptr){
                        boost::ignore_unused(_);
                        return std::make_tuple( tag::_literal, val );
                }
                template<class T>
                expr_impl_t make_expr_impl_(T val, void_t<decltype( std::declval<T&>().get_expr_impl_t_() )>*_=nullptr){
                        boost::ignore_unused(_);
                        return val.get_expr_impl_t_();
                }



                template<class Tag>
                struct meta_information
                {
                        using impl_t = expression::impl_t;

                        explicit meta_information(impl_t const& impl):impl_{impl}{}
                        meta_information(const meta_information& that):impl_{that.impl_}{}
                        meta_information( meta_information&& that):impl_{std::move(that.impl_)}{}
                        
                        
                        /////////////////////////////////////////////
                        // there are only for l-values
                        /////////////////////////////////////////////
                        meta_information operator=(meta_information const& param){
                                static_assert( std::is_same<Tag, tag_l_value >::value,"can only assign to l-values");
                                return meta_information{
                                        std::make_tuple(
                                                tag::_assign
                                              , impl_
                                              , param.impl_
                                        )
                                };
                        }
                        meta_information operator=(meta_information&& param){
                                static_assert( std::is_same<Tag, tag_l_value >::value,"can only assign to l-values");
                                return meta_information{
                                        std::make_tuple(
                                                tag::_assign
                                              , impl_
                                              , std::move(param.impl_)
                                        )
                                };
                        }
                        template<
                                class Param
                              , class _tag = Tag
                              , class = void_t<
                                        std::enable_if_t<std::is_same< _tag, tag_l_value>::value>
                                      , decltype( make_expr_impl_( std::declval<Param&>() ) )
                              >
                        >
                        meta_information<tag_l_value>
                        operator=(Param const& param)const{
                                return meta_information<tag_l_value>{
                                        std::make_tuple(
                                                tag::_assign
                                              , impl_
                                              , make_expr_impl_(param)
                                        )
                                };
                        }
                private:
                        template<class Expr_Tag
                               , class L_Param
                               , class R_Param
                               , class = void_t< decltype( make_expr_impl_( std::declval< L_Param&>() ) )
                                               , decltype( make_expr_impl_( std::declval< R_Param&>() ) )
                                 >
                        >
                        friend meta_information<tag_r_value>
                        make_binary_( Expr_Tag const& expr_tag, L_Param const& l_param, R_Param const& r_param){
                                return meta_information<tag_r_value>{
                                        std::make_tuple(
                                                expr_tag
                                              , make_expr_impl_(l_param)
                                              , make_expr_impl_(r_param)
                                        )
                                };
                        }
                public:
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
                        #define EXPR_DSL_binary_op(z,n,e)                                                               \
                                template<class L_Param , class R_Param                                                  \
                                       , class = void_t< decltype( make_binary_( std::declval<tag::plus&>()             \
                                                                               , std::declval< L_Param&>()              \
                                                                               , std::declval< R_Param&>() ) ) > >      \
                                friend meta_information<tag_r_value>                                                    \
                                operator BOOST_PP_SEQ_ELEM(1,e)( L_Param const& l_param, R_Param const& r_param){                         \
                                        return make_binary_(tag::BOOST_PP_CAT(_,BOOST_PP_SEQ_ELEM(0,e)),l_param,r_param);       \
                                }
                        BOOST_PP_SEQ_FOR_EACH(EXPR_DSL_binary_op,,DSL_COMPILER_arithmetic_binary_ops DSL_COMPILER_logical_binary_ops )

                        template<
                                class Param
                              , class _tag = Tag
                              , class = void_t<
                                        std::enable_if_t<std::is_same< _tag, tag_l_value>::value>
                                      , decltype( make_expr_impl_( operator+(
                                                        std::declval<meta_information&>()
                                                      , std::declval<Param&>() ) ) )
                                >
                        >
                        meta_information<tag_l_value>
                        operator+=(Param const& param){
                                return meta_information<tag_l_value>{
                                        std::make_tuple(
                                                tag::_assign
                                              , impl_
                                              , make_expr_impl_(operator+(*this,param))
                                        )
                                };
                        }
                        
                        meta_information<tag_r_value> operator!()const{
                                return meta_information<tag_r_value>{
                                        std::make_tuple(
                                                tag::_negate
                                              , impl_
                                        )
                                };
                        }

                        decltype(auto) get_expr_impl_t_()const{return(impl_);}
                        operator expression()const{return expression{impl_};}
                private:
                        impl_t impl_;
                };

        }

        template<class Tag>
        auto placeholder(){
                return detail_::meta_information<detail_::tag_l_value>{ 
                        std::make_tuple( tag::_register_, boost::typeindex::type_id<Tag>() )
                };
        }
        inline
        auto lit(funamental_t val){
                return detail_::meta_information<detail_::tag_r_value>{ 
                        std::make_tuple( tag::_literal, val )
                };
        }

        namespace detail_{

                template<class Impl >
                expression make_expr_( Impl const& impl, void_t< decltype( make_expr_impl_(std::declval<Impl&>()) ) >*_=nullptr){
                        boost::ignore_unused(_);
                        return expression{make_expr_impl_(impl)};
                }
                inline
                expression make_expr_( expression const& e){
                        return e;
                }

        }
        template<class Param> //, class _ = void_t<decltype( make_expr_impl_(std::declval<Param&>()) ) > >
        auto not_( Param const& param){
                return detail_::meta_information<detail_::tag_r_value>{
                        std::make_tuple(
                                tag::_logical_not
                              , detail_::make_expr_impl_(param)
                        )
                };
        }
        
        template<class... Exprs
               , class = void_t< decltype( detail_::make_expr_impl_( std::declval<Exprs&>() ) )... >
        >
        auto exprs(Exprs const&... _exprs){
                std::vector< expr_impl_t> vec{
                      detail_::make_expr_impl_( _exprs )...
                };
                return detail_::meta_information<detail_::tag_r_value>{
                        std::make_tuple(
                                tag::_exprs
                              , std::move(vec)
                        )
                };
        }

        static auto true_ = lit(true);
        static auto false_ = lit(false);



}
