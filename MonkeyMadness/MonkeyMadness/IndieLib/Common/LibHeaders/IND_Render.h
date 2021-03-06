/*****************************************************************************************
/* File: IND_Render.h
/* Desc: Render class using Direct3D
/*
/* Divided in:
/* - IND_Render.cpp
/* - RenderTransform2d.cpp
/* - RenderTransform3d.cpp
/* - RenderTransformCommon.cpp
/* - RenderObject2d.cpp
/* - RenderObject3d.cpp
/* - RenderPrimitive2d.cpp
/* - RenderText2d.cpp	
/* - RenderCollision2d.cpp
/*****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier L�pez L�pez (info@pixelartgames.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/


#ifndef _IND_RENDER_H_
#define _IND_RENDER_H_

// ----- Includes -----

#include "IND_Window.h"
#include "IND_FontManager.h"
#include "IND_SurfaceManager.h"
#include "IND_3dMeshManager.h"
#include "IND_AnimationManager.h"
#include "IND_FontManager.h"
#include "IND_Timer.h"
#include "IND_Font.h"
#include "IND_3dMesh.h"
#include "IND_Animation.h"
#include "IND_Camera2d.h"
#include "IND_Camera3d.h"

// ----- Libs -----

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#include <d3d9.h> 
#include <d3dx9.h>

// ----- Defines -----

#define MAX_PIXELS 2048


// --------------------------------------------------------------------------------
//									  IND_Render
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Render IND_Render
\ingroup Main
Render class for setting up the render area, click in ::IND_Render to see all the methods of this class.
*/
/*@{*/	

/*!
The methods of this class are used for preparing the render area, viewport, etc. 

Important: You can use the methods IND_Render::GetDirect3d() and  IND_Render::GetDevice()
in order to get the Direct3d pointers. This can be useful if you want to extend the features of the 
engine using Direct3d directly.
*/		
class DLL_EXP IND_Render
{
public:

	// ----- Init/End -----

	IND_Render		(): mOk(false)	{ }
	~IND_Render		()				{ End(); }


