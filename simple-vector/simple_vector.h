#pragma once

#include "array_ptr.h"

#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <utility>

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
    : cap_(capacity_to_reserve) {}

    size_t cap_ = 0;

};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
    
    //Создаёт вектор вместимостью capacity
    explicit SimpleVector(ReserveProxyObj capacity) {
        Reserve(capacity.cap_);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) 
    : items_(size), size_(size), capacity_(size) {
        // Напишите тело конструктора самостоятельно
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) 
    : items_(size), size_(size), capacity_(size) {
        std::fill_n(begin(), size, value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
    : items_(init.size()), size_(init.size()), capacity_(init.size()) {
        //size_t i = 0;
        //for (const auto& element : init) {
        //    items_[i] = element;
        //    ++i;
        //}
        std::copy(init.begin(), init.end(), begin());
    }

    //Конструктор копирования
    SimpleVector(const SimpleVector& other) 
    : items_(other.GetSize()), size_(other.GetSize()), capacity_(other.capacity_) {
        std::copy(other.cbegin(), other.cend(), begin());
    }

    //Конструктор перемещения
    SimpleVector(SimpleVector&& other) 
    : items_(std::move(other.GetSize())), size_(std::move(other.GetSize())), capacity_(std::move(other.capacity_)) {
        std::move(other.begin(), other.end(), begin());
        other.size_ = 0;
    }
    
    //Деструктор
    ~SimpleVector() {
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
        if (size_ == 0) {
            return true;
        }
        return false;
    }

    //Оператор присваивания
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector copy_rhs(rhs);
            swap(copy_rhs);
        }
        return *this;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (size_ <= index) {
            throw std::out_of_range("aaa");
        }
        else {
            return items_[index];
        }
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (size_ <= index) {
            throw std::out_of_range("aaa");
        }
        else {
            return items_[index];
        }
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            for (auto i = size_; i < new_size; ++i){
                items_[i] = Type{};
            } 
            size_ = new_size;
        }
        if (new_size > capacity_) {   
            SimpleVector new_vector(std::max(new_size, 2 * capacity_));
            std::move(begin(), end(), new_vector.begin());
            items_.swap(new_vector.items_);
            for (auto i = size_; i < new_size; ++i){
                items_[i] = Type{};
            } 
            size_ = new_size;
            capacity_ = std::max(new_size, 2 * capacity_);
        }
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            items_[size_] = item;
            size_++;
        }
        else {
            Resize(size_ + 1);
            items_[size_ - 1] = item;
        }
    }

    // Добавляет элемент в конец вектора(перемещением)
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            items_[size_] = std::move(item);
            size_++;
        }
        else {
            Resize(size_ + 1);
            items_[size_ - 1] = std::move(item);
        }
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (size_ < capacity_) {
            auto shift = (size_t(pos) - size_t(begin())) / sizeof(Type);
            Iterator it = begin() + shift;
            std::copy(it, end(), it + 1);
            *it = value;
            size_++;
            return it;
        }
        else {
            auto shift = (size_t(pos) - size_t(begin())) / sizeof(Type);
            Resize(size_ + 1);
            Iterator it = begin() + shift;
            std::copy(it, end(), it + 1);
            *it = value;
            return it;
        }
    }

    // Вставляет значение value в позицию pos(перемещением).
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= cbegin() && pos <= cend());
        if (size_ < capacity_) {
            auto shift = (size_t(pos) - size_t(begin())) / sizeof(Type);
            Iterator it = begin() + shift;
            std::move(it, end(), it + 1);
            *it = std::move(value);
            size_++;
            return it;
        }
        else {
            auto shift = (size_t(pos) - size_t(begin())) / sizeof(Type);
            Resize(size_ + 1);
            Iterator it = begin() + shift;
            std::move(it, end(), it + 1);
            *it = std::move(value);
            return it;
        }
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size_ != 0) {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos >= cbegin() && pos < cend());
        auto shift = (size_t(pos) - size_t(begin())) / sizeof(Type);
        Iterator it = begin() + shift;
        std::move(it + 1, end(), it);
        size_--;
        return it;
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    //Резервирует память 
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            SimpleVector new_vector(new_capacity);
            std::copy(begin(), end(), new_vector.begin());
            items_.swap(new_vector.items_);
            capacity_ = new_capacity;
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
        return begin() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return const_cast<Type*>(items_.Get());
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return const_cast<Type*>(items_.Get() + size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return const_cast<Type*>(items_.Get());
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return const_cast<Type*>(items_.Get() + size_);
    }

private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (std::equal(lhs.begin(), lhs.end(), rhs.begin()) && lhs.GetSize() == rhs.GetSize());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs > rhs || lhs == rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}