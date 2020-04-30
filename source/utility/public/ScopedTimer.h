#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <string>

/*
    A helper class that times the execution of functions

    @tparam TargetCallback      The target function that will be timed
    @tparam TimeReportCallback  A callback to pass the elapsed time out through
*/
template<typename TargetCallback, typename TimeReportCallback>
class ScopedTimer final
{
public:

    /*
        Constructs a new ScopedTimer, which executes the function to time

        @param isVerbose    Whether or not to output verbose messages
        @param startMessage The verbose message to print at the beginning of execution
        @param endMessage   The verbose message to print at the end of execution
        @param callable     The method to time the execution of
        @param report       The method used to report the elapsed execution time 
    */
    ScopedTimer(bool isVerbose, const std::string& startMessage, const std::string& endMessage,
                TargetCallback callable, TimeReportCallback report)
        : m_endMessage(endMessage), m_bIsVerbose(isVerbose), m_reportCallback(report)
    {
        if (m_bIsVerbose)
            std::cout << startMessage;

        m_start = std::chrono::high_resolution_clock::now();

        callable();
    }

    /*
        Destructs a ScopedTimer, which stops the timer
    */
    ~ScopedTimer() noexcept
    {
        auto end = std::chrono::high_resolution_clock::now();
        long long delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();

        if (m_bIsVerbose)
            std::cout << m_endMessage << delta << "ms\n\n";

        m_reportCallback(delta);
    }

private:

    std::string m_endMessage;
    bool m_bIsVerbose = false;
    std::function<void(long long)> m_reportCallback;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

};