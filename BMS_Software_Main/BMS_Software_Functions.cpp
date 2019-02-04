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
}//end fntcn

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
}//end fntcn

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
}//end fntcn

void getMainCurrent(RC_BMSBOARD_MAINIMEASmA_DATATYPE &main_current)
{
	int adc_reading;

  	adc_reading = analogRead(PACK_I_MEAS);
	main_current = map(adc_reading, ADC_MIN, ADC_MAX, CURRENT_MIN, CURRENT_MAX);

	return;
}//end fntcn

void getCellVoltage(RC_BMSBOARD_VMEASmV_DATATYPE cell_voltage[RC_BMSBOARD_VMEASmV_DATACOUNT])
{
 	int adc_reading;
 	
 	for(int i = 0; i<RC_BMSBOARD_VMEASmV_DATACOUNT; i++)
 	{
 	  if (i == RC_BMSBOARD_VMEASmV_PACKENTRY)
 	  {
 	  	adc_reading = analogRead(CELL_MEAS_PINS[i]);
 	  	cell_voltage[i] = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX);
 	  }
 	  else
 	  {
 	  	adc_reading = analogRead(CELL_MEAS_PINS[i]);
 	  	cell_voltage[i] = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, CELL_VOLTS_MAX);
 	  } //end if
 	  return;
 	} //end for
}//end fntcn

void getOutVoltage(int &pack_out_voltage)
{
	int adc_reading;

  	adc_reading = analogRead(PACK_V_MEAS);
  	pack_out_voltage = map(adc_reading, ADC_MIN, ADC_MAX, VOLTS_MIN, PACK_VOLTS_MAX);

  	return;
}//end fntcn

void getBattTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE &batt_temp)
{
	int adc_reading;

	adc_reading = analogRead(TEMP_degC_MEAS);
	batt_temp = map(adc_reading, ADC_MIN, ADC_MAX, TEMP_MIN, TEMP_MAX);

	return;
}//end fntcn

bool singleDebounceCurrent(int bouncing_pin, int overcurrent_threshold)
{
  int adc_threshhold = map(overcurrent_threshold, CURRENT_MIN, CURRENT_MAX, ADC_MIN, ADC_MAX);
  
  if(analogRead(bouncing_pin) > adc_threshhold)
  {  
    delay(DEBOUNCE_DELAY);
    
    if(analogRead(bouncing_pin) > adc_threshhold)
    {
       return true;
    }//end if
  }// end if 
  return false;
}//end fntcn

bool singleDebounceVoltage(int bouncing_pin, int undervoltage_threshold, int volts_max, int volts_safety_low)
{
  int adc_threshhold = map(undervoltage_threshold, VOLTS_MIN, volts_max, ADC_MIN, ADC_MAX);
  int adc_safety_min = map(volts_safety_low, VOLTS_MIN, volts_max, ADC_MIN, ADC_MAX); 
  
  if((analogRead(bouncing_pin) < adc_threshhold) && (analogRead(bouncing_pin) > adc_safety_min))
  {  
    delay(DEBOUNCE_DELAY);
    
    if((analogRead(bouncing_pin) < adc_threshhold) && (analogRead(bouncing_pin) > adc_safety_min))
    {
       return true;
    }//end if
  }// end if 
  return false;
}//end fntcn

void checkOverCurrent(RC_BMSBOARD_EVENT_DATATYPE event_report[RC_BMSBOARD_EVENT_DATACOUNT])
{
	if(singleDebounceCurrent(PACK_I_MEAS, OVERCURRENT))
	{
		event_report[RC_BMSBOARD_EVENT_PACKOVERCURRENT] = RC_BMSBOARD_EVENT_OCCURED;
	} //end if

	return;
}//end fntcn

void checkUnderVoltage(RC_BMSBOARD_EVENT_DATATYPE event_report[RC_BMSBOARD_EVENT_DATACOUNT])
{
	for(int i = 0; i < RC_BMSBOARD_VMEASmV_DATACOUNT; i++)
	{
		if(i == RC_BMSBOARD_VMEASmV_PACKENTRY)
		{
			if(singleDebounceVoltage(CELL_MEAS_PINS[RC_BMSBOARD_VMEASmV_PACKENTRY], PACK_UNDERVOLTAGE, PACK_VOLTS_MAX, PACK_SAFETY_LOW))
			{
				event_report[RC_BMSBOARD_EVENT_PACKUNDERVOLT] = RC_BMSBOARD_EVENT_OCCURED;
			} //end if
		}
		else
		{
			if(singleDebounceVoltage(CELL_MEAS_PINS[i], CELL_UNDERVOLTAGE, CELL_VOLTS_MAX, CELL_SAFETY_LOW))
			{
				event_report[i] = RC_BMSBOARD_EVENT_OCCURED;
			} //end if
		} //end if
	} //end for
	return;
}//end fntcn

