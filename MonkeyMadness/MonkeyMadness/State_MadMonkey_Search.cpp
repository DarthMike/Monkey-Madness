/*
	Filename: State_MadMonkey_Search.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for SEARCH (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "State_MadMonkey_Search.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "StateMachine_MadMonkey.h"

//Update ticks for state
void State_MadMonkey_Search::Update(float)
{
	//IF - Target found
	if(mTargetFound)
	{
		//Memorize target in agent
		b2Vec2 targetpos (mTargetBody->GetPosition());
		StateMachine_MadMonkey* sm (static_cast<StateMachine_MadMonkey*>(mActor->GetStateMachine()));
		sm->SetTarget(Vector2(targetpos.x,targetpos.y),mTargetBody,mTargetId,mTargetBodyChainId);
		sm->EnableTarget(true);

		//Change to next state
		mActor->ChangeState("Move");
	}
}

//Enter Action for state
void State_MadMonkey_Search::EnterAction()
{
	//Request new target!
	NewTargetRequestInfo eventinfo(mActor->GetPosition(),mActor->GetId());

	SingletonGameEventMgr::Instance()->QueueEvent(
									EventDataPointer(new NewTargetRequestEvent(Event_NewTargetRequest,eventinfo))
												);	

}

//Exit Action for state
void State_MadMonkey_Search::ExitAction()
{

}

//Events plugin
bool State_MadMonkey_Search::HandleEvent(const EventData& data)
{
	bool eventprocessed(false);

	//Receive a new target from game\
	//IF - New AI target event
	if(data.GetEventType() == Event_NewAITarget)
	{
		const NewAITargetEvent& eventdata (static_cast<const NewAITargetEvent&>(data));
		//IF- Target is for ME!
		if(eventdata.GetAIAgentName() == mActor->GetId())
		{
			mTargetBody = eventdata.GetTargetBody();
			mTargetId = eventdata.GetAgentName();
			mTargetBodyChainId = eventdata.GetBodyChainId();
			mTargetFound = true;
			eventprocessed = true;
		}//IF
	}//IF

	return eventprocessed;
}

//Physics collisions plugin
bool State_MadMonkey_Search::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}