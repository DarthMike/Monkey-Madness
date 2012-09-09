/*
	Filename: PlayerAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: An agent for the game character the player controls
	Comments: The player is the mouse "grabber" in this game. 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- IT WOULD BE NICE TO SEPARATE DRAWING AND LOGIC. FOR NOW IT IS LIKE THAT... ALL IN ONE CLASS...
*/

#ifndef _PLAYERAGENT
#define _PLAYERAGENT

//Library dependencies
#include <list>
//Classes dependencies
#include "IAgent.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"

//Definitions
//Properties to contain from agent - Inherited
struct PlayerAgentPar : public GameAgentPar
{
		PlayerAgentPar():
		GameAgentPar(),
		grabstrength(1.0f)
		{
			//Determine type
			type = PLAYER;
		}
		
		ContainedSprite sprite;
		float grabstrength;
		//Assignment operator for fast copying
		PlayerAgentPar& operator=(const PlayerAgentPar &newparams)
		{
			//Deep copy of elements in struct
			sprite = newparams.sprite;
			grabstrength = newparams.grabstrength;
			return (*this);
		}
};

class PlayerAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PlayerAgent(const std::string& id, PhysicsManagerPointer physicsptr):
	  IAgent(id),
	  mGlobalScale(1.0f),
	  mResY(600.0f),
	  mResX(800.0f),
	  mActive(false),
	  mControlDelay(-1.0f),
	  mPhysicsMgr(physicsptr),
	  mObjectDragged(false)
	{
		_init();
	};
	virtual ~PlayerAgent(){ _release(); }
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }			//Get type of agent
	virtual bool IsAlive()  { return (mActive); }             //Get if agent was destroyed
	virtual Vector2 GetPosition() {return mParams.position; }			//Get Position of agent
	virtual float GetRotation() {return mParams.rotation; }		//Get rotation of agent
	//----- OTHER FUNCTIONS --------------
	//Interface implementations
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);								//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();											//Destroy body

protected:
	//---- INTERNAL VARIABLES ----
	PlayerAgentPar mParams;					//All parameters needed to create the agent
	bool mActive;							//Internal "active" tracking
	float mGlobalScale;						//Global scale to draw entities
	float mResY;							//Resolution of screen in Y axis to draw entities
	float mResX;							//Resolution of screen in X axis to draw entities
	float mControlDelay;					//Timer to delay transitions and disable control by player temporally
	bool mObjectDragged;					//To track if an object is being dragged

	PhysicsManagerPointer mPhysicsMgr;			//Physics manager pointer
	Vector2 mPosition;
	Vector2 mMousePosPix;

	//---- INTERNAL FUNCTIONS ----
	void _init();
	void _release();								//Release internal resorces
	void _updatePosition();							//Update GFX position
};

#endif
