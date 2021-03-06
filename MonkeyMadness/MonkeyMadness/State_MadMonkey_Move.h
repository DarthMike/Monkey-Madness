/*
	Filename: State_MadMonkey_Move.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for MOVE (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#ifndef _STATE_MADMONKEY_MOVE
#define _STATE_MADMONKEY_MOVE

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Shared_Resources.h"

//Forward declarations
class b2Body;
class StateMachine_MadMonkey;

class State_MadMonkey_Move : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_MadMonkey_Move(AIAgent* actor,PhysicsManagerPointer physicsmgr):
    IAIState(actor),
	mPhysicsMgr(physicsmgr),
	mFinalDestAngle(0.0f),
	mFinalDestBody(NULL),
	mDestinationFound(false)
	{
		_init();
	}
	virtual ~State_MadMonkey_Move()
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
	StateMachine_MadMonkey* mSM;
	bool mDestinationFound;
	Vector2 mFinalDestPos;
	float mFinalDestAngle;
	b2Body* mFinalDestBody;
	std::string mFinalBodyChainId;
	//----- INTERNAL FUNCTIONS -----
	void _init();

};

#endif