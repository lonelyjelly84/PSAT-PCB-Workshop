/*
 
555timer_pcb_workshop_apss.c*
Created: 31/10/2025 10:20:33 pm
Author : navini*/

#include <math.h>
#include <stdint.h>
#include <avr/io.h>


#define VREF 5.0
#define ADCMAX 1023.0
#define BETA 3380.0
#define RESISTOR 10000
#define RTHERM 47000 //due to change 
#define VCC 5.0 
#define T0_K 298.15 //25 deg in kelvin 


volatile uint16_t counter = 0;

void ISR(INT0_vect) {
	counter++;
}

// initialise interrupt
void int_init(void){
	EICRA |= (1 << ISC00)|(1 << ISC01); //setting the interrupt to detect a rising edge 
	EIMSK |= (1 << INT0); //enable the INT0 interrupt 
}

void adc_init(void){
	ADMUX |= (1 << REFS0); //selecting the VREF as reference 
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enabling ADC and selecting pre-scaler 64 

}

int16_t adc_read(void){
	ADMUX &= ~((1 << MUX0) | (1 << MUX1)); //Selecting ADC Channel 0 
	ADCSRA |= (1 << ADSC); //Starting conversion 

	while(ADCSRA & (1 <<ADSC)){
		//this while loop waits for conversion to finish
	}
	//return ADC;
	return ((uint16_t)ADCH << 8) | ADCL;// this is another method of returning the ADC value!

}

//converts the ADC voltage into a temperature 
int16_t tempConversion(int16_t adcValue){
	//converting adc count to voltage 
	double VoltageTemp = (adcValue / ADCMAX) * VCC; 
	
	//calculate thermistor resistance 
	double thermistorResistance = RESISTOR * (VCC - VoltageTemp)/ VoltageTemp;

	//apply beta formula 
	double invertedTemp = (1.0/T0_K) + (1.0/ BETA) * log(thermistorResistance/RTHERM);
	double tempInKelvin = 1/invertedTemp; 
	
	//convert to Celsius 
	double tempInCelsius = tempInKelvin - 273.15;

	return (int16_t)tempInCelsius;
}

int main(void)
{
	int_init();

	//sei();
	/* Replace with your application code */
	while (1)
	{
	tempConversion(adc_read);
	}
}
