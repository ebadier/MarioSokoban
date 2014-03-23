#include "SDL_image.h"

#include "tinyxml/tinyxmlHlp.h"

#include "macros.h"

#include "sdlApplication.h"

namespace sdl
{
	//-------------------------------------------------------------------------------------------------------------------//
	Application::Application(): mScreen(NULL), mEvent(), mIsItTimeToClose(false), mJoystick(NULL)
	{
		// On initialise la SDL.
		MTest_T_MSG (SDL_Init(SDL_INIT_EVERYTHING) == -1, "SDL Initialization Error : " << SDL_GetError());

		// Log pour les Joysticks.
		MOut(SDL_NumJoysticks() << " joysticks connected");
		for (int i = 0; i < SDL_NumJoysticks() ; ++i)
		{
			MOut("Joystick_" << i << " : " << SDL_JoystickName(i));
		}

		if (SDL_NumJoysticks() > 0)
		{
			// On active la gestion des evenements des Joysticks.
			SDL_JoystickEventState(SDL_ENABLE);
			// On charge le premier Joystick connecté.
			mJoystick = SDL_JoystickOpen(0);

			MOut("----> " << SDL_JoystickNumButtons(mJoystick) << " buttons");
			MOut("----> " << SDL_JoystickNumAxes(mJoystick) << " axes");
			MOut("----> " << SDL_JoystickNumBalls(mJoystick) << " trackballs");
			MOut("----> " << SDL_JoystickNumHats(mJoystick) << " hats");
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//
	Application::~Application()
	{
		// on delete l'ecran principal.
		if(mScreen)
		{
			SDL_FreeSurface(mScreen);
			mScreen = NULL;
		}
		
		// on delete le Joystick.
		SDL_JoystickClose(mJoystick);
		// Désactivation de la répétition des touches (remise à 0)
		SDL_EnableKeyRepeat(0, 0);
		// On quitte la SDL en étant sûr que tout les objets SDL ont bien été détruis avant.
		SDL_Quit();
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void Application::Init(TinyXmlElement* pXml)
	{
		// 1) On récupère le titre de l'application.
		const std::string lTitle = TinyXmlHlp::GetAttribute(pXml, "title", "");
		MTest_T_MSG("" == lTitle, "Cannot retrieve Application Title !");

		// 2) On récupère le nom du fichier icone.
		const std::string lIconFilename = TinyXmlHlp::GetAttribute(pXml, "iconFile", "");
		MTest_T_MSG("" == lIconFilename, "Cannot retrieve icon filename !");

		// 3) On récupère les paramètres du clavier.
		TinyXmlElement* lXmlKeyBoardSettings = pXml->FirstChildElement("KeyBoardSettings");
		MTest_T_MSG(! lXmlKeyBoardSettings, "Cannot retrieve Keyboard settings !");
		unsigned lKBDelay = 0;
		unsigned lKBInterval = 0;
		{
			lKBDelay = TinyXmlHlp::GetAttributeI(lXmlKeyBoardSettings, "repeatDelay", -1);
			MTest_T_MSG(-1 == lKBDelay, "Cannot retrieve Keyboard repeat delay !");

			lKBInterval = TinyXmlHlp::GetAttributeI(lXmlKeyBoardSettings, "repeatInterval", -1);
			MTest_T_MSG(-1 == lKBInterval, "Cannot retrieve Keyboard repeat interval !");
		}

		// 4) On récupère les paramètres videos.
		TinyXmlElement* lXmlVideoSettings = pXml->FirstChildElement("VideoSettings");
		MTest_T_MSG(! lXmlVideoSettings, "Cannot retrieve Video settings !");
		unsigned lWidth = 0;
		unsigned lHeight = 0;
		unsigned lNbBitsPerPixel = 0;
		Uint32 lTotalFlag = 0;
		{
			lWidth = TinyXmlHlp::GetAttributeI(lXmlVideoSettings, "width", -1);
			MTest_T_MSG(-1 == lWidth, "Cannot retrieve Window width !");

			lHeight = TinyXmlHlp::GetAttributeI(lXmlVideoSettings, "height", -1);
			MTest_T_MSG(-1 == lHeight, "Cannot retrieve Window height !");

			lNbBitsPerPixel = TinyXmlHlp::GetAttributeI(lXmlVideoSettings, "nbBitsPerPixel", -1);
			MTest_T_MSG(-1 == lHeight, "Cannot retrieve Bits per pixel !");
		}
		// Les Flags pour le mode video.
		{
			TinyXmlElement* lXmlFlags = lXmlVideoSettings->FirstChildElement("Flags");
			MTest_T_MSG(! lXmlFlags, "Cannot retrieve Video Flags !");

			TinyXmlElement* lXmlFlag = lXmlFlags->FirstChildElement("Flag");
			while(lXmlFlag)
			{
				const std::string& lFlag = TinyXmlHlp::GetAttribute(lXmlFlag, "value", "");
				Uint32 lFlagValue = 0;
				if("cpu" == lFlag)
				{ lFlagValue = SDL_SWSURFACE;}
				else if("gpu" == lFlag)
				{ lFlagValue = SDL_HWSURFACE;}
				else if("doubleBuffer" == lFlag)
				{ lFlagValue = SDL_DOUBLEBUF;}
				else if("fullscreen" == lFlag)
				{ lFlagValue = SDL_FULLSCREEN;}
				else if("openGL" == lFlag)
				{ lFlagValue = SDL_OPENGL; }
				else if("openGLBlit" == lFlag)
				{ lFlagValue = SDL_OPENGLBLIT; }
				else if("resizable" == lFlag)
				{ lFlagValue = SDL_RESIZABLE; }
				else if("sansBordures" == lFlag)
				{ lFlagValue = SDL_NOFRAME; }
				else
				{
					MThrow("Cannot retrieve a Video Flag !");
				}

				lTotalFlag |= lFlagValue;
				lXmlFlag = lXmlFlag->NextSiblingElement();
			}
		}

		//-----------------------------------------------//
		// 1) Chargement de l'icône avant SDL_SetVideoMode
		SDL_Surface* lIcone = IMG_Load(lIconFilename.c_str());
		MTest_T_MSG(! lIcone,"Cannot load icon "<< lIconFilename << " !");
		SDL_WM_SetIcon(lIcone, NULL);
		SDL_FreeSurface(lIcone);
		lIcone = NULL;

		// 2) On donne le titre
		SDL_WM_SetCaption(lTitle.c_str(), NULL);

		// 3) Configuration de la répétition du clavier (en ms)
		SDL_EnableKeyRepeat(lKBDelay, lKBInterval);

		// 4) On creer l'ecran avec les parametres videos recueillis dans le Xml.
		mScreen = SDL_SetVideoMode(lWidth, lHeight, lNbBitsPerPixel, lTotalFlag);

		// 5) Position de départ de la souris au milieu de l'écran.
		SDL_WarpMouse((Uint16)mScreen->w / 2, (Uint16)mScreen->h / 2);
	}

	//-------------------------------------------------------------------------------------------------------------------//
}