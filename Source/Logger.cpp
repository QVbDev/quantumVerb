/*
  ==============================================================================

     Logger.cpp

  ==============================================================================
*/

#include "Logger.h"

namespace reverb
{

    //==============================================================================
    Logger::Logger(const std::string& logName,
                   const std::string& welcomeMsg)
        : log(juce::FileLogger::createDefaultAppLogger(JucePlugin_Name, logName, welcomeMsg)),
            verbosity(Level::Warning)
    {
    }

    //==============================================================================
    void Logger::setVerbosity(Level logLevel)
    {
        verbosity = logLevel;
    }

    //==============================================================================
    void Logger::print(Level logLevel, const std::string& msg)
    {
        if (logLevel > verbosity)
        {
            return;
        }

        // Format message with timestamp and severity
        std::string msgFormatted;

        msgFormatted = juce::Time::getCurrentTime().formatted("%Y-%m-%d %H:%M:%S").toStdString();

        switch (logLevel)
        {
        case Level::Fatal:
            msgFormatted += " [FATAL]: ";
            break;

        case Level::Error:
            msgFormatted += " [ERROR]: ";
            break;

        case Level::Warning:
            msgFormatted += " [WARNING]: ";
            break;

        case Level::Info:
            msgFormatted += " [INFO]: ";
            break;

        case Level::App:
        default:
            msgFormatted += " [APP]: ";
            break;
        }

        msgFormatted += msg;

        // Print formatted message to log file (thread-safe)
        std::lock_guard<std::mutex> lock(mutex);
        log->logMessage(msgFormatted);
    }

    void Logger::dualPrint(Level logLevel, const std::string& msg)
    {
        // TODO: Print to somewhere in UI as well so user knows about these errors
        std::cerr << msg;
        print(logLevel, msg);
    }


    //==============================================================================
    /**
     * Global logger object
     */
    Logger logger(JucePlugin_Name ".log");

}