#ifndef FILESERV_TIMERHEAP_HPP
#define FILESERV_TIMERHEAP_HPP

#include <vector>
#include <cstddef>
#include <functional>

template<typename T>
bool defaultCompare(const T& lhs, const T& rhs)
{
    return lhs > rhs;
}

template<typename T>
class TimerHeap
{
public: 
    using Compare = std::function<bool (const T&, const T&)>;
    TimerHeap(const Compare&& cmp/* = std::bind(&defaultCompare, std::placeholders::_1, std::placeholders::_2)*/):arr_(), compare_(cmp){}
    ~TimerHeap(){}

    void insert(const T value)
    {
        arr_.emplace_back(value);
        insertAdjust(arr_.size() - 1);
    }

    T getMin()
    {
        std::swap(arr_[0], arr_[arr_.size() - 1]);
        T tmp = arr_.back();
        arr_.pop_back();
        removeAdjust(0);
        return tmp;
    }

    T& getTop()
    {
        return arr_[0];
    }

    bool isEmpty()
    {
        return arr_.size() == 0;
    }

    int size()
    {
        return arr_.size();
    }

private: 
    size_t getLeftChild(size_t i)
    {
        return (i << 1) + 1;
    }
    size_t getRightChild(size_t i)
    {
        return (i << 1) + 2;
    }
    size_t getParent(size_t i)
    {
        return (i - 1) >> 1;
    }
    void insertAdjust(size_t pos)
    {
        while(pos != 0)
        {
            size_t parent = getParent(pos);
            if(compare_(arr_[parent], arr_[pos]))
            {
                std::swap(arr_[parent], arr_[pos]);
                pos = parent;
            }else
            {
                break;
            }
            
        }
    }
    void removeAdjust(size_t pos)
    {
        size_t l = getLeftChild(pos);
        while(l < size())
        {
            if(l + 1 < arr_.size() && compare_(arr_[l], arr_[l + 1]))
                ++l;
            if(compare_(arr_[l], arr_[pos]))
                break;
            else
            {
                std::swap(arr_[l], arr_[pos]);
                pos = l;
                l = getLeftChild(pos);
            }       
        }
    }

private: 
    std::vector<T> arr_;
    Compare compare_;
};


#endif // !FILESERV_TIMERHEAP_HPP