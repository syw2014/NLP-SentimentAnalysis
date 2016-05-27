///
/// @file PropSharedLock.h
/// @brief base class to lock/unlock property data by multiple readers
/// @author Jun Jiang <jun.jiang@izenesoft.com>
/// @date Created 2012-06-25
///
///
//  Tips:
//      When to use the Reader-writer lock, you just need to declare the objec
//      of PropSharedLock like this,and put it where needs to be protected.
//      ScopedReadLock mutex(mutex_, isLock); // reader lock, a shared lock
//      ScopedWriteLock mutex(mutex_, isLock); // writer lock, an unique lock
//      @ mutex_, is the member variable of ProSharedLock
//      @ isLock, is a local bool variable 


#ifndef PROP_SHARED_LOCK_H
#define PROP_SHARED_LOCK_H

#include <boost/thread.hpp>

class PropSharedLock
{
public:
    typedef boost::shared_mutex MutexType;
    typedef boost::shared_lock<MutexType> ScopedReadLock;
    typedef boost::unique_lock<MutexType> ScopedWriteLock;

    class ScopedReadBoolLock;
    class ScopedWriteBoolLock;

    virtual ~PropSharedLock() {}

    MutexType& getMutex() const { return mutex_; }

    void lockShared() const { mutex_.lock_shared(); }

    void unlockShared() const { mutex_.unlock_shared(); }

protected:
    mutable MutexType mutex_;
};

class PropSharedLock::ScopedReadBoolLock
{
public:
    ScopedReadBoolLock(PropSharedLock::MutexType& mutex, bool isLock)
        : lock_(mutex, boost::defer_lock)
    {
        if (isLock)
        {
            lock_.lock();
        }
    }

private:
    PropSharedLock::ScopedReadLock lock_;
};

class PropSharedLock::ScopedWriteBoolLock
{
public:
    ScopedWriteBoolLock(PropSharedLock::MutexType& mutex, bool isLock)
        : lock_(mutex, boost::defer_lock)
    {
        if (isLock)
        {
            lock_.lock();
        }
    }

private:
    PropSharedLock::ScopedWriteLock lock_;
};

#endif // PROP_SHARED_LOCK_H
