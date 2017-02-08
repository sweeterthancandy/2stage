#pragma once

namespace dsl_compiler{

        namespace detail_{
                struct placeholder_context{
                private:
                        enum class ctx_type{
                                regular
                              , breakable
                        };
                        using map_t = std::map<size_t,size_t>;
                        using internal_ctx_t = std::tuple<ctx_type, map_t  >;
                public:


                        placeholder_context(){
                                push_regular();
                        }
                        ~placeholder_context(){
                                assert( stack_.size() == 1 && "inconsistnet push/pop");
                        }
                        void push_regular(){
                                stack_.emplace_back(ctx_type::regular, map_t{});
                        }
                        void push_breakable(){
                                stack_.emplace_back(ctx_type::breakable, map_t{});
                                map_placeholder_ctx_( stack_.back(), break_placeholder() );
                                map_placeholder_ctx_( stack_.back(), continue_placeholder() );
                        }
                        
                        void pop(){
                                stack_.pop_back();
                        }

                        constexpr size_t break_placeholder()const{return -1;}
                        constexpr size_t continue_placeholder()const{return -2;}

                        size_t map_placeholder(size_t idx){
                                assert( idx != break_placeholder() && "can't map to break");
                                return map_placeholder_ctx_( stack_.back() , idx );
                        }
                        // the only difference is that it maps to the closest breakable context rather than the top
                        size_t map_break(){
                                for( auto iter = stack_.rbegin(),end = stack_.rend();
                                        iter!=end; ++iter)
                                {
                                        if( std::get<0>(*iter) == ctx_type::breakable ){
                                                return map_placeholder_ctx_( *iter, break_placeholder() );
                                        }
                                }
                                BOOST_THROW_EXCEPTION(std::domain_error("break outside breakable context"));
                        }
                        size_t map_continue(){
                                for( auto iter = stack_.rbegin(),end = stack_.rend();
                                        iter!=end; ++iter)
                                {
                                        if( std::get<0>(*iter) == ctx_type::breakable ){
                                                return map_placeholder_ctx_( *iter, continue_placeholder() );
                                        }
                                }
                                BOOST_THROW_EXCEPTION(std::domain_error("continue outside breakable context"));
                        }

                        void debug_()const{
                                for(size_t i=0;i!=stack_.size();++i){
                                        auto const& ctx = stack_[i];
                                        std::cout << boost::format("%-2s %10s ") % i % 
                                                ( std::get<0>(ctx) == ctx_type::regular ?
                                                  "regular" : "breakable" );
                                        boost::for_each( std::get<1>(stack_[i]), [](auto&& p){ 
                                                std::cout << boost::format("(%s=>%s),") % p.first % p.second;
                                        });
                                        std::cout << std::endl;
                                }
                        }
                private:
                        size_t map_placeholder_ctx_(internal_ctx_t& ctx, size_t idx){
                                if( std::get<1>(ctx).count(idx) == 0 ){
                                        std::get<1>(stack_.back()).insert(std::make_pair(idx,p_idx_));
                                        ++p_idx_;
                                }
                                return std::get<1>(ctx).find(idx)->second;
                        }
                        size_t p_idx_{1};
                        std::vector< internal_ctx_t > stack_;
                };
                

                struct compiler_contex{

                        struct tag_expr{};
                        struct tag_placeholder{};
                        struct tag_goto{};
                        struct tag_goto_if{};
                        struct tag_break{};
                        struct tag_continue{};
                        struct tag_push{};


                        using inter_t = boost::variant<
                                std::tuple<tag_expr, expression >
                              , std::tuple<tag_push, expression >
                              , std::tuple<tag_placeholder, size_t >
                              , std::tuple<tag_goto, size_t>
                              , std::tuple<tag_goto_if, expression, size_t >
                              , std::tuple<tag_break >
                              , std::tuple<tag_continue >
                        >;

                        struct inter_t_to_string_ : boost::static_visitor<std::string>
                        {
                                result_type operator()(std::tuple<tag_expr, expression > const& arg)const{
                                        return str(boost::format("tag_expr(%s)") % std::get<1>(arg));
                                }
                                result_type operator()(std::tuple<tag_push, expression > const& arg)const{
                                        return str(boost::format("tag_push(%s)") % std::get<1>(arg));
                                }
                                result_type operator()(std::tuple<tag_placeholder, size_t > const& arg)const{
                                        return str(boost::format("tag_placeholder(%s)") % std::get<1>(arg));
                                }
                                result_type operator()(std::tuple<tag_goto, size_t> const& arg)const{
                                        return str(boost::format("tag_goto(%s)") % std::get<1>(arg));
                                }
                                result_type operator()(std::tuple<tag_goto_if, expression, size_t > const& arg)const{
                                        return str(boost::format("tag_goto_if(%s,%s)") % std::get<1>(arg) % std::get<2>(arg));
                                }
                                result_type operator()(std::tuple<tag_break > const& )const{
                                        return "tag_break";
                                }
                                result_type operator()(std::tuple<tag_continue > const& )const{
                                        return "tag_continue";
                                }
                        };
                        

