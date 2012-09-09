/*
	Filename: BodychainBuilder.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which creates a series of bodies connected in a chain-like fashion by rev. joints
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
	TODO:
		- 2 STAGE BUILDING: FIRST LOAD FROM XML, (STORING PARAMETERS OF CREATION)
		                    SECONDLY CREATE A NEW OBJECT WITH THE PARAMETERS LOADED. 
*/
#ifndef _BODYCHAINBUILDER
#define _BODYCHAINBUILDER

#include <string>
//Class dependencies
#include "XMLParser.h"
#include "LogManager.h"
#include "GenericException.h"
#include "BodyChain.h"
#include "Shared_Resources.h"

//Forward declarations
class MMGame;

class BodyChainBuilder
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	BodyChainBuilder(MMGame* game):
	mGame(game)
	{
	}
	~BodyChainBuilder()
	{
	}
	//----- GET/SET FUNCTIONS -----
	BodyChainPointer GetCreatedBodyChain() { return mBodyChain; }  //Returns A COPY of the created body chain
	//----- OTHER FUNCTIONS -----
	void LoadBodyChain(const ticpp::Element* xmlelement);   //Read a part of xml info and create an object
protected:
	//----- INTERNAL VARIABLES -----
	BodyChainPointer mBodyChain;   //A pointer to created bodychain
	MMGame* mGame;
	//----- INTERNAL FUNCTIONS -----
	bool _getVerticesData(const std::string& verticesdata,float &xvalue, float &yvalue, int vertex);
	void _buildBodyChainSprites(ticpp::Element* spriteelem, float angle);  //Build all bodychain sprites needed
};

#endif