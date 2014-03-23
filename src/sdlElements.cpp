#include "SDL_image.h"

#include "macros.h"

#include "sdlElements.h"

namespace sdl
{
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-----------------------------------------------class Surface-------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	Surface::Surface(): mSurface(NULL)
	{

	}
	
	//-------------------------------------------------------------------------------------------------------------------//
	Surface::Surface(const std::string& pImgFilePath): mSurface(NULL) 
	{
		Init(pImgFilePath);
	}

	//-------------------------------------------------------------------------------------------------------------------//
	Surface::~Surface()
	{
		if(mSurface)
		{
			SDL_FreeSurface(mSurface);
			mSurface = NULL;
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void Surface::Init( const std::string& pImgFilePath )
	{
		mSurface = IMG_Load(pImgFilePath.c_str());
		MTest_T_MSG(NULL == mSurface, "Cannot load image " << pImgFilePath << " !");
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void Surface::ApplyToScreen(SDL_Rect* pPosition, SDL_Surface* pScreen)
	{
		SDL_BlitSurface(mSurface, NULL, pScreen, pPosition);
	}

	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-----------------------------------------------class Surfaces------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	//-------------------------------------------------------------------------------------------------------------------//
	Surfaces::Surfaces(): mMapSurfaces()
	{

	}
	
	//-------------------------------------------------------------------------------------------------------------------//
	Surfaces::~Surfaces()
	{
		MapSurfaces::iterator it = mMapSurfaces.begin();
		for(; it != mMapSurfaces.end(); ++it)
		{
			delete it->second;
			it->second = NULL;
		}
		mMapSurfaces.clear();
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void Surfaces::CreateSurface(SurfaceID pSurfaceID, const std::string& pImgFilePath)
	{
		MTest_T_MSG(mMapSurfaces.find(pSurfaceID) != mMapSurfaces.end(), "Surface with ID " << pSurfaceID << " already exist");
		mMapSurfaces.insert(MapSurfaces::value_type(pSurfaceID, new Surface(pImgFilePath)));
	}

	//-------------------------------------------------------------------------------------------------------------------//
	Surface* Surfaces::GetSurface(SurfaceID pSurfaceID)
	{
		MapSurfaces::const_iterator found = mMapSurfaces.find(pSurfaceID);
		MTest_T_MSG(mMapSurfaces.find(pSurfaceID) == mMapSurfaces.end(), "Surface with ID " << pSurfaceID << " not found !");
		return found->second;
	}

	//-------------------------------------------------------------------------------------------------------------------//
}