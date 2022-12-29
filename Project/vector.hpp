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

template<typename T>
void emplaceOnce(std::vector<T>& v, const T& macroName)
{
    if(v.empty() || std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.push_back(macroName);
    }
}

template<typename T>
void emplaceOnce(std::vector<T>& v, T&& macroName)
{
    if(v.empty() || std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.emplace_back( std::move(macroName) );
    }
}



#endif // VECTOR_HPP
