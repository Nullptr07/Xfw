//
// Created by Aurora on 2021/2/13.
//

#ifndef PROJECT2_UTILITY_H
#define PROJECT2_UTILITY_H
#include <bits/stdc++.h>
#include "Functional/Lambda.h"
#include "Functional/Func.h"

namespace Utility {
    /**
     * @brief A flag which used to control the loop state.
     */
    enum LoopFlag {
        None = 0,
        Break = 1,
        Continue = 2
    };

    template<int Index>
    void For(auto func) {}

    template<int Index = 0, typename First, typename... RemainArgs>
    void For(auto func, First&& firstArg, RemainArgs&&... args) {
        LoopFlag flag{LoopFlag::None};
        func(std::forward<First>(firstArg), std::integral_constant<int, Index>(), flag);
        if (flag == LoopFlag::Break) return;
        For<Index + 1>(func, std::forward<RemainArgs>(args)...);
    }

    template<size_t I>
    struct VisitImpl {
        template<typename T, typename F>
        static void Visit(T &tup, size_t idx, F fun) {
            if (idx == I - 1)
                fun(std::get<I - 1>(tup));
            else
                VisitImpl<I - 1>::Visit(tup, idx, fun);
        }
    };

    template<>
    struct VisitImpl<0> {
        template<typename T, typename F>
        static void Visit(T &tup, size_t idx, F fun) {
            fun(std::get<0>(tup));
        }
    };

    template<typename F, typename... Ts>
    void VisitAt(std::tuple<Ts...> const &tup, size_t idx, F fun) {
        VisitImpl<sizeof...(Ts)>::Visit(tup, idx, fun);
    }

    template<typename F, typename... Ts>
    void VisitAt(std::tuple<Ts...> &tup, size_t idx, F fun) {
        VisitImpl<sizeof...(Ts)>::Visit(tup, idx, fun);
    }

    template<typename TargetType>
    constexpr int Count() {
        return 0;
    }

    template<typename TargetType, typename First, typename... Args>
    constexpr int Count(First firstArg, Args... args) {
        if constexpr (std::is_same_v<First, TargetType>)
            return 1 + Count<TargetType>(args...);
        else
            return Count<TargetType>(args...);
    }

    template<typename TargetType, typename First, typename... Args>
    constexpr int Count() {
        if constexpr (std::is_same_v<First, TargetType>) {
            return 1 + Count<TargetType, Args...>();
        }
        else {
            return Count<TargetType, Args...>();
        }
    };

    template <typename TElement> struct Enumerator {
        TElement* Current;

        Enumerator() : Current(nullptr) {}
        Enumerator(TElement* current) : Current(current) {}
        Enumerator(const Enumerator& rhs) { (*this) = rhs; }
        Enumerator(Enumerator&& rhs) noexcept : Current(nullptr) { (*this) = rhs; }
        Enumerator& operator= (const Enumerator& rhs) {
            if (this == &rhs)
                return (*this);
            Current = rhs.Current;
            return (*this);
        }
        Enumerator& operator= (Enumerator&& rhs) noexcept {
            std::swap(Current, rhs.Current);
            return (*this);
        }

        virtual Enumerator& operator++ () { MoveNext(); return (*this); }
        //virtual TElement& operator* () { return *Current; }
        //virtual bool operator== (const Enumerator& rhs) = 0;
        //virtual bool operator!= (const Enumerator& rhs) = 0;
        virtual void MoveNext() = 0;
        virtual bool End() = 0;
        virtual ~Enumerator() = default;
    };

    namespace detail {
        template< class T, class U >
        concept SameHelper = std::is_same_v<T, U>;
    }

    template< class T, class U >
    concept SameAs = detail::SameHelper<T, U> && detail::SameHelper<U, T>;

    template <typename T, typename TElement> concept Enumerable = requires(T thing) {
        { thing.Enumerable } -> SameAs<TElement>;
    };
    template <typename T, typename TElement> concept FiniteEnumerable = requires(T thing) {
        { thing.FiniteEnumerable } -> SameAs<TElement>;
    };

    template <typename TElement, template <typename T> typename TContainer>
    auto Enumerate(TContainer<TElement> source,
                   unsigned int amount = 1,
                   const Func<TElement*(int, LoopFlag&)>& loopFunc = $NoCapture(TElement{}))
    {
        static_assert( Enumerable<TContainer<TElement>, TElement>, "The source type must be enumerable" );
        if constexpr ( FiniteEnumerable<TContainer<TElement>, TElement> ) {
            amount = source.Size();
        }

    }
}

#endif //PROJECT2_UTILITY_H
