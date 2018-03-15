/*
  ==============================================================================

    Logger.h

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

namespace reverb
{

    /**
    * Class used for managing app log messages.
    */
    class Logger
    {
    public:
        //==============================================================================
        /**
        * @brief Construct the logger singleton object
        *
        * Opens log file in the system-appropriate log directory, limiting its size if necessary.
        * Use Logger::getInstance() to get a reference to the singleton.
        */
        Logger(const std::string& logName,
               const std::string& welcomeMsg = "Beginning log");

        ~Logger() = default;

        Logger(const Logger&) = delete;
        void operator=(const Logger&) = delete;


        //==============================================================================
        enum class Level 
        {
            Fatal = 0,
            Error = 1,
            Warning = 2,
            Info = 3,
            App = 4
        };

        /**
         * @brief Set logger verbosity level
         *
         * Determines how severe a message must be before being printed. When verbosity is
         * low, only very important messages are logged.
         *
         * @param [in] logLevel New verbosity level
         */
        void setVerbosity(Level logLevel);


        //==============================================================================
        /**
         * @brief Print message to log file
         *
         * If current verbosity level permits it, message is formatted and printed
         * to app log file.
         * 
         * @param [in] logLevel Severity of message to print
         * @param [in] msg      Message to print
         */
        void print(Level logLevel, const std::string& msg);

        /**
         * @brief Print message to console/UI and to log file
         *
         * All messages get printed to the console. Logged messages are filtered by
         * severity depending on current verbosity setting (see Logger::print()).
         * 
         * @param [in] logLevel Severity of message to print
         * @param [in] msg      Message to print
         */
        void dualPrint(Level logLevel, const std::string& msg);

    private:
        //==============================================================================
        std::unique_ptr<juce::FileLogger> log;
        Level verbosity;

        std::mutex mutex;
    };

    extern Logger logger;

}