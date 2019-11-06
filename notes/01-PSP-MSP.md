# PendSVC切换任务时，PSP和MSP的使用

The Cortex-M3 auto-saves half of the processor context on any exception, and restores same on return from exception. So. in PendSVC's ISR, for saving content, only saving of R4-R11 is required and fixing up the stack pointers.

When the processor takes an exception, unless the exception is a tail-chained or a late-arriving exception, the processor pushes information onto the **current stack**. This operation is referred as stacking and the structure of eight data words is referred as stack frame. The stack frame contains the following information (in order) :
1. PSR
2. Return address (PC+4，下一条待执行的指令) 
3. LR (R14)
4. R12
5. R3
6. R2
7. R1
8. R0


在发生异常时，自动保存到的内容会保存在当前的栈中。异常发生前使用的是PSP，那么保存到PSP中；如果是MSP就保存到MSP中。

而异常服务例程总是使用的是MSP。

下面是一个例子，两个任务之间手动的切换。执行任务代码时，使用的是PSP。我们看看，PSP和MSP是如何使用的。

 ```c
typedef struct TaskCtrlBlock
{
    unsigned int *StackPtr;  //Stack Pointer
    :
    :
}TCB;
void TaskOne(void *param)
{
    while (1){
        FlagOne = 1;
        Delay(1000);
        FlagOne = 0;
        Delay(1000);     
        TaskSched();
    }
}
void TaskTwo(void *param)
{
    while (1){
        FlagTwo = 1;
        Delay(1000);
        FlagTwo = 0;
        Delay(1000); 
        TaskSched();
    }
}

void TaskSched(void)
{
    if (CurrentTCBPtr == TaskTable[0]){
        NextTCBPtr = TaskTable[1];
    }else{
        NextTCBPtr = TaskTable[0];
    }
    TaskSwitch();
}

void TaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET; //trigger the PendSVC
}

__asm void PendSV_Handler(void)
{
    IMPORT  CurrentTCBPtr
    IMPORT  NextTCBPtr
    
    MRS     R0, PSP //在TaskRunFirst函数中，设置了PSP为0
    CBZ     R0, PendSVHandler_nosave
    /*
     如果R0 == 0，则说明接下来运行的是第一个任务，之前没有任务在运行，
     所以不需要保存上文，只需要恢复第一个待运行的任务
    */
    STMDB   R0!, {R4-R11}
    LDR     R1, =CurrentTCBPtr
    LDR     R1, [R1]
    STR     R0, [R1] //TCBPtr->StackPtr = R0

PendSVHandler_nosave    
    LDR     R0, =CurrentTCBPtr
    LDR     R1, =NextTCBPtr      
    LDR     R2, [R1]                    
    STR     R2, [R0] //CurrentTCBPtr = NextTCBPtr 

    PUSH    {R14} // just for test
    POP     {R14} //just for test
    
    LDR     R0, [R2]
    LDMIA   R0!, {R4-R11}               
    MSR     PSP, R0  //Load PSP with new process SP    
    ORR     LR, LR, #0x04 //Ensure exception return uses process stack
    BX      LR //Exception return will restore remaining context
}

 ```

*注：上面的PendSVC异常服务例程中，我们并没有关中断，这是不对的。但是作为一个软件仿真演示工程，我们只会出现PendSVC一种异常。*

用keil软件仿真，结果截图：

![usage-psp-msp](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/usage-psp-msp.jpg)

**其中**：

- 窗口1是在执行 `MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;` 语句之前的寄存器状态，执行了该语句后，寄存器的状态就变为了窗口3。我们会发现，处理器已经从Thead模式变为了Handler模式。
- 自动保存的**PSR**、**返回地址**、**LR**、**R12**、**R3**、**R2**、**R1**、**R0** 是保存前PSP栈中的。
- 触发PendSVC异常的指令在 0x0000 0392 处，而保存的返回地址正好是 0x0000 0394 指向了下条待执行的指令。0x0000 0394 处的指令会是在任务切换回来后执行的第一条指令。
- 窗口1中的R0和R1的数值和压入栈的数值不一样，是因为 0x0000 0392 这条触发PendSVC的指令执行前，R0和R1的内容有改动。



为了验证，PendSVC异常服务程序执行时，使用的是MSP，我在PendSVC异常服务中人为的添加了出入栈的操作。如下，发现确实是使用的MSP。

可能会有人疑问，我们在PendSVC异常服务中，也有将R4-R11入栈，但是并没有发现MSP的值有变化。

就是因为异常服务使用的是MSP，和任务代码使用的不是一个栈，所以我们不能直接使用 `PUSH` 指令去保存R4-R11。我们是使用的 `STMDB` `LDMIA` 指令来将 R4-R11 存放到PSP的栈中，并且还需要手动调整栈顶指针。

其次，我们保存完上文，恢复完下文的R4-R11后，也用了 `ORR LR, LR, #0x04`  ，确保PendSVC异常从PSP返回，因为我们之前自动保存的内容也在PSP中。

![usage-psp-msp-2](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/usage-psp-msp-2.jpg)





