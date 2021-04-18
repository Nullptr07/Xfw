//
// Created by Aurora on 2021/2/13.
//

#ifndef PROJECT2_FUNC_H
#define PROJECT2_FUNC_H
#include <bits/stdc++.h>

template <typename Class, typename Ret, typename... Args> auto MemFn( Ret(Class::*func)(Args...) ) {
    static auto Func = func;
    return [&] (Class cls, Args&&... args) {
        return (cls.*Func)(std::forward<Args>(args)...);
    };
}

union [[gnu::may_alias]] FuncData {
    [[nodiscard]] void* Get() { return &PodData[0]; }
    [[nodiscard]] const void* Get() const { return &PodData[0]; }

    template <typename T> T& Get() { return *static_cast<T*>(Get()); }
    template <typename T> const T& Get() const { return *static_cast<const T*>(Get()); }

    FuncData() = default;
    FuncData(const char* data, int size) {
        for (int i=0; i<size; ++i)
            PodData[i] = data[i];
    }
    char PodData[32] = {0};
};

template <typename Ret, typename... Args> struct Func {};
template <typename Ret, typename... Args> struct Func<Ret(Args...)> {
    using TInvoker = Ret(*)(bool, char*, FuncData, Args&&...);
    using TManager = void(*)(char*, char*, bool);

    TManager Manager;
    TInvoker Invoker;
    bool     Local;
    int      Size;
    char*    Functor;
    FuncData LocalFunctor;

    template <typename TFunctor> static auto GenerateInvoker(bool local, TFunctor* func, FuncData localFunc, Args&&... args) {
        if (local) {
            if constexpr ( std::is_same_v<Ret, void> )
                localFunc.Get<TFunctor>()(std::forward<Args>(args)...);
            else
                return localFunc.Get<TFunctor>()(std::forward<Args>(args)...);
        } else {
            if constexpr ( std::is_same_v<Ret, void> )
                (*func)(std::forward<Args>(args)...);
            else
                return (*func)(std::forward<Args>(args)...);
        }
    }

    template <typename TFunctor> static void GenerateManager(TFunctor* dst, TFunctor* src, bool construct) {
        if (construct)
            new (dst) TFunctor (*src);
        else
            dst->~TFunctor();
    }

    constexpr static bool LocalConstruct(auto functor) {
        return sizeof(functor) < 32 && std::is_trivially_copyable_v<decltype(functor)>;
    }

    Func() : Size(0) {}
    template <typename TFunctor> Func(TFunctor functor)
            : Size(sizeof(functor))
            , Local(LocalConstruct(functor))
            , Invoker(reinterpret_cast<TInvoker>(GenerateInvoker<TFunctor>))
    {
        if constexpr (sizeof(functor) < 32 && std::is_trivially_copyable_v<decltype(functor)>)
            LocalFunctor = FuncData{ reinterpret_cast<const char*>(&functor), Size };
        else {
            Functor = new char[Size];
            Manager = reinterpret_cast<TManager>(GenerateManager<TFunctor>);
            Manager(Functor, reinterpret_cast<char*>(&functor), true);
        }
    }
    Func& operator= (const Func& rhs) {
        if (&rhs == this)
            return (*this);

        Size = rhs.Size;
        Local = rhs.Local;
        Manager = rhs.Manager;
        Invoker = rhs.Invoker;
        if (Local) {
            LocalFunctor = rhs.LocalFunctor;
        } else {
            Functor = new char[Size];
            Manager(Functor, rhs.Functor, true);
        }
        return (*this);
    }
    Func& operator= (Func&& rhs) noexcept {
        if (&rhs == this)
            return (*this);

        this->~Func();
        std::swap(Size, rhs.Size);
        if (Size == 0)
            return (*this);
        std::swap(Local, rhs.Local);
        std::swap(Manager, rhs.Manager);
        std::swap(Invoker, rhs.Invoker);

        if (Local) {
            std::swap(LocalFunctor, rhs.LocalFunctor);
        } else {
            std::swap(Functor, rhs.Functor);
        }
        return (*this);
    }
    Func(const Func& rhs) {
        (*this) = rhs;
    }
    Func(Func&& rhs) noexcept
        : Size(0)
        , Local(true)
        , Manager(nullptr)
        , Invoker(nullptr)
        , Functor(nullptr)
        , LocalFunctor()
    {
        std::swap(Size, rhs.Size);
        std::swap(Local, rhs.Local);
        std::swap(Manager, rhs.Manager);
        std::swap(Invoker, rhs.Invoker);
        if (Local) {
            std::swap(LocalFunctor, rhs.LocalFunctor);
        } else {
            std::swap(Functor, rhs.Functor);
        }
    }
    ~Func() {
        if (!Local)
            Manager(Functor, nullptr, false);
    }

    Ret operator()(auto&&... args) const {
        static_assert(sizeof...(args) == sizeof...(Args), R"Error(
            The size of the argument pack on invocation does not equals to the size of the argument pack of this BindFunc.
        )Error");

        if (Size == 0) {
            if constexpr ( std::is_same_v<Ret, void> )
                return;
            else if constexpr ( std::is_reference_v<Ret> ) {
                static std::remove_reference_t<Ret> val{};
                return val;
            }
            else
                return {};
        }
        else
            return Invoker(Local, Functor, LocalFunctor, std::forward<Args>(args)...);
    }
};

struct Placeholder {

} _1, _2, _3;

#endif //PROJECT2_FUNC_H