void reactOverCurrent(RC_BMSBOARD_EVENT_DATATYPE event_report[RC_BMSBOARD_EVENT_DATACOUNT], bool &overcurrent_state, float &timeofovercurrent)
{
	if(event_report[RC_BMSBOARD_EVENT_PACKOVERCURRENT] == RC_BMSBOARD_EVENT_OCCURED)
	{
		digitalWrite(PACK_OUT_CTR, LOW);
		digitalWrite(PACK_OUT_IND, LOW);

		if(overcurrent_state == false)
		{
			overcurrent_state = true;
			timeofovercurrent = millis();	
		}

		notifyOverCurrent();
	}
	if((overcurrent_state == true) && (millis() >= (timeofovercurrent + OVERCURRENT_DELAY)))
	{
		digitalWrite(PACK_OUT_CTR, HIGH);
		digitalWrite(PACK_OUT_IND, HIGH);
	}
	if((overcurrent_state == true) && (millis() >= (timeofovercurrent + OVERCURRENT_DELAY)))
	{
		overcurrent_state = false;

		digitalWrite(LOGIC_SWITCH_CTR, HIGH); //BMS Suicide
	}

	return;
}//end fntcn

void reactUnderVoltage(RC_BMSBOARD_EVENT_DATATYPE event_report[RC_BMSBOARD_EVENT_DATACOUNT])
{
	for(int i = 0; i < (RC_BMSBOARD_EVENT_DATACOUNT - 1); i++)
	{
		if(event_report[i] == RC_BMSBOARD_EVENT_OCCURED)
		{
			digitalWrite(PACK_OUT_CTR, LOW);
			digitalWrite(PACK_OUT_IND, LOW);

			notifyUnderVoltage();

			digitalWrite(LOGIC_SWITCH_CTR, HIGH); //BMS Suicide
		} //end if
	} //end for
	return;
}//end fntcn

void reactOverTemp(RC_BMSBOARD_TEMPMEASmDEGC_DATATYPE batt_temp)
{
	if(batt_temp > TEMP_THRESHOLD)
	{
		overtemp_state = true;
		digitalWrite(FAN_1_CTR, HIGH);
		digitalWrite(FAN_2_CTR, HIGH);
		digitalWrite(FAN_3_CTR, HIGH);
		digitalWrite(FAN_4_CTR, HIGH);
		digitalWrite(FAN_PWR_IND, HIGH);
	}
	else
	{
		overtemp_state = false;
	}

	return;
}//end fntcn

void setEstop(RC_BMSBOARD_SWESTOPs_DATATYPE data)
{
	if(data == 0)
	{
		digitalWrite(PACK_OUT_CTR, LOW);
		digitalWrite(PACK_OUT_IND, LOW);
		
		notifyEstop();
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
}//end fntcn

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
}//end fntcn

void notifyEstop() //Buzzer sound: beeeeeeeeeeeeeeeeeeeep beeeeeeeeeep beeeeep beeep bep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(2000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(1000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(500);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(100);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}//end fntcn

void notifyReboot() //Buzzer sound: beeeeeeeeeep beeep beeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(1000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}//end fntcn

void notifyOverCurrent() //Buzzer Sound: beeeeeeeeeeeeeeeeeeeeeeeeeeeeeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(3000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}//end fntcn

void notifyUnderVoltage() //Buzzer Sound: beeep beeep beeep beeep beeeeeeeeeeeeeeeeeeeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(2000);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}//end fntcn

void notifyLowVoltage() //Buzzer Sound: beeep beeep beeep
{
	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);
	delay(250);

	digitalWrite(BUZZER_CTR, HIGH);
	digitalWrite(SW_ERR, HIGH);
	delay(250);
	digitalWrite(BUZZER_CTR, LOW);
	digitalWrite(SW_ERR, LOW);

	return;
}//end fntcn