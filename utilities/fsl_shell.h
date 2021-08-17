/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FSL_SHELL_H__
#define __FSL_SHELL_H__

/*!
 * @addtogroup SHELL
 * @{
 */

#include "fsl_common.h"
#include "serial_manager.h"
#include "generic_list.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Whether use non-blocking mode. */
#ifndef SHELL_NON_BLOCKING_MODE
#define SHELL_NON_BLOCKING_MODE SERIAL_MANAGER_NON_BLOCKING_MODE
#endif

/*! @brief Macro to set on/off auto-complete feature. */
#define SHELL_AUTO_COMPLETE (1U)

/*! @brief Macro to set console buffer size. */
#define SHELL_BUFFER_SIZE (128U)

/*! @brief Macro to set sprintf buffer size. */
#define SHELL_SPRINTF_BUFFER_SIZE (128U)

/*! @brief Macro to set maximum arguments in command. */
#define SHELL_MAX_ARGS (8U)

/*! @brief Macro to set maximum count of history commands. */
#define SHELL_HISTORY_COUNT (3U)

/*! @brief Macro to bypass arguments check */
#define SHELL_IGNORE_PARAMETER_COUNT (0xFF)

/*! @brief The handle size of the shell module. */
#define SHELL_HANDLE_SIZE (sizeof(shell_context_handle_t))

/*! @brief Macro to determine whether use common task. */
#ifndef SHELL_USE_COMMON_TASK
#define SHELL_USE_COMMON_TASK (1U)
#endif

/*! @brief Macro to set shell task priority. */
#ifndef SHELL_TASK_PRIORITY
#define SHELL_TASK_PRIORITY (2U)
#endif

/*! @brief Macro to set shell task stack size. */
#ifndef SHELL_TASK_STACK_SIZE
#define SHELL_TASK_STACK_SIZE (1000U)
#endif

/*! @brief A type for the handle special key. */
typedef enum _fun_key_status
{
    kSHELL_Normal   = 0U, /*!< Normal key */
    kSHELL_Special  = 1U, /*!< Special key */
    kSHELL_Function = 2U, /*!< Function key */
} fun_key_status_t;

/*! @brief Data structure for Shell environment. */
typedef struct _shell_context_handle
{
    list_label_t commandContextListHead; /*!< Command shellContextHandle list queue head */
    serial_handle_t serialHandle;        /*!< Serial manager handle */
    uint8_t
        serialWriteHandleBuffer[SERIAL_MANAGER_WRITE_HANDLE_SIZE];   /*!< The buffer for serial manager write handle */
    serial_write_handle_t serialWriteHandle;                         /*!< The serial manager write handle */
    uint8_t serialReadHandleBuffer[SERIAL_MANAGER_READ_HANDLE_SIZE]; /*!< The buffer for serial manager read handle */
    serial_read_handle_t serialReadHandle;                           /*!< The serial manager read handle */
    char *prompt;                                                    /*!< Prompt string */
#if (defined(SHELL_NON_BLOCKING_MODE) && (SHELL_NON_BLOCKING_MODE > 0U))

#if defined(OSA_USED)

#if (defined(SHELL_USE_COMMON_TASK) && (SHELL_USE_COMMON_TASK > 0U))
    common_task_message_t commontaskMsg; /*!< Message for common task */
#else
    uint8_t event[OSA_EVENT_HANDLE_SIZE]; /*!< Event instance */
    uint8_t taskId[OSA_TASK_HANDLE_SIZE]; /*!< Task handle */
#endif

#endif

#endif
    char line[SHELL_BUFFER_SIZE];                          /*!< Consult buffer */
    char hist_buf[SHELL_HISTORY_COUNT][SHELL_BUFFER_SIZE]; /*!< History buffer*/
    char printBuffer[SHELL_SPRINTF_BUFFER_SIZE];           /*!< Buffer for print */
    uint32_t printLength;                                  /*!< All length has been printed */
    uint16_t hist_current;                                 /*!< Current history command in hist buff*/
    uint16_t hist_count;                                   /*!< Total history command in hist buff*/
    enum _fun_key_status stat;                             /*!< Special key status */
    uint8_t cmd_num;                                       /*!< Number of user commands */
    uint8_t l_pos;                                         /*!< Total line position */
    uint8_t c_pos;                                         /*!< Current line position */
    volatile uint8_t notificationPost;                     /*!< The serial manager notification is post */
    uint8_t exit;                                          /*!< Exit Flag*/
    uint8_t printBusy;                                     /*!< Print is busy */
} shell_context_handle_t;

