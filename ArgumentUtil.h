//
// Created by Aurora on 2021/1/21.
//

#ifndef PROJECT2_ARGUMENTUTIL_H
#define PROJECT2_ARGUMENTUTIL_H
#include <bits/stdc++.h>

template <typename T> class InputParameter {
public:
    InputParameter() : Value(nullptr) {}
    explicit InputParameter(const auto& other) : Value(other) {}

    T& operator= (T other) = delete;
    operator T() { return Value; }

    friend auto In(auto value);
private:
    T Value;
};

template <typename T> class OutputParameter {
public:
    explicit OutputParameter(T& other) : Value(&other) {}

    operator T() = delete;
    //Target& operator= (const Target&) = delete;
    T& operator= (T other) { return *Value =  move(other); }
    //Target& operator= (InputParameter<Target> other) { return *Value = move((Target)other); }

    friend auto Out(auto& value);
private:
    std::remove_reference_t<T>* Value;
};

template <typename T> static auto Out(T& thing) {
    return OutputParameter<T>(thing);
}

template <typename T> static auto In(T thing) {
    return InputParameter<T>(thing);
}

#endif //PROJECT2_ARGUMENTUTIL_H
