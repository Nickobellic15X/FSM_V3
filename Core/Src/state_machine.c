
#include "statemachine.h"
#include "main.h"


typedef enum {STATE_TRANSITION, STATE_HANDLED, STATE_IGNORED,  INIT_STATUS}status;


static status ON_State_Entry(StateMachine *me, Event *e, Event_Queue *buffer)
{
	me->state=ON;
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	return STATE_TRANSITION ;

}
static status ON_State_Continue(StateMachine *me, Event *e, Event_Queue *buffer)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	return STATE_HANDLED ; //No change is state, hence State_Handled

}
static status ON_State_Exit(StateMachine *me, Event *e, Event_Queue *buffer)
{

	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	me->state=OFF;
	return STATE_TRANSITION ;

}

static status OFF_State_Entry(StateMachine *me, Event *e, Event_Queue *buffer)
{
	me->state=OFF;
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);
	return STATE_TRANSITION;

}
static status OFF_State_Continue(StateMachine *me, Event *e, Event_Queue *buffer)
{

	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);

	return STATE_HANDLED;

}
static status OFF_State_Exit(StateMachine *me, Event *e, Event_Queue *buffer)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);
	me->state=ON;
	return STATE_TRANSITION;

}


static status DEFAULT_State(StateMachine *me,Event *e, Event_Queue *buffer)
{
	  me->state=DEFAULT;
	  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,1);
	  return INIT_STATUS;
}

static status Ignore_Event(StateMachine *me,Event *e, Event_Queue *buffer)
{
	return STATE_IGNORED;
}


typedef status (*StateMachineAction)(StateMachine *me, Event *e, Event_Queue *buffer);


StateMachineAction const Statemachine_table[MAXSTATES][MAXEVENTS] ={
	               /* No Event*/ 	  		/*STARTUP*/ 				  			/*PA0*/         				 /* PC13*/
/* On State */ 	 {&ON_State_Continue, 	 &Ignore_Event,      					 &Ignore_Event,    					 &ON_State_Exit  },
/*Off state */	 {&OFF_State_Continue,	 &Ignore_Event,      					 &OFF_State_Exit,   			     &Ignore_Event   },
/*Default state*/{&Ignore_Event, 		 &DEFAULT_State/*Default start up*/,     &Ignore_Event,    					 &OFF_State_Entry},
};



void StateMachine_Dispatch(StateMachine * me, Event *e, Event_Queue *buffer)
{
	status stat;
	state_typedef previous_state= me->state;
	e->event=Pop_Event_From_Buffer(buffer);
	stat= (*Statemachine_table[me->state][e->event])(me,e,buffer);

	 if (stat==STATE_TRANSITION)
	 {
		 (*Statemachine_table[previous_state][e->event])(me,(Event *)0,buffer);
		 (*Statemachine_table[me->state][e->event])(me,e,buffer);
	 }
	 else  if (stat == INIT_STATUS){
		 (*Statemachine_table[me->state][e->event])(me,e,buffer);
	 }
	return;
}

void set_state(StateMachine *inst,int status)
{
	inst->state=status;
}

state_typedef  get_state(StateMachine *ptr_inst) 	//return a state enum not an int
{
	return ptr_inst->state;
}


void initstatemachine(StateMachine *ptr_inst)
{

	ptr_inst->ptr_get_state = get_state;
	ptr_inst->ptr_set_state = set_state;
	ptr_inst->state=2;
	return;
}

void initevent(Event *me)
{
	me->event=STARTUP;
	return;
}


void init_buffer(Event_Queue *me,int size_of_buffer){
	me->size_of_buffer = size_of_buffer;
	me->event_buffer =  (event_typedef*)malloc(me->size_of_buffer*sizeof(event_typedef)); //Initializing the buffer to zero causes some problems, use malloc instead to solve this issue
	me->event_buffer[0] = STARTUP;
	me->current_buffer_length = 1;
	me->read_index = 0;
	me->write_index = 1;

	me->ptr_Pop_Event_From_Buffer=Pop_Event_From_Buffer;
	me->ptr_Add_Event_To_Buffer=Add_Event_To_Buffer;
	return;

}

void Add_Event_To_Buffer(Event_Queue *me, event_typedef event)
{
	if(me->current_buffer_length == me->size_of_buffer){

		/* User code to execute if
		 * buffer length is full, one may assert the above expression
		 */
	}
	else{
		me->event_buffer[me->write_index] = event; //Adding event that occurred  to buffer
		me->current_buffer_length++;
		me->write_index++;

	}
	return ;
}




event_typedef Pop_Event_From_Buffer(Event_Queue *me)
{

	if( me->current_buffer_length == 0){
		return NOEVENT;
		//If there are no events to process, we return the NOEVENT event_typedef, the no_event is considerent an event condition that does what is needs to do.
	}
	else{

		event_typedef e;
		e = me->event_buffer[me->read_index];
		me->current_buffer_length--;
		me->read_index++;
		if(me->read_index == me->size_of_buffer){
			me->read_index = 0;
		}
	return e ; //Return the event
	}

}
/*
 * >Solve the case where the buffer is empty
 */