/*! @brief Shell status */
typedef enum _shell_status
{
    kStatus_SHELL_Success               = kStatus_Success,                    /*!< Success */
    kStatus_SHELL_Error                 = MAKE_STATUS(kStatusGroup_SHELL, 1), /*!< Failed */
    kStatus_SHELL_OpenWriteHandleFailed = MAKE_STATUS(kStatusGroup_SHELL, 2), /*!< Open write handle failed */
    kStatus_SHELL_OpenReadHandleFailed  = MAKE_STATUS(kStatusGroup_SHELL, 3), /*!< Open read handle failed */
} shell_status_t;

/*! @brief The handle of the shell module */
typedef void *shell_handle_t;

/*! @brief User command function prototype. */
typedef shell_status_t (*cmd_function_t)(shell_handle_t shellHandle, int32_t argc, char **argv);

/*! @brief User command data configuration structure. */
typedef struct _shell_command
{
    const char *pcCommand; /*!< The command that is executed.  For example "help".  It must be all lower case. */
    char *pcHelpString;    /*!< String that describes how to use the command.  It should start with the command itself,
                                    and end with "\r\n".  For example "help: Returns a list of all the commands\r\n". */
    const cmd_function_t
        pFuncCallBack; /*!< A pointer to the callback function that returns the output generated by the command. */
    uint8_t cExpectedNumberOfParameters; /*!< Commands expect a fixed number of parameters, which may be zero. */
    list_element_t link;                 /*!< link of the element */
} shell_command_t;

/*!
 * @brief Defines the shell handle
 *
 * This macro is used to define a 4 byte aligned shell handle.
 * Then use "(shell_handle_t)name" to get the shell handle.
 *
 * The macro should be global and could be optional. You could also define shell handle by yourself.
 *
 * This is an example,
 * @code
 * SHELL_HANDLE_DEFINE(shellHandle);
 * @endcode
 *
 * @param name The name string of the shell handle.
 */
#define SHELL_HANDLE_DEFINE(name) uint32_t name[((SHELL_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))]

#if defined(__ICCARM__)
/* disable misra 19.13 */
_Pragma("diag_suppress=Pm120")
#endif
/*!
 * @brief Defines the shell command structure
 *
 * This macro is used to define the shell command structure #shell_command_t.
 * And then uses the macro SHELL_COMMAND to get the command structure pointer.
 * The macro should not be used in any function.
 *
 * This is a example,
 * @code
 * SHELL_COMMAND_DEFINE(exit, "\r\n\"exit\": Exit program\r\n", SHELL_ExitCommand, 0);
 * SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(exit));
 * @endcode
 *
 * @param command The command string of the command. The double quotes do not need. Such as exit for "exit",
 *                help for "Help", read for "read".
 * @param descriptor The description of the command is used for showing the command usage when "help" is typing.
 * @param callback The callback of the command is used to handle the command line when the input command is matched.
 * @param paramCount The max parameter count of the current command.
 */
#define SHELL_COMMAND_DEFINE(command, descriptor, callback, paramCount) \
                                                                        \
    shell_command_t g_shellCommand##command = {                         \
        (#command), (descriptor), (callback), (paramCount), {0},        \
    }

/*!
 * @brief Gets the shell command pointer
 *
 * This macro is used to get the shell command pointer. The macro should not be used before the macro
 * SHELL_COMMAND_DEFINE is used.
 *
 * @param command The command string of the command. The double quotes do not need. Such as exit for "exit",
 *                help for "Help", read for "read".
 */
#define SHELL_COMMAND(command) &g_shellCommand##command

#if defined(__ICCARM__)
    _Pragma("diag_default=Pm120")
