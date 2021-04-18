//
// Created by Aurora on 2021/1/21.
//

#ifndef PROJECT2_FUNCTIONAL_H
#define PROJECT2_FUNCTIONAL_H
#include <bits/stdc++.h>
#include "Functional/Lambda.h"
#include "Functional/Func.h"

class Curryable {
private:
    template<typename... Args> void HandleArgs(Args&&... args) {
        std::vector<std::any> data;
        for (auto&& element : {args...})
            data.push_back(element);
        ProcessFunc(data);
    }
public:
    virtual void ProcessFunc(std::vector<std::any> data) {}

    template<typename... Args> auto operator()(Args&&... args) {
        HandleArgs(args...);

        return [&]<typename... NewArgs> (NewArgs&&... newArgs) {
            return (*this)(newArgs...);
        };
    }
};

#endif //PROJECT2_FUNCTIONAL_H
