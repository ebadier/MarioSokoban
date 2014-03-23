#include "SDL_image.h"

#include "tinyxml/tinyxmlHlp.h"

#include "macros.h"

#include "sdlMarioSokoban.h"

namespace sdl
{

	//-------------------------------------------------------------------------------------------------------------------//
	MarioSokoban::MarioSokoban():	mBackground(),
									mChoix(),
									mSurfaces(),
									mStrToSurfacesIDs(),
									mMarioCourant(NULL),
									mPositionBackgroung(),
									mPositionMario(),
									mTmpPosition(),
									mNbObjectifs(0),
									mLastLevelSuccess(1),
									mCurrentLevel(),
									mLevelLoader(),
									mLevelFilePaths()
	{
		// On défini les paramètres du parser de csv.
		mLevelLoader.set_enclosed_char(NULL, ENCLOSURE_OPTIONAL);
		mLevelLoader.set_field_term_char(';');
		mLevelLoader.set_line_term_char('\n');

		// Mapping
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("haut", HAUT));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("bas", BAS));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("gauche", GAUCHE));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("droite", DROITE));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("vide", VIDE));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("mur", MUR));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("objectif", OBJECTIF));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("caisse", CAISSE));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("caisse_ok", CAISSE_OK));
		mStrToSurfacesIDs.insert(StrToSurfaceIDMap::value_type("mario", MARIO));
	}

	//-------------------------------------------------------------------------------------------------------------------//
	MarioSokoban::~MarioSokoban()
	{
		mMarioCourant = NULL;
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::UpdateScreenInGame()
	{
		for (Sint16 line = 0; line < NB_BLOCS_HEIGHT; ++line)
		{
			mTmpPosition.y = line*BLOC_SIZE;

			for (Sint16 col = 0; col < NB_BLOCS_WIDTH; ++col)
			{
				mTmpPosition.x = col*BLOC_SIZE;
				const SurfaceID& lSurfaceID = mCurrentLevel[line * NB_BLOCS_WIDTH + col];

				if(lSurfaceID != VIDE && lSurfaceID != MARIO)
				{
					Surface* lSurface = mSurfaces.GetSurface(lSurfaceID);
					lSurface->ApplyToScreen(&mTmpPosition, mScreen);
				}
			}
		}
		mTmpPosition.x = mPositionMario.x*BLOC_SIZE;
		mTmpPosition.y = mPositionMario.y*BLOC_SIZE;
		mMarioCourant->ApplyToScreen(&mTmpPosition, mScreen);
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::GameProgress()
	{
		while(PlayCurrentLevel())
		{
			++mLastLevelSuccess;
			if(! LoadLevel(mLastLevelSuccess)){break;}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::Menu()
	{
		while (! mIsItTimeToClose)
		{
			SDL_WaitEvent(&mEvent);
			switch(mEvent.type)
			{
			// Si on clique sur la croix rouge.
			case SDL_QUIT:
					mIsItTimeToClose = true; break;
			// Si on appuie sur une touche.
			case SDL_KEYDOWN:
				switch(mEvent.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					mIsItTimeToClose = true; break;
				case SDLK_e:
					SDL_ShowCursor(SDL_ENABLE); break;
				case SDLK_d:
					SDL_ShowCursor(SDL_DISABLE); break;
				case SDLK_KP1:
					if(LoadLevel(mLastLevelSuccess))
					{
						GameProgress();
					}
					break;
				//case SDLK_KP2:
					//edit(); break;
				case SDLK_KP3:
					ManageLevels(); break;
				}
				break;
			} // FIN switch mEvent.type

			// On efface l'écran pour éviter le scintillement.
			SDL_FillRect(mScreen, NULL, SDL_MapRGB(mScreen->format, 255, 255, 255));
			// On applique le Fond d'écran.
			mBackground.ApplyToScreen(&mPositionBackgroung, mScreen);
			// On met à jour l'écran.
			SDL_Flip(mScreen);

		} // FIN while
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::ManageLevels()
	{
		bool chooseDone(false);
		while ((! mIsItTimeToClose) && (! chooseDone))
		{
			SDL_WaitEvent(&mEvent);
			switch(mEvent.type)
			{
			// Si on clique sur la croix rouge.
			case SDL_QUIT:
					mIsItTimeToClose = true; break;
			// Si on appuie sur une touche.
			case SDL_KEYDOWN:
				switch(mEvent.key.keysym.sym)
				{
				case SDLK_ESCAPE: chooseDone = true; break;
				case SDLK_e: SDL_ShowCursor(SDL_ENABLE); break;
				case SDLK_d: SDL_ShowCursor(SDL_DISABLE); break;
				case SDLK_KP1:
					if(LoadLevel(1)){GameProgress();} break;
				case SDLK_KP2:
					if(LoadLevel(2)){GameProgress();} break;
				case SDLK_KP3:
					if(LoadLevel(3)){GameProgress();} break;
				case SDLK_KP4:
					if(LoadLevel(4)){GameProgress();} break;
				case SDLK_KP5:
					if(LoadLevel(5)){GameProgress();} break;
				case SDLK_KP6:
					if(LoadLevel(6)){GameProgress();} break;
				case SDLK_KP7:
					if(LoadLevel(7)){GameProgress();} break;
				}
				break;
			}
			// On efface l'écran pour éviter le scintillement.
			SDL_FillRect(mScreen, NULL, SDL_MapRGB(mScreen->format, 255, 255, 255));
			// On applique le Choix des niveaux à l'écran.
			mChoix.ApplyToScreen(&mPositionBackgroung, mScreen);
			// On met à jour l'écran.
			SDL_Flip(mScreen);
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//
	bool MarioSokoban::PlayCurrentLevel()
	{
		bool levelSuccess = true;
		while ((! mIsItTimeToClose) && mNbObjectifs)
		{
			SDL_WaitEvent(&mEvent);
			switch(mEvent.type)
			{
			// Si on clique sur la croix rouge.
			case SDL_QUIT:
					mIsItTimeToClose = true; break;
			// Si on appuie sur une touche.
			case SDL_KEYDOWN:
				switch(mEvent.key.keysym.sym)
				{
                // Autres touches
				case SDLK_ESCAPE: levelSuccess=false; mNbObjectifs=0; break;
				case SDLK_r: LoadLevel(mLastLevelSuccess); break; // retry level.
				case SDLK_e: SDL_ShowCursor(SDL_ENABLE); break;
				case SDLK_d: SDL_ShowCursor(SDL_DISABLE); break;
                // Si on appuie sur une touche directionnel.
                default : MoveMario(); break;
				}
				break;
			} // FIN switch mEvent.type

			// On efface l'écran pour éviter le scintillement.
			SDL_FillRect(mScreen, NULL, SDL_MapRGB(mScreen->format, 255, 255, 255));
			// On applique les surfaces concernant le jeu sur l'écran.
			UpdateScreenInGame();
			// On met à jour l'écran.
			SDL_Flip(mScreen);
		}

		return levelSuccess;
	}

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::MoveMario()
	{
	    SurfaceID* lNextCase(NULL);
		SurfaceID* lNextNextCase(NULL);

		switch(mEvent.key.keysym.sym)
	    {
		//-----------------------------------------------//
	    case SDLK_UP: // Flèche haut.
			if (mPositionMario.y - 1 < 0) { break; }// Si dépasse l'écran, on arrête
            lNextCase = &mCurrentLevel[(mPositionMario.y - 1) * NB_BLOCS_WIDTH + mPositionMario.x];
			if (*lNextCase == MUR) { break; } // Si obstacle, on arrête
			// On peut pousser une caisse.
			if(mPositionMario.y - 2 >= 0)
			{
				lNextNextCase = &mCurrentLevel[(mPositionMario.y - 2) * NB_BLOCS_WIDTH + mPositionMario.x];
				if(! MoveCaisse(lNextCase, lNextNextCase)) { break; }
			}
			// On peut déplacer mario.
			--mPositionMario.y; mMarioCourant = mSurfaces.GetSurface(HAUT);
			break;

        //-----------------------------------------------//
		case SDLK_DOWN: // Flèche bas.
			if(mPositionMario.y + 1 > mScreen->h) { break; }// Si dépasse l'écran, on arrête
            lNextCase = &mCurrentLevel[(mPositionMario.y + 1) * NB_BLOCS_WIDTH + mPositionMario.x];
			if (*lNextCase == MUR) { break; } // Si obstacle, on arrête
			if(mPositionMario.y + 2 <= mScreen->h)
			{// On peut pousser une caisse.
				lNextNextCase = &mCurrentLevel[(mPositionMario.y + 2) * NB_BLOCS_WIDTH + mPositionMario.x];
				if(!MoveCaisse(lNextCase, lNextNextCase)) { break; }
			}
			// On peut déplacer mario.
			++mPositionMario.y; mMarioCourant = mSurfaces.GetSurface(BAS);
			break;

        //-----------------------------------------------//
		case SDLK_RIGHT: // Flèche droite
			if (mPositionMario.x + 1 > mScreen->w) { break; }// Si dépasse l'écran, on arrête
            lNextCase = &mCurrentLevel[mPositionMario.y * NB_BLOCS_WIDTH + mPositionMario.x + 1];
			if (*lNextCase == MUR) { break; } // Si obstacle, on arrête
			if(mPositionMario.x + 2 <= mScreen->w)
			{// On peut pousser une caisse.
				lNextNextCase = &mCurrentLevel[mPositionMario.y * NB_BLOCS_WIDTH + mPositionMario.x + 2];
				if(!MoveCaisse(lNextCase, lNextNextCase)) { break; }
			}
			// On peut déplacer mario.
			++mPositionMario.x; mMarioCourant = mSurfaces.GetSurface(DROITE);
			break;

        //-----------------------------------------------//
		case SDLK_LEFT: // Flèche gauche
			if (mPositionMario.x - 1 < 0) { break; }// Si dépasse l'écran, on arrête
            lNextCase = &mCurrentLevel[mPositionMario.y * NB_BLOCS_WIDTH + mPositionMario.x - 1];
			if (*lNextCase == MUR) { break; } // Si obstacle, on arrête
			if(mPositionMario.x - 2 >= 0)
			{// On peut pousser une caisse.
				lNextNextCase = &mCurrentLevel[mPositionMario.y * NB_BLOCS_WIDTH + mPositionMario.x - 2];
				if(!MoveCaisse(lNextCase, lNextNextCase)) { break; }
			}
			// On peut déplacer mario.
			--mPositionMario.x; mMarioCourant = mSurfaces.GetSurface(GAUCHE);
			break;
	    }
	}

	//-------------------------------------------------------------------------------------------------------------------//
    bool MarioSokoban::MoveCaisse(SurfaceID* premiereCase, SurfaceID* pSecondeCase)
    {
        if (*premiereCase == CAISSE || *premiereCase == CAISSE_OK)
        {
            // Si on veut pousser une caisse, il faut vérifier qu'il n'y a pas de mur ou caisse dans la case d'après.
			if(*pSecondeCase == MUR || *pSecondeCase == CAISSE || *pSecondeCase == CAISSE_OK){ return false; }
			
			if (*pSecondeCase == OBJECTIF)
            {
                *pSecondeCase = CAISSE_OK;
                --mNbObjectifs;
            }
            else
            {
                *pSecondeCase = CAISSE;
            }
            // Cas où on peut rebouger une caisse_ok.
            if (*premiereCase == CAISSE_OK)
            {
                *premiereCase = OBJECTIF;
				++mNbObjectifs;
            }
            else
            {
                *premiereCase = VIDE;
            }
        }
		return true;
    }

	//-------------------------------------------------------------------------------------------------------------------//
	void MarioSokoban::Init(const std::string& pXmlFileName)
	{
		std::string errorMsg;
		std::auto_ptr<XmlDocument> doc = TinyXmlHlp::OpenXmlFile(pXmlFileName, "SDLApplication", errorMsg);
		MTest_T_MSG(! doc->IsValid(), errorMsg);
		
		Application::Init(doc->mRoot);

		//-----------------------------------------------//
		TinyXmlElement* lXmlImages = doc->mRoot->FirstChildElement("Images");
		MTest_T_MSG(! lXmlImages, "Cannot retrieve Images !");

		const std::string lImagesDir = TinyXmlHlp::GetAttribute(lXmlImages, "imagesDir", "");
		MTest_T_MSG("" == lImagesDir, "Cannot retrieve imagesDir !");
		{
			{
				// On récupère d'abord le fond d'ecran.
				TinyXmlElement* lXmlFond = lXmlImages->FirstChildElement("BackGround");
				MTest_T_MSG(! lXmlFond, "Cannot retrieve BackGround !");
				{
					std::string lCompleteFilename(lImagesDir);
					const std::string lFondFilename = TinyXmlHlp::GetAttribute(lXmlFond, "filename", "");
					MTest_T_MSG("" == lFondFilename, "Cannot retrieve BackGround filename !");
					lCompleteFilename += lFondFilename;

					// On charge le fond d'écran.
					mBackground.mSurface = IMG_Load(lCompleteFilename.c_str());
					MTest_T_MSG(! mBackground.mSurface, "Cannot load BackGround from "<< lCompleteFilename << " !");

					// On récupère sa position.
					mPositionBackgroung.x = (Sint16)TinyXmlHlp::GetAttributeI(lXmlFond, "positionX", std::numeric_limits<int>::min());
					MTest_T_MSG(std::numeric_limits<int>::min() == mPositionBackgroung.x, "Cannot retrieve BackGround positionX !");
					mPositionBackgroung.y = (Sint16)TinyXmlHlp::GetAttributeI(lXmlFond, "positionY", std::numeric_limits<int>::min());
					MTest_T_MSG(std::numeric_limits<int>::min() == mPositionBackgroung.y, "Cannot retrieve BackGround positionY !");
				}
			}
			{
				// On récupère l'image de choix de niveau.
				TinyXmlElement* lXmlChoix = lXmlImages->FirstChildElement("Choix");
				MTest_T_MSG(! lXmlChoix, "Cannot retrieve Choix !");
				{
					// On récupère d'abord le nom de l'image.
					std::string lCompleteFilename(lImagesDir);
					const std::string lChoixFilename = TinyXmlHlp::GetAttribute(lXmlChoix, "filename", "");
					MTest_T_MSG("" == lChoixFilename, "Cannot retrieve Choix filename !");
					lCompleteFilename += lChoixFilename;

					// On charge l'image.
					mChoix.mSurface = IMG_Load(lCompleteFilename.c_str());
					MTest_T_MSG(! mChoix.mSurface, "Cannot load Choix from "<< lCompleteFilename << " !");
				}
			}
		}

		//-----------------------------------------------//
		// On récupère toutes les autres images.
		{
			TinyXmlElement* lXmlOthers = lXmlImages->FirstChildElement("Others");
			MTest_T_MSG(! lXmlOthers, "Cannot retrieve Others !");

			TinyXmlElement* lXmlImg = lXmlOthers->FirstChildElement("Img");
			MTest_T_MSG(! lXmlImg, "Cannot retrieve Img !");

			while(lXmlImg)
			{
				// On récupère d'abord le nom de l'image.
				std::string lCompleteFilename(lImagesDir);
				{
					const std::string lImgFilename = TinyXmlHlp::GetAttribute(lXmlImg, "filename", "");
					MTest_T_MSG("" == lImgFilename, "Cannot retrieve Img filename !");
					lCompleteFilename += lImgFilename;
				}
				// On récupère son ID.
				const std::string lSurfaceIDXml = TinyXmlHlp::GetAttribute(lXmlImg, "id", "");
				MTest_T_MSG("" == lSurfaceIDXml, "Cannot retrieve Img id !");
				StrToSurfaceIDMap::const_iterator found = mStrToSurfacesIDs.find(lSurfaceIDXml);
				MTest_T_MSG(found == mStrToSurfacesIDs.end(), "Img id " << lSurfaceIDXml << " unknown !");  
				mSurfaces.CreateSurface(found->second, lCompleteFilename);

				lXmlImg = lXmlImg->NextSiblingElement();
			}
		}

		//-----------------------------------------------//
		// On récupère les Levels
		TinyXmlElement* lXmlLevels = doc->mRoot->FirstChildElement("Levels");
		MTest_T_MSG(! lXmlLevels, "Cannot retrieve Levels !");
		{
			const std::string lLevelsDir = TinyXmlHlp::GetAttribute(lXmlLevels, "levelsDir", "");
			MTest_T_MSG("" == lLevelsDir, "Cannot retrieve levelsDir !");

			// On récupère les noms des niveaux.
			TinyXmlElement* lXmlLevel = lXmlLevels->FirstChildElement("Level");
			MTest_T_MSG(! lXmlLevel, "Cannot retrieve Level !");
			while(lXmlLevel)
			{
				const std::string lLevelname = TinyXmlHlp::GetAttribute(lXmlLevel, "filename", "");
				MTest_T_MSG("" == lLevelname, "Cannot retrieve Level filename !");
				const std::string lCompleteLevelname(lLevelsDir + lLevelname);
				mLevelFilePaths.push_back(lCompleteLevelname);

				lXmlLevel = lXmlLevel->NextSiblingElement();
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------------------//
	bool MarioSokoban::LoadLevel(int pLevelNum)
	{
		// remis à zéro ICI, c'est la meilleure solution.
		mNbObjectifs = 0;

		if(0 == pLevelNum || pLevelNum > (int)mLevelFilePaths.size())
		{
			return false;
		}

		if(mLastLevelSuccess < pLevelNum)
		{
			MOutAndPopup("Level " << pLevelNum << " is locked !");
			return false;
		}

		// On récupère le nom du niveau.
		const std::string& lLevel = mLevelFilePaths[pLevelNum-1];
		MTest_T_MSG(! mLevelLoader.init(lLevel.c_str()), "Cannot load level " << lLevel);

		for(Sint16 line=0; line < NB_BLOCS_HEIGHT; ++line)
		{
			const csv_row& lcsv_row = mLevelLoader.get_row();

			for(Sint16 col=0; col < NB_BLOCS_WIDTH; ++col)
			{
				// on récupère le contenu de la case.
				const std::string& lSurfaceCsvID = lcsv_row[col];
				StrToSurfaceIDMap::const_iterator found = mStrToSurfacesIDs.find(lSurfaceCsvID);
				MTest_T_MSG(found == mStrToSurfacesIDs.end(), "Wrong Level format \"" << lLevel << "\" :\n"
					<< "Unknown SurfaceID at (line " << line+1 << ", col " << col+1 << ") !");
				// On récupère son équivalent enum.
				const SurfaceID& lSurfaceID = found->second;
				mCurrentLevel[line * NB_BLOCS_WIDTH + col] = lSurfaceID;
				if(lSurfaceID == MARIO)
				{
				    mPositionMario.y = line;
				    mPositionMario.x = col;
				}

				if(lSurfaceID == OBJECTIF)
				{
				    ++mNbObjectifs;
				}
			}

		}
		// La position de départ de Mario est "bas".
		mMarioCourant = mSurfaces.GetSurface(BAS);

		// On affiche le niveau à l'écran. 
		UpdateScreenInGame();

		return true;
	}

	//-------------------------------------------------------------------------------------------------------------------//
}
