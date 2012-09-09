/*
	Filename: PlayerAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent for the game character the player controls
	Comments: The player is the mouse "grabber" in this game. 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PlayerAgent.h"
#include "IndieLibManager.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "Camera2D.h"
#include <sstream>

//Update object status
void PlayerAgent::UpdateState(float dt)
{
	//IF - Agent is active
	if(!mActive)
		return;

	//Update control delay variable
	if(mControlDelay >= 0.0f)
		mControlDelay -= dt;

	//IF - Exists sprite
	if(mParams.sprite.gfxentity)
	{	
		_updatePosition();
		
	}//IF
	
	//Update position of anchor point in mousejoint
	if(mObjectDragged)
	{
		b2Joint* joint (mPhysicsMgr->GetJoint(PhysicsManager::MouseJointName));
		if( joint && joint->GetType() == e_mouseJoint)
		{
			b2MouseJoint* mousejoint (static_cast<b2MouseJoint*>(joint));
			if(mousejoint)
			{
				//Mouse joint found, so update target anchor point
				float unitx = static_cast<float32>(mPosition.x);
				float unity = static_cast<float32>(mPosition.y);
				mousejoint->SetTarget(b2Vec2(unitx,unity));
			}
		}
			
	}
}

//Process possible collisions
bool PlayerAgent::HandleCollision(const CollisionEventData&)
{	
	bool processed(false);
	return processed;
}

//Process possible events
bool PlayerAgent::HandleEvent(const EventData& data)
{
	bool eventprocessed (false);
	//IF - Mouse position update
	if(data.GetEventType() == Event_NewMousePos)
	{
		const NewMousePosEvent& eventdata = static_cast<const NewMousePosEvent&>(data);
		mMousePosPix = eventdata.GetPosPix();
	}//ELSE - Take and object in mouse position
	else if(data.GetEventType() == Event_TakeObjectCommand && !mObjectDragged)
	{
		//Query for bodies in the mouse place ("units" coordinates)
		b2Vec2 position(static_cast<float32>(mPosition.x),
					 static_cast<float32>(mPosition.y));
		
		b2AABB box;
		box.lowerBound = position - b2Vec2(1.0f,1.0f);
		box.upperBound = position + b2Vec2(1.0f,1.0f);
		std::vector<b2Body*> foundbodies = mPhysicsMgr->QueryforBodies(box);
		//If a body was found, create an anchor point there (mousejoint) to move it
		if(!foundbodies.empty())
		{	
			std::vector<b2Body*>::iterator itr (foundbodies.begin());
			//LOOP - Check if there is "pickable" body
			while(!mObjectDragged && itr != foundbodies.end())
			{
				IAgent* relagent (static_cast<IAgent*>((*itr)->GetUserData()));
				if(relagent->GetType() != AI
				   &&
				   relagent->GetType() != PLAYER)
				{
					//Create the mouse joint definition
					b2MouseJointDef md;
					md.body2 = (*itr);
					md.target = position;
					md.maxForce = mParams.grabstrength;
					mPhysicsMgr->CreateMouseJoint(&md);
					//Wake up body to move it if it was idle
					(*itr)->WakeUp();
					mObjectDragged = true;

					//Update animation
					if(mParams.sprite.gfxentity->GetAnimation())
					{
						mParams.sprite.gfxentity->SetSequence(1);
					}
				}

				++itr;
			}//LOOP END
		}		
	}//ELSE - Release an object in hold
	else if(data.GetEventType() == Event_ReleaseObjectCommand && mObjectDragged)
	{
		mPhysicsMgr->DestroyMouseJoint();	
		mObjectDragged = false;
		//Update animation
		if(mParams.sprite.gfxentity->GetAnimation())
		{
			mParams.sprite.gfxentity->SetSequence(0);
		}
	}//ELSE - Joint destroyed event
	else if(data.GetEventType() == Event_DestroyedJoint)
	{
		const DestroyedJointEvent& theevent (static_cast<const DestroyedJointEvent&>(data));
		b2Joint* affectedjoint(theevent.GetEventData().joint);
			
		//IF - Somehow, mousejoint destroyed
		if(affectedjoint->GetType() == e_mouseJoint && mObjectDragged)
		{
			//As is user didnt click
			mObjectDragged = false;
			//Update animation
			if(mParams.sprite.gfxentity->GetAnimation())
			{
				mParams.sprite.gfxentity->SetSequence(0);
			}
		}//IF
	}	
			
	return eventprocessed;
}

//Handle out of limits
void PlayerAgent::HandleOutOfLimits(const OutOfLimitsEventData&) 
{ 
	//Nothing
}					

//Create from params
void PlayerAgent::Create( const GameAgentPar *params)	
{
	//Agent params should be of type
	//Copy parameters from creation 
	assert(params->type == PLAYER);
	
	//Cast down params hierarchy to this type of params
	const PlayerAgentPar *pagentparams = static_cast<const PlayerAgentPar*>(params);
	//Copy all parameters
	mParams = *pagentparams;
	
	//Finally, update internal tracking
	mActive = true;
}

//Destroy agent
void PlayerAgent::Destroy()
{
	//Finally, update internal tracking
	mActive = false;
}

//Init
void PlayerAgent::_init()
{
	//Init internal variables
	mGlobalScale = SingletonIndieLib::Instance()->GetGeneralScale();
	mResY = static_cast<float>(SingletonIndieLib::Instance()->Window->GetHeight());
	mResX = static_cast<float>(SingletonIndieLib::Instance()->Window->GetWidth());
}


//Release internal resources
void PlayerAgent::_release()
{
	//Deregister graphics entity from indielib
	if(mParams.sprite.gfxentity)
	{
		SingletonIndieLib::Instance()->Entity2dManager->Delete(mParams.sprite.gfxentity.get());
	}
}

//Display position of sprite (mouse)
void PlayerAgent::_updatePosition()
{
	//-------Update position in world coordinates----------------------------------
	//Get necessary values from camera "general"
	float zoom = SingletonIndieLib::Instance()->GetCamera("General")->GetZoom(); //Zoom of "general" camera
	Vector2 camerapos = SingletonIndieLib::Instance()->GetCamera("General")->GetPosition();		//Position of "general" camera
	//Transform position of mouse to world coordinates
	mPosition.x = (mMousePosPix.x) / (mGlobalScale * zoom);
	mPosition.y = (mResY - mMousePosPix.y) / (mGlobalScale * zoom);

	//Transform position to screen center (in world coordinates)
	Vector2 offset(mResX / (mGlobalScale * zoom),mResY / (mGlobalScale * zoom)); 
	mPosition -= offset/2; 
	//Offset position to real camera position
	mPosition += camerapos;	

	//--------------Update mouse display position---------------------------------
	if(mParams.sprite.gfxentity)
		mParams.sprite.gfxentity->SetPosition(static_cast<float>(mMousePosPix.x - mResX/2),
												 static_cast<float>(mMousePosPix.y + mResY/2),
												 0);
}