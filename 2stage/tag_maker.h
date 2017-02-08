#pragma once

#include <boost/preprocessor.hpp>

#define DSL_COMPILER_make_tag( z, d, name )                                    \
        struct name {                                                          \
                friend std::ostream& operator<<( std::ostream& ostr, name ) {  \
                        return ostr << BOOST_PP_STRINGIZE( name );             \
                }                                                              \
        };                                                                     \
        static name BOOST_PP_CAT( _, name );

// must be called from global scope
#define DSL_COMPILER_make_tags( seq )                                          \
        namespace dsl_compiler {                                               \
                namespace tag {                                                \
                        BOOST_PP_SEQ_FOR_EACH( DSL_COMPILER_make_tag, , seq )  \
                }                                                              \
        }

