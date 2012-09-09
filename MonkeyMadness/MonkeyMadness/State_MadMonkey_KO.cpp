/*
	Filename: State_MadMonkey_KO.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for KO (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_MadMonkey_KO.h"
#include "StateMachine_MadMonkey.h"
#include "GameEvents.h"
#include "GameEventManager.h"
#include <sstream>

//Update ticks for state
void State_MadMonkey_KO::Update(float dt)
{
	//Just count ko time
	mCounter += dt;

	if(mCounter > 10000.0f)
	{
		mActor->ChangeState("Search");
	}
}

//Enter Action for state
void State_MadMonkey_KO::EnterAction()
{
	//Send event as monkey is KO
	MonkeyKOInfo eventinfo(mActor->GetPosition());
	SingletonGameEventMgr::Instance()->QueueEvent(
										EventDataPointer(new MonkeyKOEvent(Event_MonkeyKO,eventinfo))
										);
	mCounter = 0.0f;

	AnimationControllerPointer animcontroller (mActor->GetAnimationController());
	animcontroller->SetNextAnimation(false,3,true);

	//Reset rotation (sprite coherence)
	Vector2 actorpos (mActor->GetPosition());
	mActor->ChangePosition(actorpos,0.0f);
	b2Body* actorbody(mActor->GetPhysicalBody());
	actorbody->SetAngularVelocity(0.0f);
}

//Exit Action for state
void State_MadMonkey_KO::ExitAction()
{
}

//Events plugin
bool State_MadMonkey_KO::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_MadMonkey_KO::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}