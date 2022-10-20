#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <stdexcept>
#include <iostream>
#include <utility>
#include <iterator>

class ReserveProxyObj {
public:
    ReserveProxyObj() = default;
    ReserveProxyObj(size_t value) {
        value_ = value;
    }

    size_t GetValue() {
        return value_;
    }

private:
    size_t value_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
    using Ptr = ArrayPtr<Type>;

    SimpleVector() noexcept {
        size_ = 0;
        capacity_ = 0;
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        size_ = capacity_ = size;
        items_ = Ptr(size);
        //std::fill(begin(), end(), 0);
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        size_ = capacity_ = size;
        items_ = Ptr(size);
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_ = capacity_ = init.size();
        items_ = Ptr(size_);
        for(size_t i = 0; i < size_; ++i) {
            items_[i] = std::data(init)[i];
        }
    }

    // Создаёт вектор с резервированной вместительностью
    SimpleVector(ReserveProxyObj capacity) {
        capacity_ = capacity.GetValue();
        size_ = 0;
    }

    //Копирующий конструктор
    SimpleVector(const SimpleVector& other) {
        size_ = other.size_;
        capacity_ = other.capacity_;
        items_ = Ptr(capacity_);
        std::copy(other.begin(), other.end(), begin());
    }
    //Копирующее присваивание
    SimpleVector& operator=(const SimpleVector& rhs) {
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        items_ = Ptr(capacity_);
        std::copy(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()), begin());
        return *this;
    }

    //Перемещающий конструктор
    SimpleVector(SimpleVector&& other) {
        size_ = std::move(other.size_);
        capacity_ = std::move(other.capacity_);
        items_ = Ptr(std::move(other.items_));
        other.size_ = 0;
        other.capacity_ = 0;
        other.items_.Nullify();
    }
    //Перемещающее присваивание
    SimpleVector& operator=(SimpleVector&& rhs) {
        size_ = std::move(rhs.size_);
        capacity_ = std::move(rhs.capacity_);
        items_ = Ptr(std::move(rhs.items_));
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        rhs.items_.Nullify();
        return *this;
    }

    ~SimpleVector() {
        items_.Release();
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0 ? true : false;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_) {
            using namespace std::literals;
            throw std::out_of_range("Index is greater than vector size"s);
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_) {
            using namespace std::literals;
            throw std::out_of_range("Index is greater than vector size"s);
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size <= capacity_) {
            size_ = new_size;
        } else {
            capacity_ = std::max(capacity_*2, new_size);
            Ptr temp = Ptr(capacity_);
            std::move(begin(), end(), temp.Get());
            size_ = new_size;
            items_.Release();
            items_ = temp;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return &items_[size_];
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if(size_ == capacity_) {
            Resize(size_+1);
        } else if(size_ == 0) {
            ++size_;
            items_ = Ptr(1);
        } else {
            ++size_;
        }

        items_[size_-1] = item;
    }

    // Добавляет элемент в конец вектора, move-версия
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        if(size_ == capacity_) {
            Resize(size_+1);
        } else if(size_ == 0) {
            ++size_;
            items_ = Ptr(1);
        } else {
            ++size_;
        }

        items_[size_-1] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        auto dist = std::distance(begin(), const_cast<Iterator>(pos));
        if(capacity_ == 0) {
            Resize(1);
            items_[0] = value;
            return begin();
        } else if(size_ == capacity_) {
            Resize(size_+1);
        } else {
            ++size_;
        }
        std::copy_backward(begin() + dist, end() - 1, end());
        *(begin() + dist) = move(value);
        return begin() + dist;
    }

    Iterator Insert(Iterator pos, Type&& value) {
        auto dist = std::distance(std::make_move_iterator(begin()), std::make_move_iterator(pos));
        if(capacity_ == 0) {
            Resize(1);
            items_[0] = std::move(value);
            return begin();
        } else if(size_ == capacity_) {
            Resize(size_+1);
        } else {
            ++size_;
        }
        std::move_backward(begin() + dist, end() - 1,
                           end());
        *(begin() + dist) = std::move(value);
        return begin() + dist;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        Iterator res = const_cast<Iterator>(pos);
        std::copy(res+1, end(), res);
        --size_;
        return res;
    }

    Iterator Erase(Iterator pos) {
        std::copy(std::make_move_iterator(pos+1), std::make_move_iterator(end()), pos);
        --size_;
        return pos;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        items_.swap(other.items_);
    }

    void Reserve(size_t new_capacity) {
        if(new_capacity > capacity_) {
            Ptr temp = Ptr(capacity_);
            std::copy(begin(), end(), temp.Get());
            items_.Release();
            items_ = temp;
            capacity_ = new_capacity;
        }
    }

private:
    ArrayPtr<Type> items_;
    size_t size_;
    size_t capacity_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if(lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs == rhs) || (lhs < rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs == rhs) || (lhs > rhs);
}