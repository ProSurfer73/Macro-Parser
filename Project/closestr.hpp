#ifndef CLOSESTR_HPP
#define CLOSESTR_HPP

#include <string>
#include "config.hpp"

/** \brief check if two strings are NEARLY equal.
 *
 * \param original the first string.
 * \param current the second string.
 * \return true if one looks like the other, false if they don't correspond at all.
 */
bool isRoughlyEqualTo(const std::string& original, const std::string& current);

#endif // CLOSESTR_HPP
