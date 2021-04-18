//
// Created by Aurora on 2021/1/28.
//

#ifndef PROJECT2_List_H
#define PROJECT2_List_H
#include <bits/stdc++.h>
#include "Functional/Lambda.h"
#include "Utility.h"

template <class U, class T>
struct is_explicitly_convertible
{
    enum {value = std::is_constructible<T, U>::value && !std::is_convertible<U, T>::value};
};

template <typename From, typename To> constexpr bool IsExplicitlyConvertible = is_explicitly_convertible<From, To>::value;

template <typename T> struct Field {
private:
    T Value;
    Func<T()> Getter;
    Func<void(T)> Setter;
public:
#define ReqConv(From) requires std::is_convertible_v<From, T>
    Field() : Value(T{}) {}
    template <typename TNew> Field(TNew newValue) ReqConv(TNew) : Value(static_cast<T>(newValue)) {}
    template <typename TNew> Field(const Field<TNew>& other) ReqConv(TNew) {
        (*this) = other;
    }
    template <typename TNew> Field(Field<TNew>&& other) noexcept ReqConv(TNew) : Value(T{}) {
        (*this) = other;
    }
    template <typename TNew> auto& operator= (const Field<TNew>& other) ReqConv(TNew) {
        if (this == &other)
            return (*this);
        Value = other.Value;
        return (*this);
    }
    template <typename TNew> auto& operator= (Field<TNew>&& other) noexcept ReqConv(TNew) {
        if (this == &other)
            return (*this);
        std::swap(Value, other.Value);
        return (*this);
    }
    operator T&() noexcept {
        return Value;
    }
    template <typename TNew> explicit operator TNew&() noexcept requires IsExplicitlyConvertible<TNew, T> {
        return static_cast<TNew>(Value);
    }
    operator const T&() const noexcept {
        return Value;
    }
    template <typename TNew> explicit operator const TNew&() const noexcept requires IsExplicitlyConvertible<TNew, T> {
        return static_cast<TNew>(Value);
    }
    Field& operator= (T rhs) noexcept(noexcept(Value = rhs)) {
        Value = rhs;
        return (*this);
    }
#undef ReqConv
};

#define FieldDef(type, name) Field<type> name
#define ReflBody \
    auto GetField(std::string name) noexcept { \
                     \
    }

struct TestClass {
    // Field<int> Int; // Int("meta=default") = 5;
    // Field<List<int>> Lst; // Lst("meta=[pos:[0]]") = 100; Lst("inv=RemoveIf")($1 % 2 == 0);
};

struct Refl {
    Refl() = default;
};

