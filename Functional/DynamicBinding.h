//
// Created by Aurora on 2021/2/15.
//

#ifndef PROJECT2_DYNAMICBINDING_H
#define PROJECT2_DYNAMICBINDING_H
#include "../Utility.h"

template <int PlaceholderCount, typename Ret, typename... Args> struct Binder {
    std::tuple<Args...> TupleForCall;
    List<int> BindIndexLst;
    Func<Ret(Args...)> BindFunc;

    // Pre-binding the known arguments and store the unknown(binding) arguments' index for later.
    void PreBind(auto&&... item) {
        using namespace Utility;
        // Iterate over the whole argument-pack and classify out the real value and the placeholders (substitute for later).
        For([&] (auto element, auto index, [[maybe_unused]] auto flag) {
            if constexpr ( !std::is_same_v<decltype(element), Placeholder> ) {
                // (1) If it is NOT a placeholder, store it to the "call tuple".
                std::get<index.value>(TupleForCall) = element;
            }
            else {
                // (2) Else, store the index of the placeholder to the BindIndexLst.
                BindIndexLst.Add(index);
            }
        }, item...);
    }

    // Binding the real arguments to the placeholders.
    template <typename... TInvArgs> auto operator()(TInvArgs&&... args) -> Ret {
        return CallImpl(std::forward<TInvArgs>(args)...);
    }
private:
    template <typename... TInvArgs> decltype(auto) CallImpl(TInvArgs&&... args) {
        using namespace Utility;
        // Check does the argument-pack's size == placeholders' size.
        static_assert(sizeof...(TInvArgs) == PlaceholderCount,
                      "The arguments size of the function after binding does NOT equals to the placeholders' amount.");

        // Iterate over the argument-pack, than binding the argument to corresponding Placeholder.
        For([&] (auto element, auto index, [[maybe_unused]] auto flag) {
            VisitAt(TupleForCall, BindIndexLst[index], [&] (auto& realElement) {
                realElement = std::forward<decltype(element)>(element);
            });
        }, args...);

        // Call the function and return the result.
        return functional_detail::callFunc(BindFunc, TupleForCall);
    }
};

template <typename Ret, typename... Args> auto Bind(Func<Ret(Args...)> func, auto&&... bindLst) {
    auto binder = Binder<Utility::Count<Placeholder, std::remove_reference_t<decltype(bindLst)>...>(), Ret, Args...>();
    binder.BindFunc = func;
    binder.PreBind(bindLst...);
    return binder;
}

#endif //PROJECT2_DYNAMICBINDING_H