#endif

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
        extern "C"
{
#endif /* _cplusplus */

    /*!
     * @name Shell functional operation
     * @{
     */

    /*!
     * @brief Initializes the shell module
     *
     * This function must be called before calling all other Shell functions.
     * Call operation the Shell commands with user-defined settings.
     * The example below shows how to set up the Shell and
     * how to call the SHELL_Init function by passing in these parameters.
     * This is an example.
     * @code
     *   static SHELL_HANDLE_DEFINE(s_shellHandle);
     *   SHELL_Init((shell_handle_t)s_shellHandle, (serial_handle_t)s_serialHandle, "Test@SHELL>");
     * @endcode
     * @param shellHandle Pointer to point to a memory space of size #SHELL_HANDLE_SIZE allocated by the caller.
     * The handle should be 4 byte aligned, because unaligned access doesn't be supported on some devices.
     * You can define the handle in the following two ways:
     * #SHELL_HANDLE_DEFINE(shellHandle);
     * or
     * uint32_t shellHandle[((SHELL_HANDLE_SIZE + sizeof(uint32_t) - 1U) / sizeof(uint32_t))];
     * @param serialHandle The serial manager module handle pointer.
     * @param prompt  The string prompt pointer of Shell. Only the global variable can be passed.
     * @retval kStatus_SHELL_Success The shell initialization succeed.
     * @retval kStatus_SHELL_Error An error occurred when the shell is initialized.
     * @retval kStatus_SHELL_OpenWriteHandleFailed Open the write handle failed.
     * @retval kStatus_SHELL_OpenReadHandleFailed Open the read handle failed.
     */
    shell_status_t SHELL_Init(shell_handle_t shellHandle, serial_handle_t serialHandle, char *prompt);

    /*!
     * @brief Registers the shell command
     *
     * This function is used to register the shell command by using the command configuration shell_command_config_t.
     * This is a example,
     * @code
     * SHELL_COMMAND_DEFINE(exit, "\r\n\"exit\": Exit program\r\n", SHELL_ExitCommand, 0);
     * SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(exit));
     * @endcode
     * @param shellHandle The shell module handle pointer.
     * @param shellCommand  The command element.
     * @retval kStatus_SHELL_Success Successfully register the command.
     * @retval kStatus_SHELL_Error An error occurred.
     */
    shell_status_t SHELL_RegisterCommand(shell_handle_t shellHandle, shell_command_t * shellCommand);

    /*!
     * @brief Unregisters the shell command
     *
     * This function is used to unregister the shell command.
     *
     * @param shellCommand The command element.
     * @retval kStatus_SHELL_Success Successfully unregister the command.
     */
    shell_status_t SHELL_UnregisterCommand(shell_command_t * shellCommand);

    /*!
     * @brief Sends data to the shell output stream.
     *
     * This function is used to send data to the shell output stream.
     *
     * @param shellHandle The shell module handle pointer.
     * @param buffer Start address of the data to write.
     * @param length Length of the data to write.
     * @retval kStatus_SHELL_Success Successfully send data.
     * @retval kStatus_SHELL_Error An error occurred.
     */
    shell_status_t SHELL_Write(shell_handle_t shellHandle, char *buffer, uint32_t length);

    /*!
     * @brief Writes formatted output to the shell output stream.
     *
     * Call this function to write a formatted output to the shell output stream.
     *
     * @param shellHandle The shell module handle pointer.
     *
     * @param   formatString Format string.
     * @return  Returns the number of characters printed or a negative value if an error occurs.
     */
    int SHELL_Printf(shell_handle_t shellHandle, const char *formatString, ...);

/*!
 * @brief The task function for Shell.
 * The task function for Shell; The function should be polled by upper layer.
 * This function does not return until Shell command exit was called.
 *
 * @param shellHandle The shell module handle pointer.
 */
#if !(defined(SHELL_NON_BLOCKING_MODE) && (SHELL_NON_BLOCKING_MODE > 0U))
    void SHELL_Task(shell_handle_t shellHandle);
#endif

    /* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __FSL_SHELL_H__ */
