// Battery Managment System (BMS) Software /////////////////////////////////////////
	// Functions cpp File
//
// Created for 2019 Valkyrie by: Jacob Lipina, jrlwd5
//
//
// Libraries ///////////////////////////////////////////////////////////////////////
//
#include "BMS_Software_Main.h"
#include "BMS_Software_Functions.h"
//
// Functions ///////////////////////////////////////////////////////////////////////

void setInputPins()
{
	pinMode(PACK_I_MEAS, 	INPUT);
	pinMode(PACK_V_MEAS, 	INPUT);
	pinMode(LOGIC_V_MEAS, 	INPUT);
	pinMode(TEMP_degC_MEAS, INPUT);
	pinMode(C1_V_MEAS, 		INPUT);
	pinMode(C2_V_MEAS, 		INPUT);
	pinMode(C3_V_MEAS, 		INPUT);
	pinMode(C4_V_MEAS, 		INPUT);
	pinMode(C5_V_MEAS, 		INPUT);
	pinMode(C6_V_MEAS, 		INPUT);
	pinMode(C7_V_MEAS, 		INPUT);
	pinMode(C8_V_MEAS, 		INPUT);

	return;
}

void setOutputPins()
{
	pinMode(PACK_OUT_CTR, 		OUTPUT);
	pinMode(PACK_OUT_IND, 		OUTPUT);
	pinMode(LOGIC_SWITCH_CTR, 	OUTPUT);
	pinMode(BUZZER_CTR, 		OUTPUT);
	pinMode(FAN_1_CTR, 			OUTPUT);
	pinMode(FAN_2_CTR, 			OUTPUT);
	pinMode(FAN_3_CTR, 			OUTPUT);
	pinMode(FAN_4_CTR, 			OUTPUT);
	pinMode(FAN_PWR_IND, 		OUTPUT);
	pinMode(SW_IND, 			OUTPUT);
	pinMode(SW_ERR, 			OUTPUT);

	return;
}

void setOutputStates()
{
	digitalWrite(PACK_OUT_CTR, 		LOW);
	digitalWrite(PACK_OUT_IND, 		LOW);
	digitalWrite(LOGIC_SWITCH_CTR, 	LOW);
	digitalWrite(BUZZER_CTR, 		LOW);
	digitalWrite(FAN_1_CTR, 		LOW);
	digitalWrite(FAN_2_CTR, 		LOW);
	digitalWrite(FAN_3_CTR, 		LOW);
	digitalWrite(FAN_4_CTR, 		LOW);
	digitalWrite(FAN_PWR_IND, 		LOW);
	digitalWrite(SW_IND, 			LOW);
	digitalWrite(SW_ERR, 			LOW);
	
	return;
}

void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current)
{
	int adc_reading;

  	adc_reading = analogRead(PACK_I_MEAS);
	main_current = map(adc_reading, ADC_MIN, ADC_MAX, CURRENT_MIN, CURRENT_MAX);

	return;
}

void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
 	int adc_reading;
 	
 	for(int i = 0; i<RC_BMSBOARD_VMEASmV_DATACOUNT; i++)
 	{
 	  if (i == 0)
 	  {
 	  	adc_reading = analogRead(CELL_MEAS_PINS[i])
 	  	cell_voltage[i] = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX);
 	  }
 	  else
 	  {
 	  	adc_reading = analogRead(CELL_MEAS_PINS[i])
 	  	cell_voltage[i] = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, CELL_VOLTS_MAX);
 	  } //end if
 	  return;
 	} //end for
}

void getOutVoltage(uint16_t &pack_out_voltage);
{
	int adc_reading;

  	adc_reading = analogRead(PACK_V_MEAS);
  	pack_out_voltage = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX);

  	return;
}

void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp)
{
	int adc_reading;

	adc_reading = analogRead(TEMP_degC_MEAS);
	batt_temp = map(adc_reading, ADC_MIN, ADC_MAX, TEMP_MIN, TEMP_MAX);

	return;
}

void setEstop(RC_BMSBOARD_SWESTOPs_DATATYPE data) //??Should data be an array here?
{
	if(data == 0)
	{
		digitalWrite(PACK_OUT_CTR, LOW);
		digitalWrite(PACK_OUT_IND, LOW);
		
		notifyEstop();

		digitalWrite(LOGIC_SWITCH_CTR, HIGH); //BMS suicide
	}
	else
	{
		digitalWrite(PACK_OUT_CTR, LOW);
		digitalWrite(PACK_OUT_IND, LOW);

		notifyReboot();

		delay(data);

		digitalWrite(PACK_OUT_CTR, HIGH);
		digitalWrite(PACK_OUT_IND, HIGH);
	} //end if
	return;
}

void setFans(RC_BMSBOARD_FANEN_DATATYPE data) //make sure command turning fans on does not get overridden by the temp being too low.
{
	if(data == RC_BMSBOARD_FANEN_ENABLED) //data == 1
	{
		digitalWrite(FAN_1_CTR, HIGH);
		digitalWrite(FAN_2_CTR, HIGH);
		digitalWrite(FAN_3_CTR, HIGH);
		digitalWrite(FAN_4_CTR, HIGH);
		digitalWrite(FAN_PWR_IND, HIGH);
	} //end if
	if(data == RC_BMSBOARD_FANEN_DISABLED) //data == 0
	{
		digitalWrite(FAN_1_CTR, LOW);
		digitalWrite(FAN_2_CTR, LOW);
		digitalWrite(FAN_3_CTR, LOW);
		digitalWrite(FAN_4_CTR, LOW);
		digitalWrite(FAN_PWR_IND, LOW);
	} //end if
	return;
}

void notifyEstop() //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(2000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(1000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(500);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(100);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}

void notifyReboot() //Buzzer sound: beeeeeeeeeep beeep beeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(1000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}

void notifyOverCurrent() //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(3000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}

void notifyUnderVoltage() //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250)

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(2000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}

void notifyLowVoltage() //Buzzer Sound: beeep beeep beeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	Delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	Delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}