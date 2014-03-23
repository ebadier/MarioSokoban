#ifndef __SDL_MARIOSOKOBAN_H__
#define __SDL_MARIOSOKOBAN_H__

#include <array>

#include "csv/csv_parser.h"

#include "sdlApplication.h"
#include "sdlElements.h"

class TinyXmlElement;

namespace sdl
{
	/**
	*	\brief Jeu MarioSokoban.
	*/
	class MarioSokoban : public Application
	{
	private:
		#define BLOC_SIZE		34 // Taille d'un bloc (carré) en pixels
		#define NB_BLOCS_WIDTH	12
		#define NB_BLOCS_HEIGHT	12
		#define NB_TOTAL_BLOCS	NB_BLOCS_WIDTH * NB_BLOCS_HEIGHT
		#define WINDOW_WIDTH	BLOC_SIZE * NB_BLOCS_WIDTH
		#define WINDOW_HEIGHT	BLOC_SIZE * NB_BLOCS_HEIGHT

		/**
		*	\brief SurfaceIDs for MarioSokoban.
		*/
		const enum {HAUT, BAS, GAUCHE, DROITE, VIDE, MUR, OBJECTIF, CAISSE, CAISSE_OK, MARIO};

		/**
		*	\brief Mapping [Xml : SurfaceID].
		*/
		typedef std::map< std::string, SurfaceID > StrToSurfaceIDMap;

		/**
		*	\brief return true if player win level.
		*/
		bool Play();

		/**
		*	\brief Edit levels.
		*/
		void Edit();

		/**
		*	\brief Go to an unlock level.
		*/
		void ManageLevels();

		/**
		*	\brief Load a level from a file.
		*/
		bool LoadLevel(int pLevelNum);
		
		/**
		*	\brief Refresh screen.
		*/
		void UpdateScreenInGame();
		
		/**
		*	\brief Move Mario with keyboard input and manages collisions.
		*/
		void MoveMario();

		/**
		*	\brief calcul si la surface de la première case peut-être déplacée sur la seconde case.
		*	\return vrai si il y a eu déplacement, et faux sinon.
		*/
		bool MoveCaisse(SurfaceID* premiereCase, SurfaceID* secondeCase);

	public:
		MarioSokoban();
		virtual ~MarioSokoban();

		/**
		*	\brief Init SDL Application from Xml.
		*/
		void Init(const std::string& pXmlFileName);
		
		/**
		*	\brief main game loop.
		*/
		void Menu();

	private:
		// Sprites.
		Surface				mBackground; //!< l'image de Fond.
		Surface				mChoix; //!< l'image qui apparait à la sélection du niveau.
		Surfaces			mSurfaces; //!< toutes les Surfaces.
		StrToSurfaceIDMap	mStrToSurfacesIDs; //!< pour l'init depuis le Xml.
		Surface*			mMarioCourant; //!< Mario (HAUT | BAS | GAUCHE | DROITE) (HANDLE).

		// Positions.
		SDL_Rect			mPositionBackgroung; //!< La position du Fond d'écran.
		SDL_Rect			mPositionMario; //!< La position de Mario.
		SDL_Rect			mTmpPosition; //!< Une position pour toutes les autres surfaces.

		// Progression.
		int					mNbObjectifs; //!< Le nombre d'objectifs restant.
		int					mLastLevelSuccess; //!< Le numéro du dernier niveau réussi.

		// Levels.
		std::array<SurfaceID,NB_TOTAL_BLOCS>	mCurrentLevel; //!< current level.
		csv_parser								mLevelLoader; //!< Pour charger un niveau.
		std::vector< std::string >				mLevelFilePaths; //!< Contient les noms des fichiers de niveaux du jeu.
	};
}

#endif // __SDL_MARIOSOKOBAN_H__