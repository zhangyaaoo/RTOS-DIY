#ifndef __OS_TYPE_H_
#define __OS_TYPE_H_

#ifdef __OS_TYPE_C_
#define OS_TYPE_EXT
#else
#define OS_TYPE_EXT extern
#endif

#include <stdint.h>

typedef     uint32_t        TaskPrio_t;

typedef enum
{
    TASK_STATE_RUN,
    TASK_STATE_READY,
    TASK_STATE_DELAYED,
}TaskState_t;
#endif /* __OS_TYPE_H_ */
