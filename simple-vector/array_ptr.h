#pragma once

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if(size == 0) {
            raw_ptr_ = nullptr;
        } else {
            raw_ptr_ = new Type[size]{};
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    //Копирующий конструктор
    ArrayPtr(const ArrayPtr& other) = delete;
    //Копирующее присваивание
    ArrayPtr& operator=(const ArrayPtr& rhs) = delete;

    //Перемещающий конструктор
    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::move(other.raw_ptr_);
    }
    //Перемещающее присваивание
    ArrayPtr& operator=(ArrayPtr&& rhs) {
        raw_ptr_ = std::move(rhs.raw_ptr_);
        return *this;
    }

    ~ArrayPtr() {
    }

    ArrayPtr& operator++() {
        return *(raw_ptr_ + 1);
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    void Release() noexcept {
        //Type* temp = raw_ptr_;
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
        //return temp;
    }

    void Nullify() {
        raw_ptr_ = nullptr;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        // Реализуйте операцию самостоятельно
        return *(raw_ptr_ + index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        // Реализуйте операцию самостоятельно
        return *(raw_ptr_ + index);
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        // Заглушка. Реализуйте операцию самостоятельно
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        // Реализуйте метод самостоятельно
        Type* temp = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = temp;
    }

private:
    Type* raw_ptr_ = nullptr;
};