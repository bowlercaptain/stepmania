#ifndef ProfileManager_H
#define ProfileManager_H
/*
-----------------------------------------------------------------------------
 Class: ProfileManager

 Desc: Interface to profiles that exist on the machine or a memory card
	plugged into the machine.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/


#include "PlayerNumber.h"
#include "GameConstantsAndTypes.h"

struct Profile
{
	Profile() { Init(); }
	void Init()
	{
		m_sName = "";
		m_sLastUsedHighScoreName = "";
		m_bUsingProfileDefaultModifiers = false;
		m_sDefaultModifiers = "";
		m_iTotalPlays = 0;
		m_iTotalPlaySeconds = 0;
		m_iTotalGameplaySeconds = 0;
		m_iCurrentCombo = 0;
	}

	bool LoadFromIni( CString sIniPath );
	bool SaveToIni( CString sIniPath );
	CString m_sName;
	CString m_sLastUsedHighScoreName;
	bool m_bUsingProfileDefaultModifiers;
	CString m_sDefaultModifiers;
	int m_iTotalPlays;
	int m_iTotalPlaySeconds;
	int m_iTotalGameplaySeconds;
	int m_iCurrentCombo;
};

class ProfileManager
{
public:
	ProfileManager();
	~ProfileManager();

	bool CreateLocalProfile( CString sName );
	bool RenameLocalProfile( CString sProfileID, CString sNewName );
	bool DeleteLocalProfile( CString sProfileID );

	bool CreateMemoryCardProfile( CString sName );

	void GetLocalProfileIDs( vector<CString> &asProfileIDsOut );
	void GetLocalProfileNames( vector<CString> &asNamesOut );

	bool LoadFirstAvailableProfile( PlayerNumber pn );
	bool IsMemoryCardInserted( PlayerNumber pn );
	bool SaveProfile( PlayerNumber pn );
	void UnloadProfile( PlayerNumber pn );

	void SaveMachineProfile();

	bool IsUsingProfile( PlayerNumber pn ) { return !m_sProfileDir[pn].empty(); }
	Profile* GetProfile( PlayerNumber pn );

	Profile* GetMachineProfile() { return &m_MachineProfile; }

	bool IsUsingMemoryCard( PlayerNumber pn ) { return m_bUsingMemoryCard[pn]; }


	//
	// High scores
	//
	void InitMachineScoresFromDisk();
	void SaveMachineScoresToDisk();

	struct CategoryData
	{
		struct HighScore
		{
			int iScore;
			CString	sName;

			HighScore()
			{
				iScore = 0;
			}

			bool operator>=( const HighScore& other ) const
			{
				return iScore >= other.iScore;
			}
		};
		vector<HighScore> vHighScores;

		void AddHighScore( HighScore hs, int &iIndexOut );

	} m_CategoryDatas[NUM_MEMORY_CARDS][NUM_STEPS_TYPES][NUM_RANKING_CATEGORIES];

	void AddHighScore( StepsType nt, RankingCategory rc, PlayerNumber pn, CategoryData::HighScore hs, int &iMachineIndexOut )
	{
		hs.sName = RANKING_TO_FILL_IN_MARKER[pn];
		m_CategoryDatas[pn][nt][rc].AddHighScore( hs, iMachineIndexOut );
		m_CategoryDatas[MEMORY_CARD_MACHINE][nt][rc].AddHighScore( hs, iMachineIndexOut );
	}

	void ReadSM300NoteScores();
	void ReadSongScoresFromFile( CString fn, MemoryCard mc );
	void ReadCourseScoresFromFile( CString fn, MemoryCard mc );
	void ReadCategoryScoresFromFile( CString fn, MemoryCard mc );

	void SaveSongScoresToFile( CString fn, MemoryCard mc );
	void SaveCourseScoresToFile( CString fn, MemoryCard mc );
	void SaveCategoryScoresToFile( CString fn, MemoryCard mc );
	void SaveStatsWebPageToFile( CString fn, MemoryCard mc );

private:
	bool LoadDefaultProfileFromMachine( PlayerNumber pn );
	bool LoadProfileFromMemoryCard( PlayerNumber pn );
	bool LoadProfile( PlayerNumber pn, CString sProfileDir, bool bIsMemCard );
	bool CreateProfile( CString sProfileDir, CString sName );

	// Directory that contains the profile.  Either on local machine or
	// on a memory card.
	CString m_sProfileDir[NUM_PLAYERS];

	bool m_bUsingMemoryCard[NUM_PLAYERS];
	
	// actual loaded profile data
	Profile	m_Profile[NUM_PLAYERS];	

	Profile m_MachineProfile;

};


extern ProfileManager*	PROFILEMAN;	// global and accessable from anywhere in our program

#endif
