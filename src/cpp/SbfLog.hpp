#pragma once

#include "sbfLog.h"
#include "sbfLogFile.h"

namespace neueda
{

class SbfLog {
public:
    /*!
        \brief Constructs a log that outputs to stdout.
        \return A SbfLog object.
     */
    SbfLog ();

    /*!
        \brief Destructor that deletes the private log handler.
        \return None.
     */
    virtual ~SbfLog ();

    /*!
        \brief Logs a message with a given level into the log.
        \param level the log level.
        \param fmt the formatting string.
        \param ... the parameters for the formatting string.
        \return None.
    */
    virtual void log (sbfLogLevel level, const char* fmt, ...);

    /*!
        \brief Logs a chunk of binary data with a given level into the log.
        \param level the log level.
        \param buf the data to be logged.
        \param len the size of the data in bytes.
        \return None.
    */
    virtual void logData (sbfLogLevel level, void* buf, size_t len);

    /*!
        \brief Returns a handle to the private C log struct.
        \return Pointer to a struct sbfLogImpl.
     */
    virtual sbfLog getHandle ();

    /*!
        \brief Returns the log's level.
        \return the log's level.
    */
    virtual sbfLogLevel getLevel();

    /*!
        \brief Sets the log's level
        \param[in] level The log's level.
        \return None.
    */
    virtual void setLevel(sbfLogLevel level);

    /*!
        \brief Sets the log's level from a string.
        \param[in] level Null terminated string representation of the log level.
    */
    virtual bool setLevelFromString (const char* s);

    /*!
       \brief Return a string representation of an enum log level.
       \param[in] level The log level to conver to string.
       \return Null terminated string representation of the pass in level.
     */
    virtual const char* levelToString (sbfLogLevel level);

protected:
    sbfLog mValue;
};

}
