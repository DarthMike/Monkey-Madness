/*
	Filename: State_MadMonkey_Move.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: State Class for MOVE (MadMonkey State Machine)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity  
*/

#include "State_MadMonkey_Move.h"
#include "StateMachine_MadMonkey.h"
#include "PhysicsManager.h"
#include "BodyChainAgent.h"

//Update ticks for state
void State_MadMonkey_Move::Update(float)
{
	//Update animations
	AnimationControllerPointer animcontroller (mActor->GetAnimationController());
	animcontroller->SetNextAnimation(false,0,false);

	//Track target position changes
	b2Body* targetbody (mSM->GetTargetBody());
	std::string agentid (mSM->GetTargetAgentId());
	std::string bodychainid(mSM->GetTargetBodyChainId());
	b2Vec2 bodypos (targetbody->GetPosition());

	mSM->SetTarget(Vector2(bodypos.x,bodypos.y),targetbody,agentid,bodychainid);
	mSM->EnableTarget(true);

	//IF - Destination found
	if(mDestinationFound)
	{
		mActor->ChangePosition(mFinalDestPos,mFinalDestAngle);
		mActor->AttachToPoint(mFinalDestPos,mFinalDestBody);
		
		//Change to next state
		mActor->ChangeState("Hang");
	}
	else if(mSM->HasTarget())
	{

		Vector2 targetpoint(mSM->GetTargetPos());
		Vector2 position(mActor->GetPosition());
		Vector2 posdifference(targetpoint - position);

		//IF -  Far away
		if(abs(posdifference.x) > 15.0f)   //TODO: PARAMETRIZE THIS!!
		{
			//Far away, make jumps
			if(posdifference.x > 0)
			{
				mActor->Jump(Vector2(0.4,0.6),1.0f);	
			}
			else
			{
				mActor->Jump(Vector2(-0.4,0.6),1.0f);
			}
		}//ELSE - CLOSE ENOUGH!
		else
		{
			//Just make the jump calculated to end in the place we want!
			mActor->Jump(targetpoint);
		}
	}
}

//Enter Action for state
void State_MadMonkey_Move::EnterAction()
{
	assert(mSM->GetTargetBody());
	assert(mSM->GetTargetAgentId() != "");

	//Reset rotation (sprite coherence)
	Vector2 actorpos (mActor->GetPosition());
	mActor->ChangePosition(actorpos,0.0f);
	b2Body* actorbody(mActor->GetPhysicalBody());
	actorbody->SetAngularVelocity(0.0f);
}

//Exit Action for state
void State_MadMonkey_Move::ExitAction()
{
	if(mDestinationFound)
	{
		//Change variables of SM
		std::string targetagentid (mSM->GetTargetAgentId());
		mSM->SetTarget(mFinalDestPos,mFinalDestBody,targetagentid,mFinalBodyChainId);
		mSM->EnableTarget(true);
	}
}

//Events plugin
bool State_MadMonkey_Move::HandleEvent(const EventData&)
{
	bool eventprocessed(false);

	return eventprocessed;
}

//Physics collisions plugin
bool State_MadMonkey_Move::HandleCollision(const CollisionEventData& data)
{
	bool eventprocessed(false);

	const ContactInfo collisiondata (data.GetCollisionData());
	//IF - New collision event
	if(data.GetEventType() == Event_NewCollision)
	{
		//New collision event: Handle Hanging from branch
		b2Shape* othershape(NULL);
		b2Body* otherbody(NULL);
		std::string targetid (mSM->GetTargetAgentId());
		std::string targetbodychainid(mSM->GetTargetBodyChainId());
		
		BodyChainAgent* theagent(NULL);
		//First check the other agent is the target bodychain agent
		if(collisiondata.agent1->GetType() == BODYCHAIN
		   &&
		   collisiondata.agent1->GetId() == targetid)
		{
			theagent = static_cast<BodyChainAgent*>(collisiondata.agent1);
			otherbody = collisiondata.collidedbody1;
			othershape = collisiondata.collidedshape1;
		}
		else if(collisiondata.agent2->GetType() == BODYCHAIN
			    &&
				collisiondata.agent2->GetId() == targetid)
		{
			theagent = static_cast<BodyChainAgent*>(collisiondata.agent2);
			otherbody = collisiondata.collidedbody2;
			othershape = collisiondata.collidedshape2;			
		}
		
		//IF - Collision with other bodychain agent
		if(theagent)
		{
			assert(otherbody);
			BodyChainPointer thebodychain (theagent->GetBodyChainfromBody(otherbody));
			if(thebodychain)
			{
				BodyChain::BodiesList::const_iterator itr (thebodychain->GetFirstBodyIterator());
				if(thebodychain->GetId() == targetbodychainid && otherbody != (*itr)
				   && !thebodychain->IsBroken() && thebodychain->AgentsCanAttach())
				{
					mFinalDestPos = Vector2(collisiondata.position.x,collisiondata.position.y);
					mFinalDestAngle = otherbody->GetAngle();
					mFinalDestBody = otherbody;
					mFinalBodyChainId = thebodychain->GetId();
					mDestinationFound = true;
				}
			}
		}//IF
	}//IF

	return eventprocessed;
}

void State_MadMonkey_Move::_init()
{
	//Store SM pointer
	mSM = static_cast<StateMachine_MadMonkey*>(mActor->GetStateMachine());
}