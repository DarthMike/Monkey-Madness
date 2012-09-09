/*
	Filename: State_MadMonkey_Hang.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for HANG (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/


#include "State_MadMonkey_Hang.h"
#include "StateMachine_MadMonkey.h"
#include "PhysicsManager.h"
#include "GameEvents.h"
#include "GameEventManager.h"
#include "Math.h"
#include <sstream>

//Update ticks for state
void State_MadMonkey_Hang::Update(float dt)
{
	if(mThrown)
		mChangeStateDelay += dt;

	//Change of state is delayed
	if(mChangeStateDelay > 1500.0f	)
			mActor->ChangeState("Search");

	b2Vec2 currentspeed (mActor->GetPhysicalBody()->GetLinearVelocity());
	Vector2 currentpos(mActor->GetPosition());
	//IF - Just hanged
	if(mJustHanged)
	{
		//Store prev. speed and dont make calculations
		mPrevSpeed = Vector2(currentspeed.x,currentspeed.y);
		mPrevPos = currentpos;
		mJustHanged = false;
	}//ELSE - Was hanging before
	else
	{
		//IF - Big difference in speeds (Scalar projection in direction of prev. speed)
		//double scalarproj (mPrevSpeed.DotProduct(Vector2(currentspeed.x,currentspeed.y)));
		//float angle (SingletonMath::SignedAngleBetweenVecs(mPrevSpeed,Vector2(currentspeed.x,currentspeed.y)));
		//if(scalarproj < -0.3 && abs(angle) > (SingletonMath::Pi / 3))
		double speed = abs(currentpos.x + currentpos.y - mPrevPos.x - mPrevPos.y)/dt;
		
		if(speed > 0.018)
		{
			mIsShaking = true;
		}
		//Store prev. speed
		mPrevSpeed = Vector2(currentspeed.x,currentspeed.y);
		mPrevPos = mActor->GetPosition();

	}//IF


	//IF - Must fall because of big shaking
	if(mIsShaking)
	{
        mCounter += dt;

		if(mCounter > 800.0f)
		{
			//Get SM
			StateMachine_MadMonkey* sm (static_cast<StateMachine_MadMonkey*>(mActor->GetStateMachine()));
			MonkeyDetachedInfo eventinfo(sm->GetTargetBodyChainId(),sm->GetTargetAgentId());
			
			SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer (new MonkeyDetachedEvent(Event_MonkeyDetached,eventinfo))
												);

			//Disable target for agent
			sm->EnableTarget(false);
			//Detach from where it was
			mActor->DetachFromPoint();
			mActor->ChangeState("Fly");   //Fly!!!!
			mCounter = 0.0f;
		}
	}//ELSE - Not Shaking
	else
	{
		mCounter = 0.0f;
	}
}

//Enter Action for state
void State_MadMonkey_Hang::EnterAction()
{
	//Send a message monkey is hanging from branch
	//Get SM
	StateMachine_MadMonkey* sm (static_cast<StateMachine_MadMonkey*>(mActor->GetStateMachine()));
	
	MonkeyAttachedInfo eventinfo(sm->GetTargetBodyChainId(),sm->GetTargetAgentId());
	
	SingletonGameEventMgr::Instance()->QueueEvent(
									EventDataPointer (new MonkeyAttachedEvent(Event_MonkeyAttached,eventinfo))
										);

	mJustHanged = true;

	AnimationControllerPointer animcontroller (mActor->GetAnimationController());
	animcontroller->SetNextAnimation(false,1,true);
}

//Exit Action for state
void State_MadMonkey_Hang::ExitAction()
{
}

//Events plugin
bool State_MadMonkey_Hang::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_MadMonkey_Hang::HandleCollision(const CollisionEventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}