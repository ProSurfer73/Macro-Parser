#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>


// This file contains useful features related to vector manipulation

template<typename T>
void removeDuplicates(std::vector<T>& vec)
{
    for(unsigned i=0; i<vec.size(); ++i)
    {
        for(unsigned j=i+1; j<vec.size(); ++j)
        {
            if(vec[i] == vec[j]){
                vec.erase(vec.begin()+i);
                i--;
                break;
            }
        }
    }
}


template<typename T>
bool emplaceOnce(std::vector<T>& v, const T& value)
{
    if(v.empty()){
        v.push_back(value);
        return true;
    }

    if(std::find(v.begin(), v.end(), value)==v.end())
    {
        v.push_back(value);
        return true;
    }

    return false;
}

template<typename T>
bool emplaceOnce(std::vector<T>& v, const T&& value)
{
    if(v.empty()){
        v.push_back(std::move(value));
        return true;
    }

    if(std::find(v.begin(), v.end(), value)==v.end())
    {
        v.push_back(std::move(value));
        return true;
    }

    return false;
}

template<typename T>
void removeFromVector(std::vector<T>& v, const T& value)
{
    for(auto it=v.begin(); it!=v.end();)
    {
        if(*it == value)
            it = v.erase(it);
        else
            ++it;
    }
}



#endif // VECTOR_HPP
