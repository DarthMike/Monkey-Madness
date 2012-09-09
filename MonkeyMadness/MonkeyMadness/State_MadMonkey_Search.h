/*
	Filename: State_MadMonkey_Search.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for SEARCH (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#ifndef _STATE_MADMONKEY_SEARCH
#define _STATE_MADMONKEY_SEARCH

//Library dependecies

//Class dependencies
#include "IAIState.h"
#include "AIAgent.h"
#include "Shared_Resources.h"

//Forward declarations
class b2Body;

class State_MadMonkey_Search : public IAIState<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	State_MadMonkey_Search(AIAgent* actor, PhysicsManagerPointer physicsmgr):
    IAIState(actor),
	mPhysicsMgr(physicsmgr),
	mTargetFound(false),
	mTargetBody(NULL)
	{
	}
	virtual ~State_MadMonkey_Search()
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

	bool mTargetFound;
	b2Body* mTargetBody;
	std::string mTargetId;
	std::string mTargetBodyChainId;
	//----- INTERNAL FUNCTIONS -----

};

#endif
