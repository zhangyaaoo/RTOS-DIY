#ifndef __OS_CFG_H_
#define __OS_CFG_H_

#ifdef __OS_CFG_C_
#define OS_CFG_EXT
#else
#define OS_CFG_EXT extern
#endif

#define PRIO_NUM_MAX        32u

#define TASK_RUN_TIMESLICE  10u

#define IDLETASK_STACK_SIZE 1024u

#endif /* __OS_CFG_H_ */
