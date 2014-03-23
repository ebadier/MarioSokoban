#ifndef __SDL_ELEMENTS_H__
#define __SDL_ELEMENTS_H__

#include <map>

struct SDL_Rect;
struct SDL_Surface;

namespace sdl
{
	/**
	*	\brief Handler for SDL_Surface
	*/
	class Surface
	{
	private:
		Surface(const Surface&);
		Surface& operator=(const Surface&);

	public:
		Surface();
		Surface(const std::string& pImgFilePath);
		~Surface();

		/**
		*	\brief Init SDL_Surface with the given image file path.
		*	Throws an exception if image not loaded.
		*/
		void Init(const std::string& pImgFilePath);
		
		/**
		*	\brief Apply Surface on a screen.
		*/
		void ApplyToScreen(SDL_Rect* pPosition, SDL_Surface* pScreen);

	public:
		SDL_Surface*	mSurface; //!< AGGREGATION.
	};


	typedef int SurfaceID;

	/**
	*	\brief Surface container.
	*/
	class Surfaces
	{
	private:
		Surfaces(const Surfaces&);
		Surfaces& operator=(const Surfaces&);

	public:
		Surfaces();
		~Surfaces();

		/**
		*	\brief Create a new Surface with pSurfaceID.
		*	You are responsible to give a unique ID.
		*	Throws exception if a Surface with pSurfaceID already exist.
		*/
		void CreateSurface(SurfaceID pSurfaceID, const std::string& pImgFilePath);
		
		/**
		*	\brief Get Surface corresponding to the given ID.
		*	Throws exception if pSurfaceID not found.
		*/
		Surface* GetSurface(SurfaceID pSurfaceID);

	private:
		typedef std::map< SurfaceID, Surface* > MapSurfaces;
		MapSurfaces	mMapSurfaces; //!< AGGREGATIONS.
	};
}

#endif // __SDL_ELEMENTS_H__
