/*
	Filename: State_MadMonkey_Fly.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for FLY (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_MadMonkey_Fly.h"
#include "StateMachine_MadMonkey.h"
#include "PhysicsManager.h"
#include "AnimationController.h"

//Update ticks for state
void State_MadMonkey_Fly::Update(float)
{
	//IF - Check if collided 
	if(mCollided)
	{
		//KO?
		if(mKO)
			mActor->ChangeState("KO");   //KO!!
		else
			mActor->ChangeState("Move");  //Go back to work!
	}//IF
}

//Enter Action for state
void State_MadMonkey_Fly::EnterAction()
{
	AnimationControllerPointer animcontroller (mActor->GetAnimationController());
	animcontroller->SetNextAnimation(false,2,true);

	b2Body* actorbody(mActor->GetPhysicalBody());
	float angularvel(actorbody->GetAngularVelocity());
	if(angularvel >= 0.0f)
		actorbody->ApplyTorque(60.0f);  //Rotate it (procedurally generated animation! ;)
	else
		actorbody->ApplyTorque(-60.0f);  //Rotate it (procedurally generated animation! ;)
}

//Exit Action for state
void State_MadMonkey_Fly::ExitAction()
{
}

//Events plugin
bool State_MadMonkey_Fly::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_MadMonkey_Fly::HandleCollision(const CollisionEventData& data)
{
	bool eventprocessed(false);

	if(data.GetEventType() == Event_CollisionResult)
	{
		const ContactInfo& collisiondata (data.GetCollisionData());
		
		float normalimpulse(0.0f);
		
		//IF - Physical collision
		if(
			(collisiondata.collidedbody1 != data.GetActiveBody()
		    &&
		    collisiondata.agent1->GetType() == PHYSICBODY
		    )
		    ||
		    (collisiondata.collidedbody2 != data.GetActiveBody()
		    &&
		    collisiondata.agent2->GetType() == PHYSICBODY
		    )
		   )
		{
			normalimpulse = collisiondata.normalimpulse;
			mCollided = true;
		}//IF

		//IF - Max normal surpassed
		if(	normalimpulse > 12.0f)
		{
			mKO = true;	
		}//IF
	}

	return eventprocessed;
}