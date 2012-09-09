/*
	Filename: BodyChain.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class encapsulating info of a body chain
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _BODYCHAIN
#define _BODYCHAIN

//Library dependencies	
#include <string>
#include <list>
#include <boost\shared_ptr.hpp>  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...
//Class dependencies
#include "Shared_Resources.h"
#include "GFXDefs.h"
#include "Vector2.h"

//Forward declarations
class b2Body;
class b2Joint;
class IAgent;

//A shared pointer of this class
class BodyChain;
typedef boost::shared_ptr<BodyChain> BodyChainPointer;

class BodyChain
{
friend class BodyChainBuilder;  //Builder is friend to access internals while construction of object
public:
	//Map of string->bodychain
	typedef std::list<BodyChainPointer> BodyChainList;
	typedef BodyChainList::iterator BodyChainListIterator;
	//List of contained elements
	typedef std::list<b2Body*> BodiesList;
	typedef BodiesList::iterator BodiesListIterator;
	typedef std::list<std::string> JointsList;
	typedef JointsList::iterator JointsListIterator;
	typedef std::list<ContainedSprite> SpriteList;
	typedef SpriteList::iterator SpriteListIterator;
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BodyChain(PhysicsManagerPointer physicsmgr):
	mPhysicsMgr(physicsmgr),
	mChainId(""),
	mPreviousBodyLink(NULL),
	mControlActive(false),
	mAgentsCanAttach(false),
	mNumAttached(0),
	mMaxAttached(1),
	mBroken(false),
	mDestroyed(false)
	{
	}
	~BodyChain()
	{
		_release();
	}
	//----- GET/SET FUNCTIONS -----
	std::string GetId() { return mChainId; }
	void SetPreviousBodyChainId(const std::string& previd) { mPreviousLinkId = previd; }
	std::string GetPreviousBodyChainId() { return mPreviousLinkId; }
	void SetNextBodyChainId(const std::string& nextid) { mNextLinkId = nextid; }
	std::string GetNextBodyChainId() { return mNextLinkId; }
	b2Body* GetPrevBodyLink() { return mPreviousBodyLink; }
	void SetAgentReference(IAgent* agent);
	void SetBroken(bool broken) { mBroken = broken; }
	//Access to bodies data (read only)
	BodiesList::const_iterator GetFirstBodyIterator() { return mBodiesList.begin(); }
	BodiesList::const_iterator GetLastBodyIterator() { return mBodiesList.end(); }
	JointsList::const_iterator GetFirstJointIterator() { return mJointsList.begin(); }
	JointsList::const_iterator GetLastJointIterator() { return mJointsList.end(); }
	//Number of attached agents
	int GetNumAttachedAgents() { return mNumAttached; }
	bool AgentsCanAttach() { return mAgentsCanAttach; }
	bool IsBroken() { return mBroken; }
	//----- OTHER FUNCTIONS -----
	void EnableControl(bool enable) { mControlActive = enable; }
	void Update(float dt);     //Update control
	bool IsBodyInChain(b2Body* body);
	bool DestroyBodyIfInChain(b2Body* body);
	void Destroy();

	void IncrementAttached();
	void DecrementAttached();

private:
	//----- INTERNAL VARIABLES -----
	PhysicsManagerPointer mPhysicsMgr;   //Pointer to physics manager

	std::string mChainId;     //Internal data
	BodiesList mBodiesList;
	JointsList mJointsList;
	SpriteList mSpriteList;
	std::string mPreviousLinkId;
	std::string mNextLinkId;
	b2Body* mPreviousBodyLink;
	
	bool mDestroyed;		//Destroyed tracking
	bool mControlActive;	//Controller active tracking
	bool mBroken;			//Broken tracking
	bool mAgentsCanAttach;  //Tracks if an agent can attach to it
	int mNumAttached;		//Number of attached agents (if they can)
	int mMaxAttached;       //Number of max agents attached to break a joint
	//----- INTERNAL FUNCTIONS -----
	void _updateGFX();
	void _release();
};

#endif