                        void emit_expr( expression const& expr ){
                                inter_.emplace_back( std::make_tuple(tag_expr(), expr) );
                        }
                        void emit_goto( size_t idx){
                                inter_.emplace_back( std::make_tuple(tag_goto(), pctx_.map_placeholder(idx) ) );
                        }
                        void emit_goto_if( expression const& expr, size_t idx ){
                                inter_.emplace_back( std::make_tuple(tag_goto_if(), expr, pctx_.map_placeholder(idx) ) );
                        }
                        void emit_break(){
                                inter_.emplace_back( std::make_tuple(tag_goto(), pctx_.map_break() ) );
                        }
                        void emit_continue(){
                                inter_.emplace_back( std::make_tuple(tag_goto(), pctx_.map_continue() ) );
                        }
                        void emit_push( expression const& expr){
                                inter_.emplace_back( std::make_tuple(tag_push(), expr ) );
                                
                        }
                        
                        void placeholder(size_t idx){
                                inter_.emplace_back( std::make_tuple(
                                        tag_placeholder()
                                      , pctx_.map_placeholder(idx)
                                ));
                        }
                        void break_placeholder(){
                                inter_.emplace_back( std::make_tuple(
                                        tag_placeholder()
                                      , pctx_.map_break()
                                ));
                        }
                        void continue_placeholder(){
                                inter_.emplace_back( std::make_tuple(
                                        tag_placeholder()
                                      , pctx_.map_continue()
                                ));
                        }

                public:

                        void begin_placeholder_context(){
                                pctx_.push_regular();
                        }
                        void begin_breakable_placeholder_context(){
                                pctx_.push_breakable();
                        }
                        void end_placeholder_context(){
                                pctx_.pop();
                        }

                        struct resolver_cache : boost::static_visitor<>{
                                void operator()(std::tuple<tag_expr, expression > const& ){
                                        ++idx_;
                                }
                                void operator()(std::tuple<tag_push, expression > const& ){
                                        ++idx_;
                                }
                                void operator()(std::tuple<tag_placeholder, size_t > const& arg){
                                        cache_.insert(std::make_pair(std::get<1>(arg),idx_));
                                }
                                void operator()(std::tuple<tag_goto, size_t> const& ){
                                        ++idx_;
                                }
                                void operator()(std::tuple<tag_goto_if, expression, size_t > const& ){
                                        ++idx_;
                                }
                                void operator()(std::tuple<tag_break> const& ){
                                        ++idx_;
                                }
                                void operator()(std::tuple<tag_continue> const& ){
                                        ++idx_;
                                }
                                auto operator[](size_t idx)const{
                                        if( cache_.count(idx) == 0 )
                                                BOOST_THROW_EXCEPTION(std::domain_error("un mapped placeholder (" + boost::lexical_cast<std::string>(idx) + ")"));
                                        return cache_.find(idx)->second;
                                }
                                void debug()const{
                                        for( auto const& p : cache_ ){
                                                std::cout << "(" << p.first << "=>" << p.second << "), ";
                                        }
                                        std::cout << "\n";
                                }
                        private:
                                size_t idx_{0};
                                std::map<size_t,size_t> cache_;

                        }; 

                        struct resolver_ : boost::static_visitor<>{
                                resolver_( resolver_cache const& cache):cache_(cache){}

                                void operator()(std::tuple<tag_expr, expression > const& arg){
                                        vec_.emplace_back( std::make_tuple(
                                                tag::_expr
                                              , std::get<1>( arg ) ) );
                                }
                                void operator()(std::tuple<tag_push, expression > const& arg){
                                        vec_.emplace_back( std::make_tuple(
                                                tag::_push
                                              , std::get<1>( arg ) ) );
                                }
                                void operator()(std::tuple<tag_placeholder, size_t > const& ){
                                }
                                void operator()(std::tuple<tag_goto, size_t> const& arg){
                                        vec_.emplace_back( std::make_tuple(
                                                tag::_goto_
                                              , cache_[ std::get<1>( arg ) ]
                                              ));
                                }
                                void operator()(std::tuple<tag_goto_if, expression, size_t > const& arg){
                                        vec_.emplace_back( std::make_tuple(
                                                tag::_goto_if
                                              , std::get<1>( arg )
                                              , cache_[ std::get<2>( arg ) ] ) 
                                        );
                                }
                                void operator()(std::tuple<tag_break> const& ){
                                }
                                void operator()(std::tuple<tag_continue> const& ){
                                }
                                program finish(){
                                        return program{vec_};
                                }
                        private:
                                resolver_cache cache_;
                                std::vector< backend_statement::impl_t > vec_;
                        };

                        auto compile(){
                                resolver_cache cache;
                                for( size_t idx{0},sz{inter_.size()};idx!=sz;++idx){
                                        boost::apply_visitor( cache, inter_[idx] );
                                }
                                resolver_ res(cache);
                                for( size_t idx{0},sz{inter_.size()};idx!=sz;++idx){
                                        boost::apply_visitor( res, inter_[idx] );
                                }
                                return res.finish();
                        }

                        void debug()const{
                                for(size_t i=0;i!=inter_.size();++i){
                                        std::cout << boost::format("%2i") % i << "    " 
                                                << boost::apply_visitor(
                                                        inter_t_to_string_(), inter_[i] 
                                                )
                                                << "\n"
                                        ;
                                }
                        }
                private:
                        std::vector<inter_t> inter_;

                        placeholder_context pctx_;
                };
        }
}
