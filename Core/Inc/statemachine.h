/*
 * statemachine.h
 *
 *  Created on: 24 Aug 2021
 *      Author: revun
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include <stdlib.h>

typedef struct _Event_Queue Event_Queue;
typedef struct _StateMachine StateMachine;
typedef struct _Event Event;

typedef enum {ON, OFF, DEFAULT,MAXSTATES}state_typedef;

typedef enum {NOEVENT,STARTUP,PA0,PC13,MAXEVENTS}event_typedef;

typedef struct _StateMachine{

		state_typedef state;

		void(*ptr_set_state)(StateMachine*,int status);
		state_typedef(*ptr_get_state)(StateMachine*);

}StateMachine;

typedef struct _Event{
	event_typedef event;
}Event;

void set_state(StateMachine *inst,int status);

state_typedef  get_state(StateMachine *ptr_inst);

void initstatemachine(StateMachine *ptr_inst);

void event_handler(StateMachine *me, Event *e, Event_Queue *buffer);



typedef struct _Event_Queue{
	int size_of_buffer;
	event_typedef *event_buffer;
	int read_index;
	int write_index;
	int current_buffer_length;
	void(*ptr_Add_Event_To_Buffer)(Event_Queue *me, event_typedef event);
	event_typedef(*ptr_Pop_Event_From_Buffer)(Event_Queue *me);


}Event_Queue;

void init_buffer(Event_Queue *me , int size_of_buffer);
void Add_Event_To_Buffer(Event_Queue *me, event_typedef state);
event_typedef Pop_Event_From_Buffer(Event_Queue *me);

#endif /* INC_STATEMACHINE_H_ */
