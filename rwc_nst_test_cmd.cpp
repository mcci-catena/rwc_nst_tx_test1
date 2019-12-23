/*

Module: rwc_nst_test_cmd.cpp

Function:
    Process the various test commands.

Copyright:
    See accompanying LICENSE file for copyright and license information.

Author:
    Terry Moore, MCCI Corporation   November 2019

*/

#include "rwc_nst_test_cmd.h"

#include "rwc_nst_test.h"
#include "rwc_nst_test_lmiclog.h"
#include <strings.h>

using namespace McciCatena;

/*

Name:   ::cmdTxTest()

Function:
    Command dispatcher for "tx" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdTxTest;

    McciCatena::cCommandStream::CommandStatus cmdTxTest(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "tx" command takes no arguments. It starts a transmit
    test.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.

cCommandStream::CommandStatus cmdTxTest(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {

    if (argc != 1)
        return cCommandStream::CommandStatus::kInvalidParameter;

    if (! gTest.evSendStartTx())
        {
        pThis->printf("busy\n");
        return cCommandStream::CommandStatus::kError;
        }

    return cCommandStream::CommandStatus::kSuccess;
    }

/*

Name:   ::cmdRxTest()

Function:
    Command dispatcher for "rx" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdRxTest;

    McciCatena::cCommandStream::CommandStatus cmdRxTest(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "rx" command takes no arguments. It starts a receive
    test.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.

cCommandStream::CommandStatus cmdRxTest(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {

    if (argc != 1)
        return cCommandStream::CommandStatus::kInvalidParameter;

    if (! gTest.evSendStartRx())
        {
        pThis->printf("busy\n");
        return cCommandStream::CommandStatus::kError;
        }

    return cCommandStream::CommandStatus::kSuccess;
    }

/*

Name:   ::cmdRxWindowTest()

Function:
    Command dispatcher for "rw" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdRxWindowTest;

    McciCatena::cCommandStream::CommandStatus cmdRxWindowTest(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "rw" command takes no arguments. It starts a receive window
    test. The receive window test waits for a rising edge on a specified
    digital line (param RxDigIn), and captures the os_getTime() value.
    It then starts a single receive scheduled at `param RxWindow`, using
    RxSyms and ClockError to simulate the LMIC's window. 

    This process repeats (controlled by param RxCount), and counts of pulses
    and successful receives are accumulated.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.

cCommandStream::CommandStatus cmdRxWindowTest(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {

    if (argc != 1)
        return cCommandStream::CommandStatus::kInvalidParameter;

    if (! gTest.evSendStartRxWindow())
        {
        pThis->printf("busy\n");
        return cCommandStream::CommandStatus::kError;
        }

    return cCommandStream::CommandStatus::kSuccess;
    }

/*

Name:   ::cmdRxCount()

Function:
    Command dispatcher for "count" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdRxCount;

    McciCatena::cCommandStream::CommandStatus cmdRxCount(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "count" command takes no arguments. It prints out the current
    received-packet count.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.

cCommandStream::CommandStatus cmdRxCount(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {

    if (argc != 1)
        return cCommandStream::CommandStatus::kInvalidParameter;

    gTest.evStopTest();

    pThis->printf("RxCount: %u\n", gTest.getRxCount());

    return cCommandStream::CommandStatus::kSuccess;
    }

/*

Name:   ::cmdParam()

Function:
    Command dispatcher for "param" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdParam;

    McciCatena::cCommandStream::CommandStatus cmdParam(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "param" command has three forms:

    1. "param" by itself displays all the parameters
    2. "param x" displays parameter x (only)
    3. "param x v" sets x to v.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.
cCommandStream::CommandStatus cmdParam(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {
    switch (argc)
        {
    default:
        return cCommandStream::CommandStatus::kInvalidParameter;

    case 1:
        for (auto & p : cTest::ParamInfo)
            {
            char buf[64];
            if (gTest.getParam(p.getName(), buf, sizeof(buf)))
                pThis->printf("%s: %s\n", p.getName(), buf);
            }
        break;

    case 2:
        {
        char buf[64];
        if (gTest.getParam(argv[1], buf, sizeof(buf)))
            pThis->printf("%s\n", buf);
        else if (strcasecmp(argv[1], "help") == 0|| argv[1][0] == '?')
            {
            for (auto & p : cTest::ParamInfo)
                {
                pThis->printf("%s: %s\n", p.getName(), p.getHelp());
                }
            }
        else
            return cCommandStream::CommandStatus::kInvalidParameter;
        }
        break;

    case 3:
        {
        if (! gTest.setParam(argv[1], argv[2]))
            return cCommandStream::CommandStatus::kInvalidParameter;
        }
        break;
        }

    return cCommandStream::CommandStatus::kSuccess;
    }

/*

Name:   ::cmdLog()

Function:
    Command dispatcher for "log" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdLog;

    McciCatena::cCommandStream::CommandStatus cmdLog(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "log" command has one form

    1. "log" by itself dumps the log.
    2. "log registers" displays the current radio registers.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.
cCommandStream::CommandStatus cmdLog(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {
    switch (argc)
        {
    default:
        return cCommandStream::CommandStatus::kInvalidParameter;

    case 1:
        eventQueue.printAll();
        return cCommandStream::CommandStatus::kSuccess;

    case 2:
        if (strcasecmp(argv[1], "registers") == 0)
            {
            eventQueue.printAllRegisters();
            pThis->printf("\n");
            return cCommandStream::CommandStatus::kSuccess;
            }
        return cCommandStream::CommandStatus::kInvalidParameter;
        }
    }

/*

Name:   ::cmdQuit()

Function:
    Command dispatcher for "q" command.

Definition:
    McciCatena::cCommandStream::CommandFn cmdQuit;

    McciCatena::cCommandStream::CommandStatus cmdQuit(
        cCommandStream *pThis,
        void *pContext,
        int argc,
        char **argv
        );

Description:
    The "q" command takes no arguments. It just stops the current test.

Returns:
    cCommandStream::CommandStatus::kSuccess if successful.
    Some other value for failure.

*/

// argv[0] is the matched command name.

cCommandStream::CommandStatus cmdQuit(
    cCommandStream *pThis,
    void *pContext,
    int argc,
    char **argv
    )
    {

    if (argc != 1)
        return cCommandStream::CommandStatus::kInvalidParameter;

    gTest.evStopTest();

    return cCommandStream::CommandStatus::kSuccess;
    }
