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



#endif // VECTOR_HPP
