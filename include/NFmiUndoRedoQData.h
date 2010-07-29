#pragma once

#include <string>
#include <deque>
#include "NFmiHarmonizerBookKeepingData.h"

class NFmiRawData;

class NFmiUndoRedoQData
{
public:
	NFmiUndoRedoQData(void);
	~NFmiUndoRedoQData(void);

	bool SnapShotData(const std::string& theAction, const NFmiHarmonizerBookKeepingData &theHarmonizerBookKeepingData, const NFmiRawData &theRawData);
	void RearrangeUndoTable(void);
	bool Undo(void);
	bool Redo(void);
	bool UndoData(const NFmiHarmonizerBookKeepingData &theHarmonizerBookKeepingData, NFmiRawData &theRawData);
	bool RedoData(NFmiRawData &theRawData);
	void UndoLevel(long theDepth, const NFmiRawData &theRawData);
	const NFmiHarmonizerBookKeepingData* CurrentHarmonizerBookKeepingData(void) const;

private:
	long* itsMaxUndoLevelPtr;
	long* itsMaxRedoLevelPtr;
	int* itsCurrentUndoLevelPtr;
	int* itsCurrentRedoLevelPtr;

	char** itsUndoTable;
	std::string* itsUndoTextTable;
	std::deque<NFmiHarmonizerBookKeepingData> *itsUndoRedoHarmonizerBookKeepingData; // t�m�n elin kaari seuraa tiiviisti itsUndoTable:a
									// t�h�n talletetaan harmonisaatiossa 'likaantuvat' parametrit ja ajat ja koska
									// editorissa on undo/redo toiminto, pit�� my�s t�m�n olla synkassa datan kanssa
};