template <typename TElement> class List {
    using uint = unsigned int;
    using Enumerable = TElement;
    using FiniteEnumerable = TElement;
    //using SimpleContinuousEnumerable = void;

protected:
    TElement* RawArr = new TElement[1];
    uint CurrentSize = 0;
    uint CurrentCapacity = 1;

    decltype(auto) AtImpl(uint index) const {
        return RawArr[index];
    }

public:
    List() noexcept : RawArr(new TElement[1]), CurrentSize(0), CurrentCapacity(1) {}
    explicit List(uint size) noexcept : CurrentSize(0), CurrentCapacity(size), RawArr(new TElement[size]) {
        for (uint index = 0; index < size; ++index)
            RawArr[index] = TElement{};
    }
    List(std::initializer_list<TElement> list) noexcept : CurrentSize(list.size()), CurrentCapacity(list.size()), RawArr(new TElement[list.size()]) {
        uint index = 0;
        for (auto iter = list.begin(); iter != list.end(); ++iter, ++index)
            RawArr[index] = (*iter);
    }
    List(const List<TElement>& rhs) noexcept {
        (*this) = rhs;
    }
    List(List<TElement>&& rhs) noexcept {
        (*this) = rhs;
    }
    List(uint size, TElement element) noexcept : List(size) {
        for (uint index = 0; index < CurrentSize; ++index)
            RawArr[index] = element;
    }
    List(uint size, TElement* arr) {
        if (arr == RawArr)
            return;
        CurrentSize = size;
        CurrentCapacity = size;
        delete[] RawArr;
        RawArr = arr;
    }
    auto& operator= (const List<TElement>& rhs) noexcept {
        if (this == &rhs) return *this;
        if (rhs.Size() < CurrentCapacity) {
            CurrentSize = rhs.CurrentSize;
            for (uint index = 0; index < CurrentSize; ++index)
                RawArr[index] = rhs[index];
            return *this;
        }

        Clear();
        auto* newArr = new TElement[rhs.Size()];
        for (uint index = 0; index < rhs.CurrentSize; ++index)
            newArr[index] = rhs[index];
        RawArr = newArr;
        CurrentSize = rhs.CurrentSize;
        CurrentCapacity = rhs.CurrentSize;
        return *this;
    }
    auto& operator= (List<TElement>&& rhs) noexcept {
        if (this == &rhs) return *this;
        std::swap(this->CurrentCapacity, rhs.CurrentCapacity);
        std::swap(this->CurrentSize, rhs.CurrentSize);
        std::swap(this->RawArr, rhs.RawArr);
        return *this;
    }
    void Reserve(uint howMany) {
        if (howMany <= CurrentCapacity) return;
        auto* newArr = new TElement[howMany];
        for (uint index = 0; index < CurrentSize; ++index)
            newArr[index] = RawArr[index];
        delete[] RawArr;
        RawArr = newArr;
        CurrentCapacity = howMany;
    }
    void Add(const TElement element) {
        if (CurrentCapacity == 0)
            Reserve(8);
        else if (CurrentSize == CurrentCapacity)
            Reserve(2 * CurrentCapacity);

        RawArr[CurrentSize] = element;
        CurrentSize += 1;
    }
    void Add(uint size, const TElement* range) {
        if (range == RawArr) {
            Add(*this);
            return;
        }
        if (CurrentSize + size > CurrentCapacity)
            Reserve(CurrentCapacity + size);
        // after Reserve => range's pointer == RawArray's pointer???
        for (uint i = 0; i < size; ++i) {
            Add(range[i]);
        }
    }
    void Add(List<TElement> range) {
        Add(range.Size(), range.RawArr);
    }
    void Add(std::initializer_list<TElement> range) {
        Add(range.size(), range.begin());
    }
    void AddFront(const TElement& element) { Insert(0, element, false); }
    void AddFront(uint size, const TElement* range) { Insert(0, size, range, false); }
    void AddFront(List<TElement> range) { Insert(0, range, false); }
    void AddFront(std::initializer_list<TElement> range) { Insert(0, range, false); }

    std::tuple<List<TElement>, TElement, List<TElement>> Split(const uint index) {
        return { CurrentSize == 0 || CurrentSize == 1 || index == 0 ?
                    List<TElement> {} :
                    Truncate(0, index - 1),
                 RawArr[index],
                 CurrentSize == 0 || CurrentSize == 1 || index + 1 > CurrentSize - 1 ?
                    List<TElement> {} :
                    Truncate(index + 1, CurrentSize == 0 || CurrentSize == 1 ? CurrentSize : CurrentSize - 1) };
    }
    void Insert(uint index, const TElement& element, bool after = true) {
        Insert(index, {element}, after);
    }
    void Insert(uint index, List<TElement> range, bool after = true) {
        Insert(index, range.Size(), range.RawArr, after);
    }
    void Insert(uint index, std::initializer_list<TElement> range, bool after = true) {
        Insert(index, range.size(), range.begin(), after);
    }
    void Insert(uint index, uint size, const TElement* range, bool after = true) {
        if ((index == CurrentSize - 1 && after) || CurrentSize == 0) {
            Add(size, range);
            return;
        }
        else if (range == RawArr) {
            Insert(index, (*this), after);
            return;
        }

        auto tuple = Split(index);
        List<TElement> first = std::get<0>(tuple), last = std::get<2>(tuple);
        TElement element = std::get<1>(tuple);

        if (after)
            first.Add(element);
        else {
            uint size1 = last.CurrentSize + 1;
            auto* newArr = new TElement[size1];
            newArr[0] = element;
            for (uint i = 1; i <= size1 - 1; ++i)
                newArr[i] = last[i - 1];
            last.Clear();
            last.CurrentSize = size1;
            last.CurrentCapacity = size1;
            last.RawArr = newArr;
        }

        Clear();
        Reserve(CurrentCapacity + size);
        Add(first);
        Add(size, range);
        Add(last);
    }
    static List<TElement> Combine(List<TElement> first, List<TElement> second) {
        List<TElement> final(first.Size() + second.Size());
        final.Add(first);
        final.Add(second);
        return final;
    }
    List<TElement> Truncate(uint begin, uint end) const {
        if (begin > CurrentSize || end > CurrentSize)
            throw std::out_of_range(std::string("The argument \"begin\"(") + std::to_string(begin) + "), or/and the argument \"end\"(" + std::to_string(end) + "), must less than "
                                    "current List's size(" + std::to_string(CurrentSize) + ")");
        if (begin > end)
            throw std::invalid_argument(std::string("The argument \"begin\"(") + std::to_string(begin) + "), must less than the argument \"end\"(" + std::to_string(end) + ")");

        List<TElement> final(end - begin + 1);
        for (auto i = begin; i <= end; ++i)
            final.Add(RawArr[i]);
        return final;
    }
    List<TElement> TruncateLen(uint begin, uint length) const {
        return Truncate(begin, begin + length);
    }

    void prt() {
        if (CurrentSize == 0) {
            std::wcout << "(Empty)" << std::endl;
            return;
        }
        ForEach($$( std::wcout << $1 << " ";));
        std::wcout << std::endl;
    }

    void Remove(uint index) noexcept {
        if (index == CurrentSize - 1) {
            PopBack();
            return;
        }
        auto tuple = Split(index);
        (*this) = Combine(std::get<0>(tuple), std::get<2>(tuple));
    }
    void Remove(uint begin, uint end) noexcept {
        auto tuple1 = Split(begin);
        auto tuple2 = Split(end);
        (*this) = Combine(std::get<0>(tuple1), std::get<2>(tuple2));
    }
    void RemoveRange(uint begin, uint size) noexcept {
        Remove(begin, begin + size);
    }
    void RemoveAll(TElement element) {
        RemoveIf($($1 == element));
    }
    void RemoveFirst() noexcept {
        Remove(0);
    }
    void RemoveFirst(TElement element) {
        RemoveFirstIf($($1 == element));
    }
    template <typename TFuncSignature> void RemoveIf(TFuncSignature func) {
        using namespace Utility;
        for (uint i = 0; i < CurrentSize; ++i) {
            LoopFlag flag{ LoopFlag::None };
            bool found = func(RawArr[i], i, flag) == true;
            if (found) { Remove(i); --i; }
            if (flag == LoopFlag::Break) break;
            if (flag == LoopFlag::Continue) continue;
        }
    }
    template <typename TFuncSignature> void RemoveFirstIf(TFuncSignature func) {
        using namespace Utility;
        RemoveIf($(
            auto ret = func($1, $2, $3);
            if (ret == true)
                $3 = LoopFlag::Break;
            ret
        ));
    }
    template <typename TFuncSignature> void ForEach(TFuncSignature func) {
        using namespace Utility;
        LoopFlag flag = LoopFlag::None;
        for (uint index = 0; index < CurrentSize; ++index) {
            func(RawArr[index], index, flag);
            if (flag == LoopFlag::Break) break;
            if (flag == LoopFlag::Continue) continue;
        }
    }
    int Find(const TElement& element) const {
        for (int i=0; i<CurrentSize; ++i)
            if (RawArr[i] == element)
               return i;
        return -1;
    }
    void Replace(const TElement& elementToFind, const TElement& elementToReplace) {
        ForEach($$(
            if ($1 == elementToFind)
                $1 = elementToReplace;
        ));
    }
    bool Contains(const TElement& element) const {
        return Find(element) != -1;
    }
    ~List() noexcept {
        Clear();
    }
    struct Enumerator : Utility::Enumerator<TElement> {
        int Index = 0;
        List<TElement>* Lst;

        Enumerator() : Index(0), Lst(nullptr), Utility::Enumerator<TElement>() {}
        Enumerator(List<TElement>* lst, int idx) : Lst(lst), Utility::Enumerator<TElement>(&(lst->RawArr[idx])), Index(idx) {}
        Enumerator(const Enumerator& rhs) { (*this) = rhs; }
        Enumerator(Enumerator&& rhs) noexcept : Index(0), Utility::Enumerator<TElement>(nullptr), Lst(nullptr) { (*this) = rhs; }
        Enumerator& operator= (const Enumerator& rhs) {
            if (this == &rhs)
                return (*this);
            this->Lst = rhs.Lst;
            this->Index = rhs.Index;
            this->Current = rhs.Current;
            return (*this);
        }
        Enumerator& operator= (Enumerator&& rhs) noexcept {
            if (this == &rhs)
                return (*this);
            std::swap(this->Lst, rhs.Lst);
            std::swap(this->Index, rhs.Index);
            std::swap(this->Current, rhs.Current);
            return (*this);
        }
        void MoveNext() override {
            ++Index;
            this->Current = &(Lst->RawArr[Index]);
        }
        bool End() override {
            return Index >= Lst->CurrentSize - 1;
        }
        bool operator== (const Enumerator& rhs) const noexcept {
            return this->Index == rhs.Index && this->Current == rhs.Current;
        }
        bool operator!= (const Enumerator& rhs) const noexcept {
            return !((*this) == rhs);
        }
        auto operator* () {
            return std::make_tuple(*(this->Current), Index);
        }
    };

    decltype(auto) begin()  noexcept        { return List<TElement>::Enumerator(this, 0); }
    decltype(auto) end()    noexcept        { return List<TElement>::Enumerator(this, CurrentSize); }
    const auto& begin()     const noexcept  { return List<TElement>::Enumerator(this, 0); }
    const auto& end()       const noexcept  { return List<TElement>::Enumerator(this, CurrentSize); }

    [[nodiscard]] TElement& First() noexcept { return RawArr[0]; }
    [[nodiscard]] TElement& Last()  noexcept { return RawArr[CurrentSize - 1]; }
    [[nodiscard]] const TElement& First() const noexcept { return RawArr[0]; }
    [[nodiscard]] const TElement& Last()  const noexcept { return RawArr[CurrentSize - 1]; }
    [[nodiscard]] const TElement& ConstFirst() const noexcept { return First(); }
    [[nodiscard]] const TElement& ConstLast()  const noexcept { return Last(); }
    [[nodiscard]] const TElement* Data()       const noexcept { return RawArr; }
    [[nodiscard]] TElement* Data()             noexcept { return RawArr; }
    [[nodiscard]] uint Size()            const noexcept { return CurrentSize; }
    [[nodiscard]] bool Empty()           const noexcept { return CurrentSize == 0; }
    [[nodiscard]] uint Capacity()        const noexcept { return CurrentCapacity; }
    [[nodiscard]] auto& operator[] (uint index) noexcept(noexcept(index <= CurrentSize - 1)) {
        return AtImpl(index);
    }
    [[nodiscard]] const auto& operator[] (uint index) const noexcept(noexcept(index <= CurrentSize - 1)){
        return AtImpl(index);
    }
    void ShrinkToFit()   noexcept { CurrentCapacity = CurrentSize; }
    void PopBack()       noexcept { CurrentSize--; }
    void PopFront()      noexcept { Remove(0); }
    bool EndsWith(const TElement& element) const noexcept { return Last() == element; }
    void Swap(uint index1, uint index2) {
        std::swap(RawArr[index1], RawArr[index2]);
    }
    void Clear() {
        delete[] RawArr;
        RawArr = new TElement[1];
        CurrentSize = 0;
        CurrentCapacity = 1;
    }
};


#endif //PROJECT2_List_H
