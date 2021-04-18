//
// Created by Aurora on 2021/2/13.
//

#ifndef PROJECT2_LAMBDA_H
#define PROJECT2_LAMBDA_H
#include <bits/stdc++.h>

namespace functional_detail {
    template <typename... AllArgs>
    struct index {};

    template <typename Target, typename... AllArgs>
    struct index<Target, Target, AllArgs...> : std::integral_constant<size_t, 0> { };

    template <typename Target, typename First, typename... Remain>
    struct index<Target, First, Remain...> : std::integral_constant<size_t, 1 + index<Target,Remain...>::value> { };

    template<typename What, typename ... Args> struct is_present {
        static constexpr bool value {(std::is_same_v<What, Args> || ...)};
    };
    template<int N, typename... Types> auto& get(std::tuple<Types...> tuple) {
        static_assert(N < sizeof...(Types), R"Error(
                      The lambda is trying to visit invalid lambda argument.
                      Check if
                            (1) the caller forgot to pass the missing arguments
                            (2) the callee use the wrong lambda argument.
                      )Error");
        enum { TMP_COUNTER = __COUNTER__ };
        return std::get<N>(tuple);
    }
    template <typename Target, typename... Args> void chg(Target newValue, Args&... args) {
        if constexpr (is_present<Target, Args...>::value)
            std::get<index<Target, Args...>::value>(std::forward_as_tuple(args...)) = newValue;
    }
    template <unsigned int... Seq, typename... Args> decltype(auto) callFunc(auto func, std::integer_sequence<unsigned int, Seq...>, std::tuple<Args...> tupleForCall) {
        return func(std::forward<Args>(std::get<Seq>(tupleForCall))...);
    }
    template <typename... Args> decltype(auto) callFunc(auto func, std::tuple<Args...> tupleForCall) {
        return callFunc(func, std::make_index_sequence<sizeof...(Args)>(), tupleForCall);
    }
}

#define lamarg(NUMBER) (functional_detail::get<NUMBER>(ArgPack))
#define $return(EXPR) ({ auto result = (EXPR); return result; result; })
#define $break ({                                                                                           \
                    func                                                                        \
                    void();                                                                                 \
               })
#define $continue ({                                                                        \
                        functional_detail::chg<LoopFlag>(LoopFlag::Continue, lamArgs...);   \
                        return void();                                                      \
                        void();                                                             \
                  })
#define $$break(retExpr) ({                                                                                             \
                            functional_detail::chg<LoopFlag>(LoopFlag::Break, lamArgs...);                              \
                            auto ret = (retExpr);                                                                       \
                            return ret;                                                                                 \
                            ret;                                                                                        \
                         })
#define $$continue(retExpr) ({                                                                                          \
                                functional_detail::chg<LoopFlag>(LoopFlag::Continue, lamArgs...);                       \
                                auto ret = (retExpr);                                                                   \
                                return ret;                                                                             \
                                ret;                                                                                    \
                            })
#define $1 lamarg(0)
#define $2 lamarg(1)
#define $3 lamarg(2)
#define $4 lamarg(3)
#define $5 lamarg(4)
#define $6 lamarg(5)
#define $7 lamarg(6)
#define $8 lamarg(7)
#define $9 lamarg(8)
#define $10 lamarg(9)
#define LamHead [[maybe_unused]] auto ArgPack = std::forward_as_tuple(lamArgs...);          \
                [[maybe_unused]] const unsigned int ArgSize = sizeof...(LamArgs);

#define $(LamBody) [&]<typename... LamArgs> (LamArgs&&... lamArgs) {                        \
    LamHead;                                                                                \
    return ({ LamBody; });                                                                  \
}

#define $$(LamBody) [&]<typename... LamArgs> (LamArgs&&... lamArgs) {                       \
    LamHead;                                                                                \
    return ({ LamBody; void(); });                                                          \
}

#define $NoCapture(LamBody) []<typename... LamArgs> (LamArgs&&... lamArgs) {                \
    LamHead;                                                                                \
    return ({ LamBody; });                                                                  \
}
#define $$NoCapture(LamBody) []<typename... LamArgs> (LamArgs&&... lamArgs) {               \
    LamHead;                                                                                \
    return ({ LamBody; void(); });                                                          \
}

#endif //PROJECT2_LAMBDA_H
