#ifndef CONFLICT_HPP
#define CONFLICT_HPP

#include <string>
#include <vector>

/// Main abstract class

class ConflictSolver
{
public:
    virtual void resolve(const std::string& curexpr, const std::vector<std::string>& possibilities, std::vector<int>& choices) = 0;
};

/// Concrete classes, fully implemented

class SolverFirst : public ConflictSolver
{
public:
    virtual void resolve(const std::string& curexpr, const std::vector<std::string>& possibilities, std::vector<int>& choices);
};

class SolverEvery : public ConflictSolver
{
public:
    virtual void resolve(const std::string& curexpr, const std::vector<std::string>& possibilities, std::vector<int>& choices);
};

class SolverFirst defaultSolver;

#endif // CONFLICT_HPP
