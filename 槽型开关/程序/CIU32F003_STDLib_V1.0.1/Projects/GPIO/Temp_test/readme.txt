槽型开关 CIU32F003 完整测试程序

一、已经保留和重新整理的功能
1. RCH最高主频48MHz，HCLK和PCLK均不分频。
2. ADC时钟16MHz，最短3周期采样，单次转换约1us。
3. TIM1硬件触发ADC，定时器计数单位为1us。
4. ADC转换完成中断读取数据，主循环不轮询ADC。
5. NO、NC、LED均可通过宏选择有效电平。
6. NO、NC、LED均保留独立底层控制函数。
7. 输出通道支持运行时整体使能和关闭。
8. LED支持运行时使能、关闭和亮灭控制。
9. ADC阈值与回差判定。
10. 输出通道使能时，NO和NC始终为相反状态。
11. 增加9点抗脉冲滤波，去掉最高2点和最低2点，中间5点平均后再判断阈值。

二、原理图引脚
PA0  -- IO_OUTPUT_NC
PB0  -- IO_OUTPUT_NO
PA1  -- IO_LED_RED
PB1  -- ADC_IN0

三、主要配置宏（Include/common.h）

功能总开关：
NO_OUTPUT_FUNCTION_ENABLE
NC_OUTPUT_FUNCTION_ENABLE
LED_FUNCTION_ENABLE

上电运行状态：
OUTPUT_CHANNELS_POWER_ON_ENABLE
LED_POWER_ON_ENABLE
DETECTION_POWER_ON_STATE

有效电平：
NO_OUTPUT_ACTIVE_LEVEL
NC_OUTPUT_ACTIVE_LEVEL
LED_ACTIVE_LEVEL

ADC采样：
ADC_SAMPLE_PERIOD_US          默认10us，即100kS/s

新增阈值功能：
ADC_THRESHOLD_CONTROL_ENABLE  阈值自动切换总开关
ADC_SWITCH_THRESHOLD          默认2048
ADC_SWITCH_HYSTERESIS         默认50

抗脉冲滤波：
ADC_PULSE_FILTER_ENABLE       滤波总开关
ADC_FILTER_DEBUG_ENABLE       调试变量更新开关
ADC_FILTER_WINDOW_SIZE        固定9点
ADC_FILTER_TRIM_COUNT         两端各去掉2点

四、ADC阈值与输出关系
ADC > 2048：
NO有效、NC无效、LED亮、g_detection_state=1。

ADC < 1998：
NO无效、NC有效、LED灭、g_detection_state=0。

ADC在1998~2048之间：
保持原状态，避免阈值附近抖动。

五、运行时控制函数
output_channels_enable(1U)：使能NO和NC输出。
output_channels_enable(0U)：关闭两个输出通道，两个通道均输出无效电平。

led_enable(1U)：使能LED跟随检测状态。
led_enable(0U)：强制关闭LED。

detection_state_set(1U)：NO有效、NC无效、LED亮。
detection_state_set(0U)：NO无效、NC有效、LED灭。

no_output_set(1U/0U)：单独控制NO有效/无效。
nc_output_set(1U/0U)：单独控制NC有效/无效。
led_set(1U/0U)：单独控制LED亮/灭。

adc_start()：启动TIM1触发ADC采样。
adc_stop()：停止TIM1和ADC转换。

六、Keil Watch调试变量
g_adc_value：最新12位ADC原始值，可以看到干扰脉冲。
g_adc_filtered_value：去掉高低异常值后的真实电平，阈值判断使用此值。
g_adc_window_min：当前9点窗口最小值。
g_adc_window_max：当前9点窗口最大值。
g_adc_filter_ready：9点窗口已收满时为1。
g_adc_sample_count：成功读取ADC的次数。
g_adc_overrun_count：ADC溢出次数。
g_threshold_switch_count：ADC跨越阈值导致输出切换的次数。
g_detection_state：当前检测状态。
g_output_channels_enabled：输出通道运行时使能状态。
g_led_enabled：LED运行时使能状态。

七、新增功能代码位置
ADC阈值处理函数：slot_switch_adc_process()。
互补输出应用函数：detection_outputs_apply()。
ADC中断入口：ADC_COMP_IRQHandler()。

注意：当输出通道整体使能时，NO和NC保持互补；主动调用output_channels_enable(0U)后，
两个通道都会进入无效状态，这是“关闭输出通道”的定义。
