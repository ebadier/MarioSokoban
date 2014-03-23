#ifndef __SDL_APPLICATION_H__
#define __SDL_APPLICATION_H__

#include "SDL.h"

class TinyXmlElement;

namespace sdl
{
	/**
	*	\brief A SDL Application.
	*/
	class Application
	{
	public:
		Application();
		virtual ~Application();
		/**
		*	\brief Init SDL Application from Xml.
		*	Throws an Exception if failed.
		*/
		void Init(TinyXmlElement* pXml);

	protected:
		SDL_Surface*	mScreen; //!< App screen (AGGREGATION).
		SDL_Event		mEvent; //!< App Event listener.
		bool			mIsItTimeToClose; //!< close App ?
		SDL_Joystick*	mJoystick; //!< Joystick (HANDLE).
	};
}

#endif // __SDL_APPLICATION_H__
