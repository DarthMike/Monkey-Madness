/*
	Filename: StateMachine_MadMonkey.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Base class for states machines of AI agents
	Comments: Implementation of state machine class for a Mad Monkey AI agent
	Attribution: It is loosely based in two books implementation of a flexible state machine:
				 "Game coding complete"  http://www.mcshaffry.com/GameCode/
				 "Programming Game AI by Example" http://www.ai-junkie.com/books/toc_pgaibe.html
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _MADMONKEYSTATEMACHINE
#define _MADMONKEYSTATEMACHINE

//Library dependencies

//Class dependencies
#include "StateMachine.h"
#include "SM_MadMonkey_States.h"
#include "AIAgent.h"
#include "Shared_Resources.h"
#include "LogManager.h"

class StateMachine_MadMonkey : public StateMachine<AIAgent>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	StateMachine_MadMonkey(AIAgent* actor, PhysicsManagerPointer physicsptr, const std::string& type):
	  StateMachine(actor,physicsptr,type),
	  mPhysicsMgr(physicsptr),
	  mTargetBody(NULL),
	  mWithTarget(false)
	{
		assert(mSMType == "MadMonkey");
		//On start default to state "Search"
		SetState("Search");
	}
	virtual ~StateMachine_MadMonkey()
	{
		if(mCurrentState)
		{
			delete mCurrentState;
			mCurrentState = NULL;
		}
	}
	//----- GET/SET FUNCTIONS -----
	void SetTarget(const Vector2& newtarget, b2Body* body, const std::string& agentid, const std::string& targetbodychainid)
	{
		//Memo data
		mTarget = newtarget;
		mTargetBody = body;
		mTargetAgentId = agentid;
		mTargetBodyChainId = targetbodychainid;
	}
	Vector2 GetTargetPos() { return mTarget; }
	b2Body* GetTargetBody() {return mTargetBody; }
	std::string GetTargetAgentId() {return mTargetAgentId; }
	std::string GetTargetBodyChainId() { return mTargetBodyChainId; }
	void EnableTarget(bool enable) { mWithTarget = enable; }
	bool HasTarget() { return mWithTarget; }
	//----- OTHER FUNCTIONS -----
	//Implementation of virtual interface
	virtual bool HandleEvent(const EventData& data);				//Process possible events (with default implementation)
	virtual bool HandleCollision(const CollisionEventData& data); //Process possible collisions (with default implementation)
	//----- PUBLIC VARIABLES ------
private:
	//----- INTERNAL VARIABLES -----
	PhysicsManagerPointer mPhysicsMgr;

	//Specific for MadMonkey SM
	Vector2 mTarget;           //Target destination
	std::string mTargetAgentId;	//Target id for agent (if any)
	std::string mTargetBodyChainId;   //Target bodychain id for agent
	b2Body* mTargetBody;	   //Target body
	bool mWithTarget;
	//----- INTERNAL FUNCTIONS -----
	//Implementation of "CreateState" Method
	virtual void _createState(const std::string& statename);  //Creates a state given a state name (string) - it is specific of every state machine
};

#endif
