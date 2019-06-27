#ifndef FILESERV_SINGLETON_H
#define FILESERV_SINGLETON_H

#include <memory>

template<typename T>
class SingLeton
{
public: 
    using PtrType = std::shared_ptr<T>;
    static PtrType& instance()
    {
        if(ptr_ == nullptr)
            ptr_.reset(new T());
        return ptr_;
    }

private: 
    static PtrType ptr_;
    SingLeton(){};
    ~SingLeton(){}

    SingLeton(const SingLeton&){}
    SingLeton& operator=(const SingLeton&){}

};

template<typename T>
typename SingLeton<T>::PtrType SingLeton<T>::ptr_(nullptr);



#endif // !FILESERV_SINGLETON_H