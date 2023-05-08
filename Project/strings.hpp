#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <string>

/// useful functions related to string manipulation.

/** \brief remove all space related characters from a string.
 *
 * \param the string for�m which we want to remove all the space related characters.
 */
void clearSpaces(std::string& str);

/** \brief lower all the alpha characters of a given string.
 *
 * \param the string to be modified.
 */
void lowerString(std::string& str);

/** \brief replace a keyword to another inside a string.
 *
 * \param str the main string.
 * \param the initial keyword.
 * \param the final keyword.
 * \return true if the keyword was found and replaced, false otherwise.
 */
bool simpleReplace(std::string& str, const std::string& from, const std::string& to);

/** \brief check if a string has a specific ending.
 *
 * \param the full string.
 * \param the endind.
 * \return true if the full string has the string ending at its very end, false if it does not.
 */
bool hasEnding (std::string const &fullString, std::string const &ending);

/** \brief check if the beginning of a string is equal to another.
 *
 * \param mainStr the big string.
 * \param beginning the beginning string.
 *
 * \return true if the beginning of mainStr is equal to beginning, false otherwise.
 */
bool startsWith(const std::string& mainStr, const std::string& beginning);


#endif // STRINGS_HPP
