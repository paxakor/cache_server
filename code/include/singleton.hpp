// Copyright 2016, Pavel Korozevtsev.

#pragma once

namespace pkr {

template<typename T>
class Singleton {
public:
    static T& get_inctance();
    static void destroy_inctance();

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() { }

protected:
    static T* instance;
};

template<typename T>
T* Singleton<T>::instance = nullptr;

template<typename T>
T& Singleton<T>::get_inctance() {
    if(!instance) {
        Singleton<T>::instance = new T();
    }
    return *instance;
}

template<typename T>
void Singleton<T>::destroy_inctance() {
    delete Singleton<T>::instance;
    Singleton<T>::instance = nullptr;
}

}  // namespace pkr
