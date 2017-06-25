#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_char_class.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <boost/config/warning_disable.hpp>

namespace qi = boost::spirit::qi;
namespace standard = boost::spirit::standard;
namespace phoenix = boost::phoenix;

// TODO:
// Files cannot end in a comment

namespace parser {
    template <typename Iterator>
    struct p_grammar : qi::grammar<Iterator, std::vector<std::vector<std::string>>, standard::space_type> {
        p_grammar() : p_grammar::base_type(spec) {
            using qi::lit;
            using qi::lexeme;
            using qi::eoi;
            using qi::eol;
            using qi::blank;
            using standard::char_;
            using boost::spirit::int_;
            using standard::string;
            using standard::no_case;
            using boost::spirit::repeat;
            using standard::alpha;
            using qi::_val;
            using qi::_1;
            using phoenix::push_back;
            using phoenix::insert;

            spec %= +module >> eoi;

            module %= utype | mtype | proctype | init | never | trace | decl_lst | ltl | inlin | ';'
                    | no_case["C_CODE"] >> '{' >> *char_ >> '}'
                    | no_case["C_EXPR"] >> '{' >> *char_ >> '}'
                    | no_case["C_DECL"] >> lexeme[*char_] >> lexeme[*char_] >> -lexeme[*char_]
                    | no_case["C_DECL"] >> lexeme[*char_] >> lexeme[*char_];

            proctype    %= -active >> no_case["PROCTYPE"] [push_back(_val,(char*)"PROCTYPE")] >> name
                        >> '(' >> -decl_lst >> ')' >> -priority >> -enabler >> '{' >> sequence >> '}';

            init        = no_case["INIT"] >> -priority >> '{' >> sequence >> '}';
            never       = no_case["NEVER"] >> '{' >> sequence >> '}';
            trace       = no_case["TRACE"] >> '{' >> sequence >> '}' | no_case["NOTRACE"] >> '{' >> sequence >> '}';
            utype       = no_case["TYPEDEF"] >> name >> '{' >> decl_lst >> '}' >> -lit(';');
            mtype       = no_case["MTYPE"] >> lit('=') >> '{' >> name % ',' >> '}' >> -lit(';');

            decl_lst    %= one_decl % -lit(';') >> -lit(';');
            one_decl    = -(visible) >> type_name [push_back(_val,_1)] >> ivar [push_back(_val,_1)] % ',';

            type_name   = no_case["BIT"]    [_val = "BIT"]
                        | no_case["BOOL"]   [_val = "BOOL"]
                        | no_case["BYTE"]   [_val = "BYTE"]
                        | no_case["PID"]    [_val = "PID"]
                        | no_case["SHORT"]  [_val = "SHORT"]
                        | no_case["INT"]    [_val = "INT"]
                        | no_case["MTYPE"]  [_val = "MTYPE"]
                        | no_case["CHAN"]   [_val = "CHAN"];
                        //| name              [_val = "UTYPE"] ;

            active      = no_case["ACTIVE"] >> -('[' >> cons >> ']');
            priority    = no_case["PRIORITY"] >> cons;
            enabler     = no_case["PROVIDED"] >> '(' >> expr >> ')';
            visible     = no_case["HIDDEN"] | no_case["SHOW"];

            sequence    %= step % -lit(';') >> -lit(';');
            step        %= decl_lst
                        | stmt >> -(no_case["UNLESS"] >> stmt)
                        | no_case["XR"] >> varref % ','
                        | no_case["XS"] >> varref % ',';

            ivar        = name >> -iarray >> -(idecl);
            iarray      = lit('[') >> cons >> ']';
            idecl       = lit('=') >> ( any_expr | ch_init );
            ch_init     = lit('[') >> cons >> no_case["OF"] >> '{' >> (type_name % ',');
            varref      = name >> -('[' >> bin_expr >> ']') >> -('.' >> varref);

            send        = varref >> '!' >> send_args
                        | varref >> '!' >> '!' >> send_args;
            recieve     = varref >> '?' >> recv_args
                        | varref >> '?' >> '?' >> recv_args
                        | varref >> '?' >> '<' >> recv_args
                        | varref >> '?' >> '?' >> '<' >> recv_args;
            recv_poll   = varref >> '?' >> '[' >> recv_args >> ']'
                        | varref >> '?' >> '?' >> '[' >> recv_args >> ']';
            send_args   = arg_lst | any_expr >> '(' >> arg_lst >> ')';
            arg_lst     = bin_expr % ',';
            recv_args   = recv_arg >> '(' >> recv_args >> ')' | recv_arg % ',';
            recv_arg    = no_case["EVAL"] >> '(' >> varref >> ')'
                        | -lit('-') >> cons
                        | varref;

            assign      = varref >> '=' >> bin_expr | varref >> "++" | varref >> "--";

            stmt        %=  no_case["IF"] >> "::" >> options >> no_case["FI"]
                        | no_case["DO"] >> "::" >> options >> no_case["OD"]
                        | no_case["ATOMIC"] >> '{' >> sequence >> '}'
                        | no_case["D_STEP"] >> '{' >> sequence >> '}'
                        | bin_expr >> "->" >> sequence
                        | '{' >> sequence >> '}'
                        | send
                        | recieve
                        | assign
                        | no_case["ELSE"] >> "->" >> sequence
                        | no_case["BREAK"]
                        | func_call [push_back(_val,(char*)"INLINE")]
                        | no_case["GOTO"] >> name
                        | label
                        | no_case["PRINTF"] >> '(' >> qstring >> -(',' >> arg_lst) >> ')'
                        | no_case["PRINTM"] >> '(' >> expr >> ')'
                        | no_case["ASSERT"] >> expr
                        | expr >> *(andor >> expr)
                        | no_case["C_CODE"] >> '[' >> *char_ >> ']' >> '{' >> *char_ >> '}'
                        | no_case["C_EXPR"] >> '[' >> *char_ >> ']' >>'{' >> *char_ >> '}'
                        | assign
                        | run
                        ;

            options     %= sequence % string("::");

            andor       = string("&&") | "||";
            binarop     = string("<=") | "=>" | "==" | "!=" | "<<" | ">>"  | andor| char_("*/%&^|<>+-");
            unarop      = lit('!') | lit('~') | lit('-');

            expr        = bin_expr | '(' >> bin_expr >> ')' | chanop >> '(' >> varref >> ')';
            bin_expr    = any_expr >> *(binarop >> any_expr );
            any_expr    = unarop >> bin_expr
                        | '(' >> bin_expr >> "->" >> bin_expr >> ':' >> bin_expr >> ')'
                        | no_case["LEN"] >> '(' >> varref >> ')'
                        | recv_poll
                        | '(' >> bin_expr >>')'
                        | no_case["TIMEOUT"]
                        | no_case["NP_"]
                        | no_case["ENABLED"] >> '(' >> bin_expr >> ')'
                        | no_case["PC_VALUE"] >> '(' >> bin_expr >> ')'
                        | no_case["RUN"] >> name >> '(' >> -arg_lst >> ')' >> -priority
                        | name >> '[' >> bin_expr >> ']' >> lit('@') >> name
                        | cons
                        | varref;
                        ;

            chanop      = no_case["FULL"] | no_case["EMPTY"] | no_case["NFULL"] | no_case["NEMPTY"];

            name        = Predefines | ( lexeme[alpha >> *(alpha | char_("0-9") | '_')] - no_case[keyword] );

            cons        = no_case["TRUE"] | no_case["FALSE"] | no_case["SKIP"]  | int_;

           keyword     = string("accept") | "active" | "arrays" | "assert" | "assign" | "atomic"
                        | "bit" | "bool" | "break" | "byte" | "c_code" | "c_decl" | "c_expr" | "c_state" | "c_track" | "chan"
                        | "comments" | "cond_expr" | "condition" | "D_proctype" | "d_step" | "datatypes" | "do" | "else" | "empty"
                        | "enabled" | "end" | "eval" | "flase" | "fi" | "float" | "full" | "goto" | "hiden" | "hierarchy" | "if"
                        | "init" | "inline" | "int" | "labels" | "len" | "local" | "ltl" | "macros" | "mtype" | "nempty" | "never"
                        | "nfull" | "notrace" | "np_" | "od" | "pc_value" | "pid" | "pointers" | "poll" | "printf" | "printm" | "priority"
                        | "probabilities" | "procedures" | "proctype" | "progress" | "provided" | "rand" | "realtime" | "recieve"
                        | "remoterefs" | "run" | "scanf" | "send" | "separators" | "sequence" | "short" | "show" | "skip" | "STDIN"
                        | "timeout" | "trace" | "true" | "typedef" | "unless" | "unsigned" | "xr" | "xs";

            Predefines  = string("_pid") | "_" | "_last" | "_nr_pr" | "np_";
            qstring     = '"' >> *(char_ - '"') >> '"';

            ltl         = no_case["LTL"] >> -name >> '{' >> *(char_ - '}') >> '}';

            label       = no_case["END"] >> -name >> lit(':')
                        | no_case["ACCEPT"]  >> -name >> ':'
                        | no_case["PROGRESS"] >> -name >> ':'
                        | name >> ':';

            run         = no_case["RUN"] >> name >> '(' >> -arg_lst >> ')';

            inlin       %= no_case["INLINE"] [push_back(_val,(char*)"INLINE")] >> name >> '(' >> -arg_lst >> ')' >> '{' >> sequence >> '}';

            func_call   =  name >> '(' >> -arg_lst >> ')';

//            module.name("module");
//            spec.name("spec");
//            never.name("never");
//            on_error<fail>( spec , std::cout << _4 << std::endl );
//            on_error<fail>( module , std::cout << _4 << std::endl );
//            on_error<fail>( never , std::cout << _4 << std::endl );

        }

        qi::rule<Iterator, std::string(), standard::space_type> type_name, ivar, varref, name, func_call;
        qi::rule<Iterator, std::vector<std::string>, standard::space_type> one_decl, proctype, sequence, step, decl_lst, inlin, stmt, options;
        qi::rule<Iterator, std::vector<std::vector<std::string>>, standard::space_type> module, spec;
        qi::rule<Iterator, standard::space_type> init, never, trace, utype, mtype, assign, priority, enabler, visible, active,
                    iarray, idecl, ch_init, send, recieve, recv_poll, send_args, arg_lst, recv_args, recv_arg,
                    andor, binarop, unarop, any_expr, expr, chanop, cons, sep, bin_expr, comment, qstring, ltl,
                    label, run, keyword, Predefines;

    };
}
