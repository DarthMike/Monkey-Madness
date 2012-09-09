/*
	Filename: BodyChainAgent.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent composed of many bodies in chain
	Comments: Composed of many BodyChain objects together
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "BodyChainAgent.h"
#include "PhysicsManager.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "GameEventManager.h"
#include "IndieLibManager.h"

//Get the chain which is connected to ground (if any)
BodyChainPointer BodyChainAgent::GetRootChain()
{
	b2Body* groundbody (mPhysicsMgr->GetBody(""));  //Empty name is ground body

	BodyChain::BodyChainListIterator itr;
	//LOOP - Search for the starting body chain
	for(itr = mParams.bodychainlist.begin(); itr != mParams.bodychainlist.end();++itr)
	{
		if((*itr)->GetPrevBodyLink() == groundbody)
			return (*itr);
	}//LOOP END

	return BodyChainPointer();
}


//Get the chain where this body belongs
BodyChainPointer BodyChainAgent::GetBodyChainfromBody(b2Body* bodytosearch)
{
	BodyChain::BodyChainListIterator itr;
	//LOOP - Search for the starting body chain
	for(itr = mParams.bodychainlist.begin(); itr != mParams.bodychainlist.end();++itr)
	{
		if((*itr)->IsBodyInChain(bodytosearch))
			return (*itr);
	}//LOOP END

	//Not found
	return BodyChainPointer();
}

//Update object status
void BodyChainAgent::UpdateState(float dt)
{
	//IF - Agent is active
	if(!mActive)
		return;

	//---------------Update "Inactive" Bodychains-------------
	BodyChain::BodyChainListIterator inactiveitr;
	//LOOP - Inactive bodychains
	for(inactiveitr = mBrokenBranches.begin(); inactiveitr != mBrokenBranches.end();++inactiveitr)
	{
		(*inactiveitr)->Update(dt);
	}//LOOP END


	//---------------Update "Active" Bodychains-------------
	BodyChain::BodyChainListIterator itr (mParams.bodychainlist.begin());
	//LOOP - All body chains
	while(itr != mParams.bodychainlist.end())
	{
		(*itr)->Update(dt);
		//IF - Branch broke this time
		if((*itr)->IsBroken())
		{
			//Insert it in "inactive" list
			mBrokenBranches.push_back(*itr);

			//Send event as this  branch is broken
			mNumActiveBranches--;  //Update tracking
			BranchBrokeInfo eventinfo((*itr)->GetId(),mId,mNumActiveBranches);
			_sendBranchBrokeEvent(eventinfo);

			//Search all connected branches to this one and mark them broken
			std::string id((*itr)->GetId());
			BodyChain::BodyChainListIterator otheritr;
			//LOOP - Find connected branches to this, starting from this position in container
			for(otheritr = itr; otheritr != mParams.bodychainlist.end();++otheritr)
			{
				//IF - Connected
				if((*otheritr)->GetPreviousBodyChainId() == id)
				{
					(*otheritr)->SetBroken(true);
					mNumActiveBranches--;
					//Send event as branch is broken
					BranchBrokeInfo eventinfo((*otheritr)->GetId(),mId,mNumActiveBranches);
					_sendBranchBrokeEvent(eventinfo);
					
				}
			}//LOOP END

			//Delete it from "active" list
			itr = mParams.bodychainlist.erase(itr);  	
		}
		else
		{
			++itr;  //Nothing, just continue
		}
	}//LOOP

	if(mOutOfLimits)
		Destroy();	
}

//Process possible collisions
bool BodyChainAgent::HandleCollision(const CollisionEventData&)
{
	if(!mActive)
		return false;

	return true;
}

//Process possible events
bool BodyChainAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed(false);

	if(!mActive)
		return false;

	//IF - A Monkey attached
	if(data.GetEventType() == Event_MonkeyAttached)
	{
		const MonkeyAttachedEvent& eventdata (static_cast<const MonkeyAttachedEvent&>(data));
		
		//IF - Attached to this agent
		if(eventdata.GetAgentId() == mId)
		{
			//Find branch (bodychain)
			BodyChain::BodyChainListIterator itr;
			std::string bodychainid (eventdata.GetBodyChainId());
			//LOOP - Find the affected bodychain list
			for(itr = mParams.bodychainlist.begin();itr != mParams.bodychainlist.end();++itr)
			{
				//IF - Found
				if((*itr)->GetId() == bodychainid)
				{
					(*itr)->IncrementAttached();
				}//IF
			}//LOOP
		}//IF

	}//ELSE - A Monkey detached
	else if(data.GetEventType() == Event_MonkeyDetached)
	{
		const MonkeyDetachedEvent& eventdata (static_cast<const MonkeyDetachedEvent&>(data));
		
		//IF - Attached to this agent
		if(eventdata.GetAgentId() == mId)
		{
			//Find branch (bodychain)
			BodyChain::BodyChainListIterator itr;
			std::string bodychainid (eventdata.GetBodyChainId());
			//LOOP - Find the affected bodychain list
			for(itr = mParams.bodychainlist.begin();itr != mParams.bodychainlist.end();++itr)
			{
				//IF - Found
				if((*itr)->GetId() == bodychainid)
				{
					(*itr)->DecrementAttached();
				}//IF
			}//LOOP
		}//IF
	}//IF

	return eventprocessed;
}

//Handle out of limits
void BodyChainAgent::HandleOutOfLimits(const OutOfLimitsEventData& data) 
{ 
	const OutOfLimitsData& eventdata (data.GetEventData());
	bool found (false);
	BodyChain::BodyChainListIterator inactiveitr;
	//LOOP - Inactive bodychains
	for(inactiveitr = mBrokenBranches.begin(); inactiveitr != mBrokenBranches.end();++inactiveitr)
	{
		//Destroy entire bodychain
		if((*inactiveitr)->IsBodyInChain(eventdata.body))
		{
			(*inactiveitr)->Destroy();
			mBrokenBranches.erase(inactiveitr);
			found = true;
			break;
		}
	}//LOOP END

	//IF - Must be active bodychain
	if(!found)
	{
		//Search body in active body chains and destroy it
		BodyChain::BodyChainListIterator itr;
		//LOOP - Search given body
		for(itr = mParams.bodychainlist.begin(); itr != mParams.bodychainlist.end(); ++itr)
		{
			//Destroy entire bodychain
			if((*itr)->IsBodyInChain(eventdata.body))
			{
				(*itr)->Destroy();
				mParams.bodychainlist.erase(itr);
				break;
			}
		}//LOOP END
	}//IF
}	

//Create from params
void BodyChainAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == BODYCHAIN);
	
	//Cast down params hierarchy to this type of params
	const BodyChainPar *pagentparams = static_cast<const BodyChainPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;

	//Reference to this agent all created bodies from bodychains
	//Update in the same loop the number of active branches
	BodyChain::BodyChainListIterator itr;
	mNumActiveBranches = 0;
	//LOOP - All body chains
	for(itr = mParams.bodychainlist.begin(); itr != mParams.bodychainlist.end(); ++itr)
	{
		(*itr)->SetAgentReference(this);
		if((*itr)->AgentsCanAttach())
		{
			mNumActiveBranches++;
		}
	}//LOOP
	
	//Update chain association with each other (previous link)
	BodyChain::BodyChainList::reverse_iterator revitr;
	//LOOP - All body chains
	for(revitr = mParams.bodychainlist.rbegin(); revitr != mParams.bodychainlist.rend(); ++revitr)
	{
		b2Body* prevbody ((*revitr)->GetPrevBodyLink());
		bool found(false);
		//As we create the bodies in order starting from the most downwards, only the
		//previously created bodychains are possible to be this previous

		BodyChain::BodyChainList::reverse_iterator previtr(revitr);
		//LOOP - Search which one is the previous
		while(!found && previtr != mParams.bodychainlist.rend())
		{
			if((*previtr)->IsBodyInChain(prevbody))
			{
				//Cross-reference them
				(*revitr)->SetPreviousBodyChainId((*previtr)->GetId());
				(*previtr)->SetNextBodyChainId((*revitr)->GetId());
				found = true;
			}
			else
			{
				previtr++;
			}
		}//LOOP
	}//LOOP
	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void BodyChainAgent::Destroy()
{
	if (!mActive)
		return;

	//Destroy related bodies
	//Reference to this agent all created bodies from bodychains
	BodyChain::BodyChainListIterator itr;
	//LOOP - All body chains
	for(itr = mParams.bodychainlist.begin(); itr != mParams.bodychainlist.end(); ++itr)
	{
		(*itr)->Destroy();
	}//LOOP
	
	//Finally, update internal tracking
	mActive = false;
}
//Init internal resources
void BodyChainAgent::_init()
{
}

void BodyChainAgent::_sendBranchBrokeEvent(const BranchBrokeInfo& eventdata)
{
	SingletonGameEventMgr::Instance()->QueueEvent(
							EventDataPointer(new BranchBrokeEvent(Event_BranchBroke,eventdata))
							);
}