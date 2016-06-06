// Copyright 2010, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef DESIGN_PATTERN_SINGLETON_HPP_
#define DESIGN_PATTERN_SINGLETON_HPP_

#include <sys/types.h>
#include <iostream>

#include <boost/thread.hpp>

namespace DesignPattern
{

enum CallOnceState
{
    ONCE_INIT = 0,
    ONCE_DONE = 1,
};

#define IZENE_ONCE_INIT { 0, 0 }

struct once_t
{
    volatile int state;
    volatile int counter;
};

// 
const size_t kMaxFinalizersSize = 256;

size_t g_finalizers_size = 0;
boost::mutex *g_mutex = NULL;
once_t g_mutex_once = IZENE_ONCE_INIT;
once_t g_singleton_finalize_once = IZENE_ONCE_INIT;

void InitSingletonMutex()
{
    g_mutex = new boost::mutex;
}


// TODO(taku):
// Linux doesn't provide InterlockedCompareExchange-like function.
// we have to double-check it
inline int InterlockedCompareExchange(volatile int *target,
                                      int new_value,
                                      int old_value)
{
#if defined(__GNUC__) && \
        (__GNUC__ > 4 ||(__GNUC__ == 4 && __GNUC_MINOR__ >= 1)) && \
        !defined(__arm__)
        // Use GCC's extention (Note: ARM GCC doesn't have this function.)
        // http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
        return __sync_val_compare_and_swap(target, old_value, new_value);
#else
#ifdef __i486__
        // Use cmpxchgl: http://0xcc.net/blog/archives/000128.html
        int result;
        asm volatile ("lock; cmpxchgl %1, %2"
                : "=a" (result)
                : "r" (new_value), "m" (*(target), "0" (old_value))
                : "memory", "cc");
        return result;
#else // __i486__
    // TODO(yusukes): Write a thread-safe implementation for ChromeOS for ARM.
    //
    // not thread safe
    if(*target == old_value)
    {
        *target = new_value;
        return old_value;
    }
    return *target;
#endif // __i486__
#endif // __GNUC__
}


// portable re-implementation of pthread_once
void CallOnce(once_t *once, void (*func)())
{
    if(once == NULL || func == NULL)
    {
        return;
    }

    if(once->state != ONCE_INIT)
    {
        return;
    }

    // change the counter in atomic
    if(0 == InterlockedCompareExchange(&(once->counter), 1, 0))
    {
        // call func
        (*func)();
        // change the status to be ONCE_DONE in atomic
        // Maybe wo won't use it , but in order to make memory barrier,
        // we use InterlockedCompareExchange just in case.
        InterlockedCompareExchange(&(once->state), ONCE_DONE, ONCE_INIT);
    }
    else
    {
        while(once->state == ONCE_INIT)
        {
        } // busy loop
    }
}

// reset once_t
void ResetOnce(once_t *once)
{
    InterlockedCompareExchange(&(once->state), ONCE_INIT, ONCE_DONE);
    InterlockedCompareExchange(&(once->counter), 0, 1);
}


// 
void DeleteSingleton();

class SingletonFinalizer
{
public:
    typedef void (*FinalizerFunc)();

    // Do not call this method directly.
    // use Singleton<Typename> instead.
    static void AddFinalizer(FinalizerFunc func);

    // Call Finalize() if you want to finalize
    // all instances created by Sigleton.
    //
    static void Finalize()
    {
        CallOnce(&g_singleton_finalize_once, &DeleteSingleton);
    }
};

SingletonFinalizer::FinalizerFunc g_finalizers[kMaxFinalizersSize];
void SingletonFinalizer::AddFinalizer(FinalizerFunc func)
{
    // When g_finalizers_size is equal to kMaxFinalizersSize,
    // SingletonFinalizer::Finalize is called already.
    // CHECK_LT(g_finalizers_size, kMaxFinalizersSize);

    // TODO(taku):
    // we only allow up  to kMaxFinalizersSize fucntions here.
    CallOnce(&g_mutex_once, &InitSingletonMutex);

    {
        boost::mutex::scoped_lock l(*g_mutex);
        g_finalizers[g_finalizers_size++] = func;
    }
}

void DeleteSingleton()
{
    // delete instances in reverse order.
    for(int i = static_cast<int>(g_finalizers_size) - 1; i >= 0; --i)
    {
        (*g_finalizers[i])();
    }
    // set kMaxFinalizersSize so that AddFinalizers cannot be called twice.
    g_finalizers_size = kMaxFinalizersSize;
    delete g_mutex;
    g_mutex = NULL;
}
// Thread-safe Singleton class.
// Usage:
//
// class Foo {}
//
// Foo *instance = Singleton<Foo>::get();
template <typename T>
class Singleton
{
public:
    static T *get()
    {
        CallOnce(&once_, &Singleton<T>::Init);
        return instance_;
    }

private:
    static void Init()
    {
        SingletonFinalizer::AddFinalizer(&Singleton<T>::Delete);
        instance_ = new T;
    }

    static void Delete()
    {
        delete instance_;
        instance_ = NULL;
        ResetOnce(&once_);
    }

    static once_t once_;
    static T *instance_;
};

template <typename T>
once_t Singleton<T>::once_ = IZENE_ONCE_INIT;

template <typename T>
T* Singleton<T>::instance_ = NULL;



} // namespace DesignPattern


#endif  // DesignPattern/singleton.hpp
