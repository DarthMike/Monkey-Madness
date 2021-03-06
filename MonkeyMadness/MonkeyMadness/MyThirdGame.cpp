/*
	Filename: MyThirdGame.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Main file to game CODENAME: MY THIRD GAME
	Comments: Its my third game, made exclusively to the Gamejolt ROGUE Competition (http://gamejolt.com/articles/gamejolt-rogue-contest/3478/)
	Attribution: 
    License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

//------------------------------INCLUDED LIBRARIES-----------------------------------------------
  //Precompiled headers for VC++ IDE (LIBRARIES COMMONLY USED ARE STORED THERE)
#include <windows.h> //For windows

//------------------------------INCLUDED CLASSES-------------------------------------------------
#include "General_Resources.h"	//General resources for whole program
#include "GameApp.h"			//The game application class
//MEMORY LEAKS DETECTION
#include "LeaksVisualC.h"
//------------------------------GLOBAL DEFINITIONS-----------------------------------------------
//Global variables - Only Singletons and config options
ConfigOptions g_ConfigOptions;  //Global properties of game
//------------------------------PROGRAM FUNCTIONS PROTOTYPES-------------------------------------

//***********************************************************************************************

//***********************************************************************************************

//-----------------------------------------------------------------------------------------------
//-------------------------------APPLICATION-----------------------------------------------------
//-----------------------------------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int)	
{
	//+++++++++++++++++++++++++++++ VARIABLE DECLARATIONS+++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    //++++++++++++++++++++++++++++++++++++++ CODE ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//Leak detections
	//Static detections FROM: http://cgempire.com/forum/tutorials-101/easy-memory-leak-detection-c-c-546.html
	FINDMEMLEAK(-1);

	//Game (edit mode false)!!!
	GameApp MyGame(false);

	//Nested in try-catch, when exception... well, show it to user and finish
	try
	{
		//Store app working path to generate relative paths from there
		g_ConfigOptions.SetupWorkingPath();
		MyGame.Load();  //Init
		MyGame.Run();   //Game loop
	}
	catch(std::exception &e)
	{
		//Show exception message
		::MessageBoxA( NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
}

//-------------------------------APPLICATION END-------------------------------------------------

//-----------------------------------------------------------------------------------------------
//-------------------------------FUNCTIONS-------------------------------------------------------
//-----------------------------------------------------------------------------------------------

