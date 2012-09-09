/*
	Filename: State_MadMonkey_KO.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for KO (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#ifndef _STATE_MADMONKEY_KO
#define _STATE_MADMONKEY_KO

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Shared_Resources.h"

class State_MadMonkey_KO : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_MadMonkey_KO(AIAgent* actor, PhysicsManagerPointer physicsmgr):
    IAIState(actor),
	mPhysicsMgr(physicsmgr),
	mCounter(0.0f)
	{
	}
	virtual ~State_MadMonkey_KO()
	{}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	//Interface implementation
	virtual void Update(float dt);   
	virtual void EnterAction();
	virtual void ExitAction();
	virtual bool HandleEvent(const EventData& data);
	virtual bool HandleCollision(const CollisionEventData& data); 
	//----- PUBLIC VARIABLES ------
private:
	//----- INTERNAL VARIABLES -----
	PhysicsManagerPointer mPhysicsMgr;
	float mCounter;
	//----- INTERNAL FUNCTIONS -----
};

#endif