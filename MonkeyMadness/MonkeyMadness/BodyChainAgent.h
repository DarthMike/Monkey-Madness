/*
	Filename: BodyChainAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent composed of many bodies in chain
	Comments: Composed of many BodyChain objects together
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
*/

#ifndef _BODYCHAINAGENT
#define _BODYCHAINAGENT

//Library dependencies
#include <assert.h>
#include <list>
//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "BodyChain.h"

//Properties to contain from agent - Inherited
struct BodyChainPar : public GameAgentPar
{
		BodyChainPar():
		GameAgentPar()
		{
			//Determine type
			type = BODYCHAIN;
		}
		
		BodyChain::BodyChainList bodychainlist;  //Body chains defined in level
		
		//Assignment operator for fast copying
		BodyChainPar& operator=(const BodyChainPar &newparams)
		{
			//Deep copy of elements in struct
			bodychainlist = newparams.bodychainlist;
			return (*this);
		}
};

class BodyChainAgent : public IAgent
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BodyChainAgent(const std::string& id, PhysicsManagerPointer physicsptr):
	  IAgent(id),
	  mPhysicsMgr(physicsptr),
	  mActive(false),
	  mOutOfLimits(false),
	  mNumActiveBranches(0)
	{
		_init();
	};
	virtual ~BodyChainAgent(){}
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }						//Get type of agent
	virtual bool IsAlive()  { return mActive; }             //Get if agent was destroyed
	virtual Vector2 GetPosition() {return mParams.position; }			//Get Position of agent
	virtual float GetRotation() {return mParams.rotation; }		//Get rotation of agent
	//Specific accessors for BodyChain Agent
	BodyChainPointer GetRootChain();    //Get the chain which is connected to ground (if any)
	BodyChain::BodyChainList::const_iterator GetFirstBodyChainItr() { return mParams.bodychainlist.begin(); }
	BodyChain::BodyChainList::const_iterator GetLastBodyChainItr() { return mParams.bodychainlist.end(); }
	BodyChainPointer GetBodyChainfromBody(b2Body* bodytosearch);
	int GetActiveBranches() { return mNumActiveBranches; }

	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	PhysicsManagerPointer mPhysicsMgr;	//PhysicsManager pointer
	BodyChainPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	bool mOutOfLimits;						//"Out of Limits" tracking

	int mNumActiveBranches;                 //Number of "not broken" branches
	BodyChain::BodyChainList mBrokenBranches;  //Broken branches (inactive)
	//---- INTERNAL FUNCTIONS ----
	void _init();							//Init internal resources
	void _sendBranchBrokeEvent(const BranchBrokeInfo& eventdata);
};

#endif