/*
	Filename: BodyChain.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class encapsulating info of a body chain
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "BodyChain.h"
#include "PhysicsManager.h"
#include "IndieLibManager.h"
#include "IAgent.h"
#include "Math.h"
#include <sstream>

//Update control
void BodyChain::Update(float)
{

	if(mDestroyed)
		return;

	//Control how many agents are attached. If surpasses limit, bodychain breaks
	//IF - Agents can attach
	if(mAgentsCanAttach)
	{
		//IF - Max agents attached surpassed
		if(mMaxAttached < mNumAttached && !mBroken)
		{
			//Break the first joint
			std::string todestroyjoint (*(mJointsList.begin()));
			mJointsList.pop_front();

			//Destroy the joint
			mPhysicsMgr->DestroyJoint(todestroyjoint);

			mBroken = true;
		}//IF
	}//IF

	//GFX Update
	if(!mSpriteList.empty())
		_updateGFX();
}

void BodyChain::IncrementAttached() 
{ 
	if(!mAgentsCanAttach) 
		return; 
		
	mNumAttached++; 
}
	
void BodyChain::DecrementAttached() 
{ 
	if(!mAgentsCanAttach)
		return;
	
	mNumAttached--; 
	//Min control
	if(mNumAttached < 0) 
		mNumAttached = 0; 
}

//Set the reference for this agent
void BodyChain::SetAgentReference(IAgent* agent)
{
	BodiesListIterator itr;
	//LOOP - ALL BODIES
	for(itr = mBodiesList.begin(); itr != mBodiesList.end(); ++itr)
	{
		(*itr)->SetUserData(agent);
	}//LOOP END
}

//Is body in chain?
bool BodyChain::IsBodyInChain(b2Body* body)
{
	BodiesListIterator itr;

	//LOOP - SEARCH BODY
	for(itr = mBodiesList.begin(); itr != mBodiesList.end(); ++itr)
	{	
		if( (*itr) == body)
			return true;
	}//LOOP END

	//Not found
	return false;
}

//Destroy body if it is in chain
bool BodyChain::DestroyBodyIfInChain(b2Body* body)
{
	if(mDestroyed)
		return false;

	BodiesListIterator itr;
	//LOOP - SEARCH BODY
	for(itr = mBodiesList.begin(); itr != mBodiesList.end(); ++itr)
	{	
		if( (*itr) == body)
		{
			//Remember to unreference related joints!
			mPhysicsMgr->DestroyBody(body);

			//Search for related joints to this body
			BodyChain::JointsListIterator jointitr (mJointsList.begin());
			//LOOP - Unreference body-related joints (Joints are destroyed automatically)
			while(jointitr != mJointsList.end())
			{
				b2Joint* joint (mPhysicsMgr->GetJoint(*jointitr));
				
				if(!joint
					||
					joint->GetBody1() == body
					||
				   joint->GetBody2() == body)
				{
					jointitr = mJointsList.erase(jointitr);
				}
				else
					++jointitr;
			}//LOOP 
			//Clear body list entry
			mBodiesList.erase(itr);
			return true;
		}
	}//LOOP END

	//Not found
	return false;	
}

void BodyChain::Destroy()
{
	if(mDestroyed)
		return;

	//Just destroy all bodies
	BodiesListIterator itr;
	//LOOP - SEARCH BODY
	for(itr = mBodiesList.begin(); itr != mBodiesList.end(); ++itr)
	{	
		//Remember to unreference related joints!
		mPhysicsMgr->DestroyBody((*itr));
	}//LOOP END

	//No references
	mBodiesList.clear();
	mJointsList.clear();

	//Clear GFX if there are
	SpriteListIterator spitr;
	//LOOP - Delete from Ilib all sprites
	for(spitr = mSpriteList.begin();spitr != mSpriteList.end(); ++spitr)
	{
		//Delete it from IndieLib
		SingletonIndieLib::Instance()->Entity2dManager->Delete((*spitr).gfxentity.get());;
	}//LOOP END

	mSpriteList.clear();

	mDestroyed = true;
}

//Update graphics display (vertex positions of texture)
void BodyChain::_updateGFX()
{

	assert(mSpriteList.size() == mBodiesList.size());

	BodiesListIterator boditr;
	SpriteListIterator spitr;
	b2Vec2 pos;
	b2Mat22 rot;
	Vector2 positionpix;
	//LOOP - all bodies, and update its gfx positions
	for(boditr = mBodiesList.begin(), spitr = mSpriteList.begin();boditr != mBodiesList.end();++boditr,++spitr)
	{
		//Get position and angle of body
		b2XForm bodytransform ((*boditr)->GetXForm());
		
		//Get final position and orientation with local sprite offsets included
		b2Vec2 transoffset(static_cast<float>((*spitr).posoffset.x),
							static_cast<float>((*spitr).posoffset.y));
		b2Mat22 rotoffset(SingletonMath::Instance()->AngleToRadians((*spitr).rotoffset));
		b2XForm spritetransform(transoffset,rotoffset);
		pos = b2Mul(spritetransform,b2Vec2(0,0));  //Apply sprite transform
		pos = b2Mul(bodytransform,pos);  //Apply body transform
		positionpix = SingletonIndieLib::Instance()->FromCoordToPix(Vector2(pos.x,pos.y));
		rot = b2Mul(bodytransform.R,spritetransform.R);
		
		//Put position and angle to sprite (offseted with local translation and rotation)
		(*spitr).gfxentity->SetAngleXYZ(0.0f,0.0f,SingletonMath::Instance()->RadiansToAngle<float>(rot.GetAngle(),true));
		int posz((*spitr).gfxentity->GetPosZ());
		(*spitr).gfxentity->SetPosition(static_cast<float>(positionpix.x),static_cast<float>(positionpix.y),posz);
	}//LOOP END
}

void BodyChain::_release()
{
	Destroy();
}