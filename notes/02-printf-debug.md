为了加入串口打印调试。需要对工程进行调整。

1. 更换芯片型号，选择ARMCM3时，无法添加串口的代码，因为arm核不提供外设的解决方案。这里我们选择的是F103RC。

![chip-f103rc](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/chip-f103rc.jpg)

工程添加上USART等模块：

![proj-component](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/proj-component.jpg)

更换芯片型号后，还需要配置下面的参数，不然调试时会遇到，初始化执行SystemInit函数时，停在等待外部晶振ready的地方。

![debug-parms](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/debug-parms.jpg)

2. 工程配置，当然还有代码相关的修改：参考[这里](https://blog.csdn.net/wangzhimin_0702/article/details/80256024)。

![use-micro-lib](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/use-micro-lib.jpg)

3. 设置虚拟串口软件和keil

![vpsd-setting](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/vpsd-setting.jpg)

开始debug调试后，输入下面的命令：

![debug-cmd](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/debug-cmd.jpg)

因为在代码中使用的是USART2，所以上述的两条命令就是让虚拟端口COM1和USART2的输入输出对应起来。S**2**IN  S**2**OUT 。为了避免每次debug时都要重新输入命令，可以像下面这样配置：

![debug-ini](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/debug-ini.jpg)

那么，我们就可用串口软件（如：Xshell）连接COM2，在COM2中能看到USART2的打印。

![print-log](https://raw.githubusercontent.com/zhangyaaoo/ImageBed/master/rtos/print-log.jpg)