/*
	Filename: AIAgent.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Agent controlled by AI
	Comments: Internally, it holds a state machine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
/*	TODO:
		- ADD SAVE AND LOAD CAPABILITIES; NOW IT IS NOT TOP PRIORITY FOR MY GAMES...
// TODO: ADDED TEMP REFERENCE TO PHYSICS MANAGER BECAUSE OF HACKS IN FORCES!! NEED TO ADD A FORCE CONTROLLER INSIDE
*/

#ifndef _AIAGENT
#define _AIAGENT

//Library dependencies
#include <assert.h>
//Classes dependencies
#include "IAgent.h"
#include "Math.h"
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "AnimationController.h"
#include "StateMachine.h"

//Forward declarations
class b2Body;
class b2Joint;

//Definitions
//Properties to contain from agent - Inherited
struct AIAgentPar : public GameAgentPar
{
		AIAgentPar():
		GameAgentPar(),
		agentAI(""),
		physicbody(NULL),
		sensorshape(NULL),
		maxspeedx(1.0f),
		maxspeedy(1.0f),
		maxsteerforce(1.0f)
		{
			//Determine type
			type = AI;
		}
		
		//Agent AI name - for creation of state machine
		std::string agentAI;
		ContainedSprite sprite;  //The graphics entity related
		b2Body* physicbody;				//Physics body related
		b2Shape* sensorshape;		//Shape to sense collisions
		
		//Movement params
		float maxspeedx;
		float maxspeedy;
		float maxsteerforce;

		//Assignment operator for fast copying
		AIAgentPar& operator=(const AIAgentPar &newparams)
		{
			//Deep copy of elements in struct
			agentAI = newparams.agentAI;
			sprite = newparams.sprite;
			physicbody = newparams.physicbody;
			sensorshape = newparams.sensorshape;
			maxspeedx = newparams.maxspeedx;
			maxspeedy = newparams.maxspeedy;
			maxsteerforce = newparams.maxsteerforce;
			return (*this);
		}
};

//Forward declarations
class PhysicsManager;  //Physics manager

class AIAgent : public IAgent
{

public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AIAgent(const std::string& id, PhysicsManagerPointer physicsptr):
    IAgent(id),
    mPhysicsMgr(physicsptr),
	mStateMachine(NULL),
	mActive(false),
	mWorldGroundCollisions(0),
	mStateChange(false),
	mOutOfLimits(false)
	{
	}
	virtual ~AIAgent()
	{
		//Release
		_release();
	}
	//----- VALUES GET/SET ---------------
	virtual AgentType GetType() { return mParams.type; }						//Get type of agent
	virtual bool IsAlive()  { return mActive; }             //Get if agent was destroyed
	virtual Vector2 GetPosition() {return mParams.position; }			//Get Position of agent
	virtual float GetRotation() {return mParams.rotation; }		//Get rotation of agent
	//State machine
	StateMachine<AIAgent>* GetStateMachine() { return mStateMachine;} 
	//Internal variables access
	AnimationControllerPointer GetAnimationController() { return mAnimController; }
	b2Body* GetPhysicalBody() { return mParams.physicbody; }
	//----- OTHER FUNCTIONS --------------
	virtual void UpdateState(float dt);								//Update object status
	virtual bool HandleCollision(const CollisionEventData& data);	//Process possible collisions
	virtual bool HandleEvent(const EventData& data);				//Process possible events
	virtual void HandleOutOfLimits(const OutOfLimitsEventData& data);	//Handle out of limits
	virtual void Create( const GameAgentPar *params);				//Create from params
	virtual void Destroy();		//Destroy

	void ChangeState(const std::string& newstatename);   //State machine state change

	//Movement control
	void Jump(const Vector2& direction, float strength);
	void Jump(const Vector2& targetpoint);
	void AttachToPoint(const Vector2& point, b2Body* bodytoattach);
	void DetachFromPoint();

	//Free control - DIRECT MANIPULATION
	void ApplyForce(const Vector2& direction, float strength);
	void ChangePosition(const Vector2& point, float angle);

protected:
	//---- INTERNAL VARIABLES ----
	AIAgentPar mParams;						//All parameters needed to create the agent
	StateMachine<AIAgent>* mStateMachine;	//Internal state machine (depends on type of agent)
	bool mActive;							//Internal "active" tracking
	bool mStateChange;						//Memo of state change request
	bool mOutOfLimits;						//Out of limits memo
	std::string mNextStateRequested;			//Next state requested
	AnimationControllerPointer mAnimController;	//GFX entity animation controller

	PhysicsManagerPointer mPhysicsMgr;
	int mWorldGroundCollisions;     //Number of collisions with static bodies of world

	std::string mAttachJointName;  //Attach joint name
	
	//---- INTERNAL FUNCTIONS ----
	void _createStateMachine( const std::string& agentAI );
	void _release();
};

#endif
