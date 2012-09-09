/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine, and a solid body agent
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AIAgent.h"
#include "PhysicsEvents.h"
#include "GameEvents.h"
#include "Creatable_StateMachines.h"
#include "PhysicsManager.h"
#include "IndieLibManager.h"

//Update object status
void AIAgent::UpdateState(float dt)
{
	if(!mActive)
		return;

	//Update GFX
	b2Vec2 position (mParams.physicbody->GetPosition());
	mParams.position = Vector2(position.x,position.y);
	mParams.rotation = mParams.physicbody->GetAngle();
	Vector2 pixposition = SingletonIndieLib::Instance()->FromCoordToPix(mParams.position);
	int posz(mParams.sprite.gfxentity->GetPosZ());
	mParams.sprite.gfxentity->SetPosition(static_cast<float>(pixposition.x),static_cast<float>(pixposition.y),posz);
	mParams.sprite.gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::Instance()->RadiansToAngle<float>(mParams.rotation,true));
	//Update animation 
	if(mAnimController)
	{
		mAnimController->Update(dt);
	}

	//Update state machine
	if(mStateMachine)
	{
		//IF - State change requested
		if(mStateChange && mNextStateRequested != "")
		{
			mStateMachine->SetState(mNextStateRequested);
			mStateChange = false;
		}//ELSE - No state change
		else
		{
			mStateMachine->Update(dt);
		}//IF
	}

	if(mOutOfLimits)
	{
		//Just Destroy agent
		Destroy(); 

		//Send event of AI out of Limits
		AgentOutInfo data(mId,mParams.type,mParams.position);
		//Send event as monkey is out of limits
		SingletonGameEventMgr::Instance()->QueueEvent(
											EventDataPointer(new AgentOutEvent(Event_AgentOut,data))
											);
	}
}

//Process possible collisions
bool AIAgent::HandleCollision(const CollisionEventData& data)
{
	const ContactInfo& collinfo (data.GetCollisionData());

	//IF - New collision
	if(data.GetEventType() == Event_NewCollision)
	{
		
		//IF Check type of body collided with - STATIC
		if((collinfo.collidedbody1 != data.GetActiveBody()
		   &&
		   collinfo.collidedbody1->IsStatic())
		   ||
		   (collinfo.collidedbody2 != data.GetActiveBody()
		   &&
		   collinfo.collidedbody2->IsStatic())
		   )
		{
			mWorldGroundCollisions++;
		}//IF
		
	}//ELSE - Deleted collision
	else if(data.GetEventType() == Event_DeletedCollision)
	{
		//Check type of body collided with - STATIC
		if((collinfo.collidedbody1 != data.GetActiveBody()
		   &&
		   collinfo.collidedbody1->IsStatic())
		   ||
		   (collinfo.collidedbody2 != data.GetActiveBody()
		   &&
		   collinfo.collidedbody2->IsStatic())
		   )
		{
			mWorldGroundCollisions--;
		}
	}//IF

	//Allways forward collision to state machine
	mStateMachine->HandleCollision(data);
	
	return (true);
}

//Handle possible events
bool AIAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed (false);
	
	//IF - DestroyedJoint event
	if(data.GetEventType() == Event_DestroyedJoint)
	{
		const DestroyedJointEvent& theevent (static_cast<const DestroyedJointEvent&>(data));
		if(theevent.GetEventData().name == mAttachJointName)
			mAttachJointName = "";
	}//IF

	//Forward event processing to state machine
	eventprocessed = mStateMachine->HandleEvent(data);

	return eventprocessed;
}

//Handle out of limits
void AIAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	mOutOfLimits = true;
}	

//Create from params
void AIAgent::Create( const GameAgentPar *params)	
{
	
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == AI);
	
	//Cast down params hierarchy to this type of params
	const AIAgentPar *aiparams = static_cast<const AIAgentPar*>(params);
	//Copy all parameters
	mParams = *aiparams;

	//Syncronize body position for first time
	b2Vec2 position (mParams.physicbody->GetPosition());
	mParams.position = Vector2(position.x,position.y);

	//Parameters copied, create state machine
	_createStateMachine(mParams.agentAI);

	//Add animation sprite to agent
	if(mParams.sprite.gfxentity->GetAnimation() != NULL)
	{
		mAnimController = AnimationControllerPointer(new AnimationController(mParams.sprite.gfxentity,0,true,false));
	}

	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void AIAgent::Destroy()
{
	if(!mActive)
		return;

	//Stop state machine
	if(mStateMachine)
	{
		mStateMachine->Stop();
	}

	//Delete from GFX system created sprite
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mParams.sprite.gfxentity.get());

	//Delete bodies data
	std::string bodyname (mPhysicsMgr->GetBodyName(mParams.physicbody));
	mPhysicsMgr->DestroyBody(bodyname);

	//Finally, update internal tracking
	mActive = false;
}

//State machine state change
void AIAgent::ChangeState(const std::string& newstatename)
{
	if(!mActive)
		return;
	//Memorize
	mNextStateRequested = newstatename;			
	mStateChange = true;
}

//Movement control
void AIAgent::Jump(const Vector2& direction, float strength)
{
	if(!mActive)
		return;

	assert(strength >=0.0f && strength <= 1.0f);  //Checkings
	
	//Perform a jump with requested strength
	//IF - Touching ground
	if(mWorldGroundCollisions > 0)
	{
		b2Vec2 impulse(static_cast<float>(direction.x),static_cast<float>(direction.y));
		impulse*= strength * mParams.maxsteerforce;
		mParams.physicbody->ApplyImpulse(impulse,mParams.physicbody->GetPosition());
	}//IF
}

