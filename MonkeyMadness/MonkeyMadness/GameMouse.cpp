/*
	Filename: GameMouse.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate logic related to mouse input in game screen
	Comments: It depends of IndieLib, as it will be library used for input
	          The game mouse, can take objects and move them using the physics engine
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "GameMouse.h"
#include "IndieLibManager.h"
#include "GameOverlay.h"
#include "MMGame.h"
#include "PhysicsManager.h"
#include "GameEventManager.h"
#include "GameEvents.h"
#include "Camera2D.h"

//Update method
void GameMouse::Update(float)
{
	//Get input and physics manager
	IND_Input* input = SingletonIndieLib::Instance()->Input;
	
#ifdef _DEBUGGING
	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELUP ))
	{
		Camera2DPointer camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(true);
	}

	if(input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN ))
	{
		Camera2DPointer  camera = SingletonIndieLib::Instance()->GetCamera("General");
		camera->Zoom(false);
	}
#endif
	//Update mouse position
	mPositionPix.x = static_cast<float>(input->GetMouseX());
	mPositionPix.y = static_cast<float>(input->GetMouseY());

	//Commands to take and drop bodies
	if(input->OnMouseButtonPress(IND_MBUTTON_LEFT) && !mDragging)
	{
		//Track dragging
		SingletonGameEventMgr::Instance()->QueueEvent(
		                             EventDataPointer(new EventData(Event_TakeObjectCommand))
										 );
		mDragging = true;
	}
		
	if(input->OnMouseButtonRelease(IND_MBUTTON_LEFT) && mDragging)
	{
		//Track back not dragging
		SingletonGameEventMgr::Instance()->QueueEvent(
		                             EventDataPointer(new EventData(Event_ReleaseObjectCommand))
										 );
		mDragging = false;	
	}

	//Send event with new position
	NewMousePosInfo newposinfo(mPositionPix);

	SingletonGameEventMgr::Instance()->QueueEvent(
		                             EventDataPointer(new NewMousePosEvent(Event_NewMousePos,newposinfo))
										 );
}
	
//Render necessary elements
void GameMouse::Render()
{
	//Render mouse is made by entities...
}