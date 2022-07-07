# M031BSP_PWM_measureCurrent
 M031BSP_PWM_measureCurrent

update @ 2022/07/07

1. check define : USE_TIMER_PWM or USE_PWM_ONLY to use 2 method to measure current

	- USE_TIMER_PWM : enable TIMER and PB1 , PWM0_CH4 , turn ON/OFF PWM per 10 sec
	
	- USE_PWM_ONLY : enable PB1 , PWM0_CH4 , at 50% duty 

2. environment setup , refer to Current measurement.xlsx


