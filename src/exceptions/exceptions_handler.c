//
// Created by adrianoii on 23/07/2021.
//

#include "exceptions_handler.h"
#include <stdlib.h>
#include "logs.h"

void throw_exception(exception_t exception)
{
    switch (exception)
    {
        case ALLOCATION_FAILED:
            log_with_color_nl(RED, "Failed to allocate more memory!");
            exit(ALLOCATION_FAILED);
        case INVALID_CLI:
            exit(INVALID_CLI);
        case INVALID_FILE:
            log_with_color_nl(RED, "Invalid file, please provide a valid path!");
            exit(INVALID_FILE);
        case INVALID_FILE_ROLLBACK_CHAR:
            log_with_color_nl(RED, "Error when trying to rollback the file cursor!");
            exit(INVALID_FILE_ROLLBACK_CHAR);
        case LEX_INVALID_BRACKET_COMMENT:
            exit(LEX_INVALID_BRACKET_COMMENT);
        case LEX_INVALID_SLASH_COMMENT:
            exit(LEX_INVALID_SLASH_COMMENT);
        case LEX_INVALID_FLOAT:
            exit(LEX_INVALID_FLOAT);
        case LEX_INVALID_SYMBOL:
            exit(LEX_INVALID_SYMBOL);
        default:
            exit(-1);
    }
}
