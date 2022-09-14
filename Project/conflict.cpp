#include "conflict.hpp"

void SolverFirst::resolve(const std::string& curexpr, const std::vector<std::string>& possibilities, std::vector<int>& choices)
{
    if(!possibilities.empty()){
        choices.clear();
        choices.push_back(0);
    }
}

void SolverEvery::resolve(const std::string& curexpr, const std::vector<std::string>& possibilities, std::vector<int>& choices)
{
    if(!possibilities.empty()){
        choices.clear();
        choices.push_back(0);
    }
}

