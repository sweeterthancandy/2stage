#pragma once

#include "void_t.h"

#include <type_traits>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/contains.hpp>

#include <boost/variant.hpp>

#include <boost/core/ignore_unused.hpp>
#include <boost/preprocessor.hpp>

namespace dsl_compiler{


        /*
                funamental_t is the atomic datatype, nothing is smaller than an funamental_t



        */

        namespace mpl = boost::mpl;

        namespace detail_{

                template<class , class ,class = void_t<>>
                struct can_modulus : std::false_type{};

                template<class Left, class Right>
                struct can_modulus<Left,Right
                                  ,void_t<decltype( std::declval<Left&>() % std::declval<Right&>() ) >>
                        : std::true_type
                {};

                static_assert( can_modulus<int,int>::value,"");
                static_assert( can_modulus<int,char>::value,"");
                static_assert( can_modulus<int,char>::value,"");
                static_assert( can_modulus<int&,const int>::value,"");

                
                using all_funamental = mpl::vector<
                        bool
                      , char
                      , unsigned char
                      , short int
                      , unsigned short int
                      , int
                      , unsigned int
                      , long int
                      , unsigned long int
                      , long long int
                      , unsigned long long int
                      , float
                      , double
                      , long double
                >;
                using funamental_t_var_t = boost::make_variant_over< all_funamental>::type;
                
                struct do_modulus : boost::static_visitor< funamental_t_var_t >
                {
                        template<class Left, class Right >
                        result_type operator()(Left const& left, Right const& right
                                      , std::enable_if_t< can_modulus<Left,Right>::value>*_=nullptr)const
                        {
                                boost::ignore_unused(_);
                                return left % right;
                        }
                        template<class Left, class Right >
                        [[noreturn]] result_type operator()(Left const& , Right const& 
                                      , std::enable_if_t<!can_modulus<Left,Right>::value>*_=nullptr)const
                        {
                                boost::ignore_unused(_);
                                BOOST_THROW_EXCEPTION(std::domain_error("can't modulus there types"));
                        }
                };
        }

        struct funamental_t
        {
                using var_t = detail_::funamental_t_var_t;
                using all_funamental = detail_::all_funamental;

                template<
                        class T
                      , class _ = std::enable_if_t<
                                mpl::contains<all_funamental, std::decay_t<T> >::value
                        >
                >
                funamental_t( T val ):var_(val){}

        private:
                explicit funamental_t( var_t var):var_{var}{}
        public:
                #define FUNDAMENTAL_make_binary_assign_op( z, u, op )                          \
                        funamental_t& operator BOOST_PP_CAT( op,                               \
                                                             = )( funamental_t const& that ) { \
                                var_ = boost::apply_visitor( []( auto lp, auto rp ) -> var_t { \
                                        return lp op rp;                                       \
                                }, var_, that.var_ );                                          \
                                return *this;                                                  \
                        }
                
                #define FUNDAMENTAL_make_binary_op( z, u, op )                                 \
                        funamental_t operator op( funamental_t const& that ) {                 \
                                return funamental_t{boost::apply_visitor(                      \
                                    []( auto lp, auto rp ) -> var_t { return lp op rp; },      \
                                    var_, that.var_ )};                                        \
                        }
                
                #define FUNDAMENTAL_make_arithmetic_op( z, u, op )                             \
                        FUNDAMENTAL_make_binary_assign_op( z, u, op )                          \
                            FUNDAMENTAL_make_binary_op( z, u, op )
                
                #define FUNDAMENTAL_make_logical_op( z, u, op )                                \
                        FUNDAMENTAL_make_binary_op( z, u, op )

                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wsign-compare"

                BOOST_PP_SEQ_FOR_EACH( FUNDAMENTAL_make_arithmetic_op,,(+)(-)(*)(/))
                BOOST_PP_SEQ_FOR_EACH( FUNDAMENTAL_make_logical_op,,(<)(>)(<=)(>=)(==)(!=))


                
                #pragma GCC diagnostic pop

                #undef FUNDAMENTAL_make_logical_op
                #undef FUNDAMENTAL_make_arithmetic_op
                #undef FUNDAMENTAL_make_binary_op 
                #undef FUNDAMENTAL_make_binary_assign_op

                funamental_t operator%(funamental_t const& that){
                        return funamental_t{ boost::apply_visitor( detail_::do_modulus(), var_, that.var_ ) };
                }

                friend funamental_t operator+(funamental_t const& self){
                        return self;
                }
                friend funamental_t operator-(funamental_t const& self){
                        return funamental_t{boost::apply_visitor( [](auto _)->var_t{ return -_;}, self.var_ )};
                }
                friend funamental_t operator!(funamental_t const& self){
                        return funamental_t{boost::apply_visitor( [](auto _)->var_t{ return !_;}, self.var_ )};
                }

#if 0
                funamental_t& operator++(){
                        var_ = boost::apply_visitor( [](auto _)->var_t{ return ++_; }, var_);
                        return *this;
                }
                funamental_t operator++(int){
                        funamental_t tmp{*this};
                        this->operator++();
                        return tmp;
                }
                funamental_t& operator--(){
                        var_ = boost::apply_visitor( [](auto _)->var_t{ return --_; }, var_);
                        return *this;
                }
                funamental_t operator--(int){
                        funamental_t tmp{*this};
                        this->operator--();
                        return tmp;
                }
#endif



                template<class T>
                operator T()const{
                        return boost::apply_visitor( [](auto _)->T{ return static_cast<T>(_); }, var_ );
                }
                
                friend std::ostream& operator<<(std::ostream& ostr, funamental_t self){
                        boost::apply_visitor( [&](auto _){ ostr << _; }, self.var_ );
                        return ostr;
                }
        private:
                var_t var_;
        };

        namespace detail_{
                template<class T>
                struct is_a_fundamental :
                        mpl::contains<
                                funamental_t::all_funamental
                              , std::decay_t<T>
                        >
                {};
        }

}
