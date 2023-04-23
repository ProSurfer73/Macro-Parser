#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>

/// This file contains useful features related to vector manipulation.


/** \brief remove duplicate values from a vector.
 *
 * \param vec the vector we want to remove duplicates values from.
 */
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

/** \brief remove a specific value from a vector.
 *
 * \param v the vector, from which we are going to remove the specific value provided.
 * \param value the value we want to remove from the vector.
 */
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

/** \brief emplace a value (if it does not already exist in the vector).
 *
 * \param v the vector to which we want to add our value.
 * \param macroName the value we want to add to our vector.
 */
template<typename T>
void emplaceOnce(std::vector<T>& v, const T& macroName)
{
    if(v.empty() || std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.push_back(macroName);
    }
}

/** \brief emplace a value (if it does not already exist in the vector).
 *
 * \param v the vector to which we want to add our value.
 * \param macroName the value we want to add to our vector.
 */
template<typename T>
void emplaceOnce(std::vector<T>& v, T&& macroName)
{
    if(v.empty() || std::find(v.begin(), v.end(), macroName)==v.end())
    {
        v.emplace_back( std::move(macroName) );
    }
}



#endif // VECTOR_HPP
