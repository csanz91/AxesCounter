#include "definition.h"
#include <LiquidCrystal.h>

//Set up the LCD
LiquidCrystal lcd(0, 1, 7, 4, 8, 9);

///////Constants///////
const unsigned char end = 255;
#define N_PLACES  5	//Number of places
#define N_TRANS   7	//Number of transitions
#define IN 5			//Number of inputs

///////States///////
#define Ini 0
#define Wait 1
#define Start_timer 2
#define End_timer 3
#define Send_pulses 4

#define Pulse_input 2

static unsigned char input[IN];			//The active inputs will be stored here
const unsigned int Maxtime=15624;					//1 second
unsigned char axes_counter=0, Pulses_sended=0, axes=0;

///////Petri's Net transitions///////
const char T0_IN[]={Ini,end};
const char T0_OUT[]={Wait,end};
const char T1_IN[]={Wait,end};
const char T1_OUT[]={Start_timer,end};
const char T2_IN[]={Start_timer,end};
const char T2_OUT[]={Start_timer,end};
const char T3_IN[]={Start_timer,end};
const char T3_OUT[]={End_timer,end};
const char T4_IN[]={End_timer,end};
const char T4_OUT[]={Send_pulses,end};
const char T5_IN[]={End_timer,end};
const char T5_OUT[]={Ini,end};
const char T6_IN[]={Send_pulses,end};
const char T6_OUT[]={Ini,end};


///////Petri's Net actions and conditions///////
void action0 (){
}
char condition0(unsigned char input[]){
	char condition0=0;
	if (1==1) {
		condition0=1;
	}
	return condition0;
}

void action1 (){
	TCNT1=0;
	// Set CS10 and CS12 bits for 1024 prescaler:
	TCCR1B |=  (1 << CS12) | (1 << CS10);
	axes_counter++;
}
char condition1(unsigned char input[]){
	char condition1=0;
	if (!input[Pulse_input]) {
		condition1=1;
	}
	return condition1;
}

void action2 (){
}
char condition2(unsigned char input[]){
	char condition2=0;
	if (input[Pulse_input]) {
		condition2=1;
	}
	return condition2;
}

void action3 (){
	TCCR1B =0;
}
char condition3(unsigned char input[]){
	char condition3=0;
	if (0){//TCNT1>Maxtime){
		condition3=1;
	}
	return condition3;
}

void action4 (){
}
char condition4(unsigned char input[]){
	char condition4=0;
	if (axes_counter>3){
		 condition4=1;
	}
	return condition4;
}

void action5 (){
	axes_counter=0;
}
char condition5(unsigned char input[]){
	char condition5=0;
	if (axes_counter<4){
		condition5=1;
	}
	return condition5;
}

void action6 (){
	axes_counter=0;
}
char condition6(unsigned char input[]){
	char condition6;
	if (Pulses_sended)
	{	
		condition6=1;
		Pulses_sended=0;
	}
	return condition6;
}

const T_TRAN Trans [N_TRANS] ={
	{T0_IN, T0_OUT, action0, condition0},
	{T1_IN, T1_OUT, action1, condition1},
	{T2_IN, T2_OUT, action2, condition2},
	{T3_IN, T3_OUT, action3, condition3},
	{T4_IN, T4_OUT, action4, condition4},
	{T5_IN, T5_OUT, action5, condition5},
	{T6_IN, T6_OUT, action6, condition6}
};

///////Memories to keep the states' marks///////
static unsigned char M[N_PLACES];
static unsigned char sig_M[N_PLACES];

///////The initial mark will be in the first state///////
void initialitation_M(){

		M[0]=1;
		sig_M[0]=1;

}

///////Functions to allow the Petri's Net flow///////
char sensitized(T_TRAN Tran,const unsigned char M[]){
	int p;
	char sensib = 1;
	
	p=0;
	while ((Tran.input_places[p] !=end) && sensib==1){
		if (M[Tran.input_places[p]] < 1){
			
			sensib = 0;
		}
		p++;
	}
	return sensib;
}

void remove_marks(T_TRAN Tran, unsigned char M[]){
	int p;
	
	p=0;
	while((Tran.input_places[p] != end)){
		M[Tran.input_places[p]]--;
		p++;
	}
}

void set_marks(T_TRAN Tran, unsigned char M[]){
	int p;
	
	p=0;
	while((Tran.input_places[p] != end)){
		M[Tran.output_places[p]] ++;
		p++;
	}
}


void generate_outputs(unsigned char M[]){

        for(int x=0;x<N_PLACES;x++)
        {
          if (M[x])
          {
            axes=x;
          }
        }
		if (M[Send_pulses]) Pulses_sended=1;

}


void read_inputs(unsigned char input[]){
	
    if(digitalRead(Pulse_input)) input[Pulse_input]=1;
    else input[Pulse_input]=0;

}


void print_lcd()
{
	char buffer[15];
	
	//First line
	lcd.setCursor(0, 0);
	sprintf(buffer,"Axes detctd:%d", axes_counter);
	lcd.print(buffer);
	
	//Second line
	lcd.setCursor(0, 1);
	sprintf(buffer,"Axes sended:%d", axes);
	lcd.print(buffer);
	
}

void setup()
{

        pinMode(Pulse_input, INPUT);

}

void loop()
{
	initialitation_M();
	while(1)
	{
		memcpy (M, sig_M, sizeof(M));
		generate_outputs(M);
		read_inputs (input);
		
		//Repeat the same Petri Net for all the trains' sensors
		for(int t=0; t<=(N_TRANS-1); t++){
			if(sensitized(Trans[t],M) && (*Trans[t].condition) (input)){
				remove_marks(Trans[t], sig_M);
				remove_marks(Trans[t],M);
				(*Trans[t].action)();
				set_marks(Trans[t], sig_M);
			}
		}
		
		
	}

}