	bool	Init				(IND_Window *pWindow);
	bool	Init				(LPDIRECT3D9 pDirect3d, IDirect3DDevice9 *pD3dDevice);
	bool	Reset				(char *pTitle, int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
	bool	ToggleFullScreen	();
	void	End					();
	bool	IsOk				() { return mOk; }

	// ----- Public methods -----

	inline void BeginScene				();
	inline void EndScene				();
	inline void ShowFpsInWindowTitle	();

	// ----- Viewports and cameras -----

	/** @name Viewport clearing (both for 2d and 3d viewports)
	*   
	*/
	//@{
	inline void	ClearViewPort			(byte pR,				
										 byte pG, 
										 byte pB);
	//@}

	/** @name Viewport and camera 2d
	*   
	*/
	//@{
	inline bool SetViewPort2d			(int pX, 
										 int pY, 
										 int pWidth, 
										 int pHeight);

	inline void SetCamera2d				(IND_Camera2d *pCamera2d);
	//@}

	/** @name Viewport and camera 3d
	*   
	*/
	//@{
	inline bool SetViewPort3d			(int pX, 
										int pY, 
										int pWidth, 
										int pHeight);

	inline void SetCamera3d				(IND_Camera3d *pCamera3d);
	//@}

	// ----- Render Primitive 2d -----

	inline bool SetAntialiasing			(bool pSwitch);

	/** @name Bliting primitives (you can also use IND_Entity2d instead this methods)
	*   
	*/
	//@{
	inline void BlitPixel				(int pX, 
										int pY, 
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);
	
	inline void BlitLine				(int pX1, 
										int pY1,
										int pX2,
										int pY2,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	inline void BlitRectangle			(int pX1, 
										int pY1,
										int pX2,
										int pY2,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	inline void BlitFillRectangle		(int pX1, 
										int pY1,
										int pX2,
										int pY2,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	inline void BlitColoredTriangle		(int pX1, 
										int pY1,
										int pX2,
										int pY2,
										int pX3,
										int pY3,
										byte pR1, byte pG1, byte pB1,
										byte pR2, byte pG2, byte pB2,
										byte pR3, byte pG3, byte pB3, 
										byte pA);
	//MIGUEL MODIFICATION
	inline void BlitTriangleList	 (IND_Point *pTrianglePoints, 
										int pNumPoints,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	inline bool BlitPoly2d				(IND_Point *pPixel, 
										int pNumLines,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	inline bool BlitRegularPoly			(int pX, 
										int pY,
										int pRadius,
										int pN,
										float pAngle,
										byte pR, 
										byte pG, 
										byte pB,
										byte pA);

	//@}


	// ----- Advanced transformations -----

	/** @name Advanced transformations methods (it's preferible to use IND_Entity2d and IND_Entity3d instead)
	*   
	*/
	//@{
	inline void SetTransform2d			(int pX,			
										int  pY,			
										float pAngleX,	
										float pAngleY,	
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										int pAxisCalX,
										int pAxisCalY,
										bool pMirrorX,
										bool pMirrorY,
										int pWidth,
										int pHeight,			
										IND_Matrix *pMatrix);

	inline void SetTransform3d			(float pX,
										float  pY,
										float  pZ,
										float pAngleX,
										float pAngleY,
										float pAngleZ,
										float pScaleX,
										float pScaleY,
										float pScaleZ,
										IND_Matrix *pMatrix);

	inline void SetRainbow2d			(IND_Type pType,
										bool pCull,
										bool pMirrorX,
										bool pMirrorY,
										IND_Filter pFilter,
										byte pR,
										byte pG,
										byte pB,
										byte pA,
										byte pFadeR,
										byte pFadeG,
										byte pFadeB,
										byte pFadeA,
										IND_BlendingType pSo,
										IND_BlendingType pDs);

	inline void SetRainbow3d			(bool pCull,
										bool pFlipNormals,
										IND_Filter pFilter,
										byte pR,
										byte pG,
										byte pB,
										byte pA,
										byte pFadeR,
										byte pFadeG,
										byte pFadeB,
										byte pFadeA,
										IND_BlendingType pSo,
										IND_BlendingType pDs);

	inline void LookAt					(float pEyeX, float pEyeY, float pEyeZ,
										float pLookAtX, float pLookAtY, float pLookAtZ,
										float pUpX, float pUpY, float pUpZ);

	inline void PerspectiveFov			(float pFov, float pAspect, float pNearClippingPlane, float pFarClippingPlane);

	inline void PerspectiveOrtho		(float pWidth, float pHeight, float pNearClippingPlane, float pFarClippingPlane);
	//@}


	// ----- Render Object 2d -----

	/** @name Advanced 2d bliting methods (it's preferible to use IND_Entity2d together with IND_Entity2dManager instead)
	*   
	*/
	//@{
	inline void BlitSurface				(IND_Surface *pSu);	

	inline void BlitGrid				(IND_Surface *pSu, byte pR, byte pG, byte pB, byte pA);
	
	inline bool BlitRegionSurface		(IND_Surface *pSu,		
										int pX, 
										int pY, 
										int pWidth, 
										int pHeight);

	inline bool BlitWrapSurface			(IND_Surface *pSu,
										int pWidth, 
										int pHeight,
										float pUDisplace,
										float pVDisplace);

	inline int BlitAnimation			(IND_Animation *pAn,
										int pSequence, 
										int pX, int pY,
										int pWidth, int pHeight,
										bool pToggleWrap,
										float pUDisplace,
										float pVDisplace);

	// ------ Render Text 2d -----
	//MIGUEL MODIFICATION: Scale enabled for fonts!
	inline void BlitText  (IND_Font *pFo, 
								  char *pText, 
								  int pX, 
								  int pY, 
								  int pOffset,
								  int pLineSpacing,
								  float pScaleX,  //MIGUEL: New param
								  float pScaleY,  //MIGUEL: New param
								  byte pR,
								  byte pG,
								  byte pB,
								  byte pA,
								  byte pFadeR,
								  byte pFadeG,
								  byte pFadeB,
								  byte pFadeA,
								  IND_Filter pLinearFilter,
								  IND_BlendingType pSo,
								  IND_BlendingType pDs,
								  IND_Align pAlign);

	//@}


	// ----- Render Object 3d -----

	/** @name Advanced 3d bliting methods (it's preferible to use IND_Entity3d together with  IND_Entity3dManager instead)
	*   
	*/
	//@{
	inline void Blit3dMesh				(IND_3dMesh *p3dMesh);
	inline void Set3dMeshSequence		(IND_3dMesh *p3dMesh, unsigned int pIndex);
	//@}

	// ----- Atributos -----

	/** @name Gets
	*   
	*/
	//@{
	//! This function returns the x position of the actual viewport
	int GetViewPortX			()		{ return mInfo.mViewPortX;			}
	//! This function returns the y position of the actual viewport
	int GetViewPortY			()		{ return mInfo.mViewPortY;			}
	//! This function returns the width position of the actual viewport
	int GetViewPortWidth		()		{ return mInfo.mViewPortWidth;		}
	//! This function returns the width position of the actual viewport
	int GetViewPortHeight		()		{ return mInfo.mViewPortHeight;		}
	//! This function returns the actual version of Direct3d that is using IndieLib.
	char *GetVersion			()		{ return mInfo.mVersion;			}
	//! This function returns the name of the graphic card vendor.
	char *GetVendor				()		{ return mInfo.mVendor;				}
	//!	This function returns the name of the graphic card.
	char *GetRenderer			()		{ return mInfo.mRenderer;			}
	//! This function returns the maximum texture size allowed by the graphic card.
	int  GetMaxTextureSize		()		{ return mInfo.mMaxTextureSize;		}
	//! This function returns the pointer to Direct3d.
	LPDIRECT3D9 GetDirect3d		()		{ return mInfo.mDirect3d;			}
	//! This function returns the pointer to the Direct3d device.
	IDirect3DDevice9 *GetDevice	()		{ return mInfo.mDevice;				}
	//! This function returns the actual fps (frames per second) in a string of chars. 
	//! @param _String IN-OUT buffer capable to hold string representation of integer FPS value
	void *GetFpsString			(char* _String)		{ _itoa (mLastFps, _String, 10); return _String;	}
	//! This function returns the actual fps (frames per second) in an int
	int GetFpsInt				()		{ return mLastFps;					}
	//! This function returns a pointer to the IND_Window object where the render has been created
	IND_Window *GetWindow		()		{ return mWindow;					}
	//! This function returns in miliseconds the time that took the previous frame. 
	/*!
	It is very useful to indicate transformations along the timeline.

	For example, making Position_X += Speed * Render.GetFrameTime() / 1000.0f we would get 
	one x position that would go moving along the time with a given speed.
	*/
	float GetFrameTime			()		{ return mLast;						}
	//@}

	char *GetNumRenderedObjectsString		()		{ char *mString = new char [128]; itoa (mNumRenderedObjects, mString, 10); return mString;			};
	char *GetNumDiscardedObjectsString		()		{ char *mString = new char [128]; itoa (mNumDiscardedObjects, mString, 10); return mString;			};

	int GetNumRenderedObjectsInt			()		{ return mNumRenderedObjects;		};
	int GetNumDiscardedObjectsInt			()		{ return mNumDiscardedObjects;		};

	void ResetNumRenderedObject				()		{ mNumRenderedObjects = 0; }
	void ResetNumDiscardedObjects			()		{ mNumDiscardedObjects = 0; }

private:

	// ----- Private methods -----
	
	bool mOk;

	// ----- Objects -----

	IND_Window *mWindow;
	HWND mWnd;

	// ----- Vars -----

	int mNumRenderedObjects;
	int mNumDiscardedObjects;

	struct StrucutFrustumPlane
	{
		D3DXVECTOR3 mNormal;
		float mDistance;

		inline float DistanceToPoint (D3DXVECTOR3 &pPnt)
		{
			return D3DXVec3Dot (&mNormal, &pPnt) + mDistance;
		}

		inline void Normalise()
		{
			float denom = 1 / sqrt ((mNormal.x * mNormal.x) + (mNormal.y * mNormal.y) +  (mNormal.z * mNormal.z));
			mNormal.x = mNormal.x * denom;
			mNormal.y = mNormal.y * denom;
			mNormal.z = mNormal.z * denom;
			mDistance = mDistance * denom;
		}
	};

	StrucutFrustumPlane mFrustumPlanes[6];

	D3DDISPLAYMODE mDisplayMode;					// Display mode
	D3DPRESENT_PARAMETERS mPresentParameters;		// Presentation parameters

	// Timer
	float mLastTime;
	float mLast;


	// Fps
	int mFpsCounter;
	float mCurrentTimeFps;
	float mLastTimeFps;
	int mLastFps;

	// ----- Primitives vertices -----

	// Temporal buffer of pixels for drawing primitives
	PIXEL mPixels [MAX_PIXELS];

	// ----- Vertex array -----

	// Temporal buffer of vertices for drawing regions of an IND_Surface
	CUSTOMVERTEX2D mVertices2d [MAX_PIXELS];

	// ----- Info -----

	struct infoStruct
	{
		int mFBWidth;					
		int mFBHeight;					
		int mViewPortX;					
		int mViewPortY;					
		int mViewPortWidth;				
		int mViewPortHeight;		
		bool mAntialiasing;			
		char mVersion [1024] ;					
		char mVendor [1024];					
		char mRenderer [1024];				
		int mMaxTextureSize;		
		DWORD mVertexShaderVersion;	
		DWORD mPixelShaderVersion;
		bool mSoftwareVertexProcessing;	
		LPDIRECT3D9	mDirect3d;
		IDirect3DDevice9 *mDevice;
	};
	struct infoStruct mInfo;

	// ----- Private methods -----
	
	bool Direct3DInit							(int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);
	bool Direct3dReset							(int pWidth, int pHeight, int pBpp, bool pVsync, bool pFullscreen);

	bool CreateRender							(IND_Window *pWindow);

	void GetInfo								();
	void WriteInfo								();

	inline D3DBLEND GetD3DBlendingType			(IND_BlendingType pBlendingType);
	inline D3DTEXTUREFILTERTYPE					GetD3DFilter (IND_Filter pFilter);
	
	inline void	FillVertex2d					(CUSTOMVERTEX2D *pVertex2d, 
												int pX, 
												int pY,
												float pU,
												float pV);

	void ResetTimer								();

	// ----- Render Object 3d -----

	void FrameMove								(IND_3dMesh *p3dMesh, float elapsedTime, const D3DXMATRIX *matWorld);
	void UpdateFrameMatrices					(const D3DXFRAME *frameBase, const D3DXMATRIX *parentMatrix);
	void DrawFrame								(LPD3DXFRAME frame);
	void DrawMeshContainer						(LPD3DXMESHCONTAINER meshContainerBase, LPD3DXFRAME frameBase);

	// ----- Primitives -----

	inline bool	IsBlockRightOfViewPort			(D3DXVECTOR4 pP1, D3DXVECTOR4 pP2, D3DXVECTOR4 pP3, D3DXVECTOR4 pP4);
	inline bool	IsBlockLeftOfViewPort			(D3DXVECTOR4 pP1, D3DXVECTOR4 pP2, D3DXVECTOR4 pP3, D3DXVECTOR4 pP4);
	inline bool	IsBlockDownOfViewPort			(D3DXVECTOR4 pP1, D3DXVECTOR4 pP2, D3DXVECTOR4 pP3, D3DXVECTOR4 pP4);
	inline bool	IsBlockUpOfViewPort				(D3DXVECTOR4 pP1, D3DXVECTOR4 pP2, D3DXVECTOR4 pP3, D3DXVECTOR4 pP4);
	
	inline void FillPixel						(PIXEL *pPixel, 
												int pX, 
												int pY,
												byte pR, 
												byte pG, 
												byte pB);
	//MIGUEL MODIFICATION
	inline void FillPixel						(PIXEL *pPixel, 
												int pX, 
												int pY,
												int pZ,
												byte pR, 
												byte pG, 
												byte pB);

	inline void SetForPrimitive					(byte pA);

	// ------ Fonts -----

	inline int GetLongInPixels					(IND_Font *pFo, char *pText, int pPos, int pOffset);

	inline void SetTranslation					(int pX, int pY, D3DXMATRIX *pMatWorld, D3DXMATRIX *pMatTraslation);

	// ----- Grid -----

	inline void BlitGridLine					(int pPosX1, int pPosY1, int pPosX2, int pPosY2,  byte pR, byte pG, byte pB, byte pA, D3DXMATRIX pWorldMatrix);
	inline void BlitGridQuad					(int pAx, int pAy,
												int pBx, int pBy,
												int pCx, int pCy,
												int pDx, int pDy,
												byte pR, byte pG, byte pB, byte pA,
												D3DXMATRIX pWorldMatrix);

	// ----- Collisions  -----

	inline void BlitCollisionCircle				(int pPosX, int pPosY, int pRadius, float pScale, byte pR, byte pG, byte pB, byte pA, IND_Matrix pWorldMatrix);
	inline void BlitCollisionLine				(int pPosX1, int pPosY1, int pPosX2, int pPosY2,  byte pR, byte pG, byte pB, byte pA, IND_Matrix pIndWorldMatrix);

	inline bool	IsTriangleToTriangleCollision	(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2);
	inline bool	IsCircleToCircleCollision		(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, float pScale1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2, float pScale2);
	inline bool	IsCircleToTriangleCollision		(BOUNDING_COLLISION *pB1, IND_Matrix pMat1, float pScale1, BOUNDING_COLLISION *pB2, IND_Matrix pMat2);


	inline bool IsTriangleToTriangleCollision	(D3DXVECTOR2 pA1, 
												D3DXVECTOR2 pB1,
												D3DXVECTOR2 pC1,
												D3DXVECTOR2 pA2,
												D3DXVECTOR2 pB2,
												D3DXVECTOR2 pC2);

	inline bool IsCircleToCircleCollision		(D3DXVECTOR2 pP1, int pRadius1,
												D3DXVECTOR2 pP2, int pRadius2);

	inline bool IsCircleToTriangleCollision		(D3DXVECTOR2 pPCenter, int pRadius1, 
												D3DXVECTOR2 a2,
												D3DXVECTOR2 b2,
												D3DXVECTOR2 c2);

	inline double PointToLineDistance			(D3DXVECTOR2 pA, D3DXVECTOR2 pB, D3DXVECTOR2 pC, bool pIsSegment);
	inline double Distance						(D3DXVECTOR2 pA, D3DXVECTOR2 pB);
	inline int	Cross3							(D3DXVECTOR2 pA, D3DXVECTOR2 pB, D3DXVECTOR2 pC);
	inline int	Dot3							(D3DXVECTOR2 pA, D3DXVECTOR2 pB, D3DXVECTOR2 pC);

	inline bool IsPointInsideTriangle			(D3DXVECTOR2 p, 
												D3DXVECTOR2 a, 
												D3DXVECTOR2 b, 
												D3DXVECTOR2 c);

	inline bool IsSegmentIntersection			(D3DXVECTOR2 a, 
												D3DXVECTOR2 b, 
												D3DXVECTOR2 c, 
												D3DXVECTOR2 d);

	inline void GetD3DMatrix					(IND_Matrix pMatrix, D3DXMATRIX *pD3DMatrix);

	// ----- Culling -----

	inline void	CalculeFrustumPlanes			();
	inline WORD CullFrustumBox					(const D3DXVECTOR3 &pAABBMin, const D3DXVECTOR3 &pAABBMax);
	inline void CalculateBoundingRectangle		(D3DXVECTOR3 *mP1, D3DXVECTOR3 *mP2, D3DXVECTOR3 *mP3, D3DXVECTOR3 *mP4);
	inline void MinAndMax4						(int p1, 
												int p2, 
												int p3, 
												int p4,
												int *pMax,
												int *pMin);
	inline void Transform4Vertices				(float pX1, float pY1,
												 float pX2, float pY2,
												 float pX3, float pY3,
												 float pX4, float pY4,
												 D3DXVECTOR4 *mP1Res,
												 D3DXVECTOR4 *mP2Res,
												 D3DXVECTOR4 *mP3Res,
												 D3DXVECTOR4 *mP4Res);
	// ----- Init / End -----

	void DestroyD3DWindow						();
	void InitAttribRender						();
	void InitVars								();
	void FreeVars								();

	// ----- Friends -----

	friend class IND_Entity2dManager;
	friend class IND_Input;
};
/*@}*/

#endif // _IND_RENDER_H_
