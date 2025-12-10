#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <iostream>
#include <cmath>
#include <string>

// Глобальные переменные для отслеживания тестов
int passedTests = 0;
int failedTests = 0;

// Функция для сравнения с вещественными числами
inline double abs_diff(double a, double b)
{
    return std::abs(a - b);
}

// Функция для тестирования условий
inline void assertEqual(bool condition, const std::string &testName)
{
    if (condition)
    {
        passedTests++;
        std::cout << "  ✓ " << testName << std::endl;
    }
    else
    {
        failedTests++;
        std::cout << "  ✗ FAILED: " << testName << std::endl;
    }
}

#endif // TEST_UTILS_H