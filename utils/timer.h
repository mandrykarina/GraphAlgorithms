#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>
#include <string>

/**
 * Простой класс для измерения времени выполнения
 */
class Timer
{
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning = false;

public:
    /**
     * Начать отсчет времени
     */
    void start()
    {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }

    /**
     * Остановить отсчет времени
     */
    void stop()
    {
        endTime = std::chrono::high_resolution_clock::now();
        isRunning = false;
    }

    /**
     * Получить время в миллисекундах
     */
    double elapsedMs() const
    {
        if (isRunning)
        {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(now - startTime).count();
        }
        return std::chrono::duration<double, std::milli>(endTime - startTime).count();
    }

    /**
     * Получить время в микросекундах
     */
    double elapsedUs() const
    {
        if (isRunning)
        {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::micro>(now - startTime).count();
        }
        return std::chrono::duration<double, std::micro>(endTime - startTime).count();
    }

    /**
     * Получить время в секундах
     */
    double elapsedSec() const
    {
        return elapsedMs() / 1000.0;
    }

    /**
     * Вывести время в удобном формате
     */
    void printElapsed(const std::string &label = "")
    {
        double ms = elapsedMs();

        if (!label.empty())
        {
            std::cout << label << ": ";
        }

        if (ms < 1.0)
        {
            std::cout << elapsedUs() << " µs" << std::endl;
        }
        else if (ms < 1000.0)
        {
            std::cout << ms << " ms" << std::endl;
        }
        else
        {
            std::cout << elapsedSec() << " sec" << std::endl;
        }
    }

    /**
     * Сбросить таймер
     */
    void reset()
    {
        isRunning = false;
    }
};

#endif // TIMER_H