1、该例程为uCOSII2.91使用例程。

2、使用说明
   (1)工程文件路径：例程目录\uCOSII2.91\MDK-ARM\Project.uvproj。
   (2)请使用MDK 4.0以上版本打开,MDK版本过低会导致无法识别工程。
   (3)下载调试工具为ULINK。
   (4)请将串口线(直连串口线)插在板子COM1口上,并打开超级终端或串口助手，配置波特率115200，8位，一个停止位，无校验位。
   (5)打开uCOSView-V310G.exe,在软件界面下点击setup，选者波特率(Baudrate)115200,端口COM1 or COM2,
      在我的电脑上,点击右键属性，硬件-设备管理器-端口(COM和LPT),Prolific USB-to-Serial Bridge属性,
	  端口设置-高级，设置COM端口号为COM1 or COM2。	  
   (6)HEX文件下载到板子后,LED灯闪烁,uCOSView-V310G.exe软件界面下,显示各任务调度情况。 
 
3、注意事项
   请务必在下载、调试、运行过程中，保持板子上电、ULINK连接并插在电脑上。