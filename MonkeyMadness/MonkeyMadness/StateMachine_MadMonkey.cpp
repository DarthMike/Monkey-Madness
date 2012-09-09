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

#include "StateMachine_MadMonkey.h"
#include "GameEvents.h"

//Events plugin
bool StateMachine_MadMonkey::HandleEvent(const EventData& data)			
{
	bool eventprocessed(false);

	//IF - Exists state
	if(mCurrentState)
	{
		//Process overall state change

		//IF - A branch broke
		if(data.GetEventType() == Event_BranchBroke)
		{
			const BranchBrokeEvent& eventdata (static_cast<const BranchBrokeEvent&>(data));

			//IF - The target was this branch
			if(mTargetBodyChainId == eventdata.GetBodyChainId()
			   &&
			   mTargetAgentId == eventdata.GetAgentId())
			{
				//Reset state to search, as this branch is broken to hang
				mActor->ChangeState("Search");

				//Detach from Point 
				mActor->DetachFromPoint();
			}//IF 
		}
		
		//Forward event to current state
		return(eventprocessed || mCurrentState->HandleEvent(data));
	}

	//No state defined
	return eventprocessed;
}

//Physics collisions events
bool StateMachine_MadMonkey::HandleCollision(const CollisionEventData& data) 
{
	//Use default implementation
	return (StateMachine<AIAgent>::HandleCollision(data));	
}


void StateMachine_MadMonkey::_createState(const std::string& statename)  //Creates a state given a state name (string) - it is specific of every state machine
{
	if(statename == "Search")
	{
		mCurrentState = new State_MadMonkey_Search(mActor,mPhysicsMgr);
	}
	else if(statename == "Move")
	{
		mCurrentState = new State_MadMonkey_Move(mActor,mPhysicsMgr);
	}
	else if(statename == "Hang")
	{
		mCurrentState = new State_MadMonkey_Hang(mActor,mPhysicsMgr);
	}
	else if(statename == "Fly")
	{
		mCurrentState = new State_MadMonkey_Fly(mActor,mPhysicsMgr);
	}
	else if(statename == "KO")
	{
		mCurrentState = new State_MadMonkey_KO(mActor,mPhysicsMgr);
	}//ELSE - Not defined state!
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("StateMachine_MadMonkey::_createState","State name '" + statename + "' for a MadMonkey is not correct! Program BUG!",LOGEXCEPTION);
	}//IF

	//IF - Created
	if(mCurrentState)
	{
		//Memo state name and execute enter action
		mCurrentStateName = statename;
		mCurrentState->EnterAction();
	}//IF
}