void AIAgent::Jump(const Vector2& targetpoint)
{
	if(!mActive)
		return;

	//Perform a jump to a point. This needs a calculation of required direction and force
	//Calculate the parameters to jump to this target
	//IF - Touching ground
	if(mWorldGroundCollisions <= 0)
		return;

	//Jump time calculation
	/*  Equation (2nd grade):
	      vy + sqrt(2g(end-start) + vy^2)      vy - sqrt(2g(end-start) + vy^2) ; 
	  t =--------------------------------  t =--------------------------------
					   g                                     g

	   With the time of impact only in Y axis, we find desired x speed, (2 solutions possible, long and short).
	   IF t is < 0 it means imaginary solution, that is, speed in Y is too small to make the jump!!!! PROGRAM BUG IN THIS CASE.
	  
	  Extracted from "Artificial Intelligence for Games" - Ian Millington
	*/

	float gravityaccy(-30.0f);
	double sqrtterm (sqrt((2*(gravityaccy)*(targetpoint.y - mParams.position.y)) + (mParams.maxspeedy * mParams.maxspeedy)));
	double time ((-mParams.maxspeedy - sqrtterm) / gravityaccy);

	if(time < 0)
	{
		//Cannot jump! it is too far
		//Make a max jump in half direction x with max speed in y, half speed in X
		Vector2 dir (targetpoint - mParams.position);
		dir.x /= 2;
		dir.Normalise();
		dir.x *= mParams.maxspeedx / 2;
		dir.x *= mParams.maxspeedy;
		#ifdef _DEBUGGING
			SingletonLogMgr::Instance()->AddNewLine("AIAgent::Jump","PROGRAM BUG: CANNOT JUMP, TOO FAR",LOGEXCEPTION);
		#endif
		mParams.physicbody->SetLinearVelocity(b2Vec2(static_cast<float>(dir.x),static_cast<float>(dir.y)));
		return;
	}

	//Check if the time can be used
	Vector2 desiredspeed(0,mParams.maxspeedy);

	desiredspeed.x = (targetpoint.x - mParams.position.x) / time;

	//IF - Speed too large
	if(abs(desiredspeed.x) > mParams.maxspeedx)
	{
		time = (-mParams.maxspeedy + sqrtterm) / gravityaccy;
		desiredspeed.x = (targetpoint.x - mParams.position.x) / time;
		//IF - Again speed too large
		if(abs(desiredspeed.x) > mParams.maxspeedx)
		{
			if(desiredspeed.x > 0)
				desiredspeed.x = mParams.maxspeedx;
			else
				desiredspeed.x = -mParams.maxspeedx;
			#ifdef _DEBUGGING
			SingletonLogMgr::Instance()->AddNewLine("AIAgent::Jump","PROGRAM BUG: JUMP IMPOSSIBLE",LOGEXCEPTION);
			#endif
		}//IF
	}//IF
	
	mParams.physicbody->SetLinearVelocity(b2Vec2(static_cast<float>(desiredspeed.x),static_cast<float>(desiredspeed.y)));
	//mParams.physicbody->ApplyImpulse(b2Vec2(mParams.maxsteerforce* 0.05f,mParams.maxsteerforce * 0.05),mParams.physicbody->GetPosition());
}

void AIAgent::AttachToPoint(const Vector2&, b2Body* bodytoattach)
{
	if(!mActive)
		return;

	//IF - Already attached
	if(mAttachJointName != "")
		return;

	//Create a fixed joint with a point to attach to it (it has to belong to a body)
	b2FixedJointDef fixedjointdef;
	fixedjointdef.collideConnected = false;
	fixedjointdef.Initialize(mParams.physicbody,bodytoattach);
	
	//Create the joint
	if(mPhysicsMgr->CreateFixedJoint(&fixedjointdef,mId+"FixedJoint"))
		mAttachJointName = mId+"FixedJoint";
}

void AIAgent::DetachFromPoint()
{
	if(!mActive)
		return;

	//IF - Not Attached
	if(mAttachJointName == "")
		return;

	mPhysicsMgr->DestroyJoint(mAttachJointName);

	mAttachJointName = "";
}

void AIAgent::ApplyForce(const Vector2& direction, float strength)
{
	if(!mActive)
		return;

	//Apply a force inconditionally to the agent body
	assert(strength >=0.0f && strength <= 1.0f);  //Checkings
	Vector2 clampedsteerforce(direction * strength * mParams.maxsteerforce);
	mParams.physicbody->ApplyForce(b2Vec2(static_cast<float>(direction.x),static_cast<float>(direction.y)),mParams.physicbody->GetPosition());

}

void AIAgent::ChangePosition(const Vector2& point, float angle)
{
	if(!mActive)
		return;

	//Change the position inconditionally to the agent body (USE WITH CARE)
	mParams.physicbody->SetXForm(b2Vec2(static_cast<float>(point.x),static_cast<float>(point.y)),angle);
}
//State machine class instantiation
void AIAgent::_createStateMachine( const std::string& agentAI )
{
	//Association of text property and AI state machine instantiation
	if(agentAI == "MadMonkey")
	{
		mStateMachine = new StateMachine_MadMonkey(this,mPhysicsMgr,"MadMonkey");	
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("AIAgent::_createStateMachine","Error: Bad Agent AI state machine name! Agent will not make anything!",LOGEXCEPTION);
	}
}

//Memory cleanup
void AIAgent::_release()
{
	Destroy();

	//Cleanup memory
	if(mStateMachine)
	{
		delete	mStateMachine;
		mStateMachine = NULL;
	}
}