
#include "NFmiUndoRedoQData.h"
#include <newbase/NFmiRawData.h>
#include <cstring>
#include <utility>

NFmiUndoRedoQData::NFmiUndoRedoQData()
{
  itsMaxRedoLevelPtr = nullptr;
  itsCurrentUndoLevelPtr = nullptr;
  itsCurrentRedoLevelPtr = nullptr;
  itsUndoTable = nullptr;
  itsUndoTextTable = nullptr;
}

NFmiUndoRedoQData::~NFmiUndoRedoQData()
{
  Clear();
}

NFmiUndoRedoQData::NFmiUndoRedoQData(const NFmiUndoRedoQData &theOther) : NFmiUndoRedoQData()
{
  try
  {
    if (theOther.itsMaxUndoLevelPtr)
      itsMaxUndoLevelPtr = new long(*theOther.itsMaxUndoLevelPtr);
    if (theOther.itsMaxRedoLevelPtr)
      itsMaxRedoLevelPtr = new long(*theOther.itsMaxRedoLevelPtr);
    if (theOther.itsCurrentUndoLevelPtr)
      itsCurrentUndoLevelPtr = new int(*theOther.itsCurrentUndoLevelPtr);
    if (theOther.itsCurrentRedoLevelPtr)
      itsCurrentRedoLevelPtr = new int(*theOther.itsCurrentRedoLevelPtr);
    itsBufferSize = theOther.itsBufferSize;

    if (theOther.itsUndoTable && theOther.itsMaxUndoLevelPtr)
    {
      const long levels = *theOther.itsMaxUndoLevelPtr;
      itsUndoTable = new char *[levels];
      for (long level = 0; level < levels; level++)
        itsUndoTable[level] = nullptr;
      for (long level = 0; level < levels; level++)
      {
        if (theOther.itsUndoTable[level])
        {
          itsUndoTable[level] = new char[itsBufferSize];
          std::memcpy(itsUndoTable[level], theOther.itsUndoTable[level], itsBufferSize);
        }
      }
    }
    if (theOther.itsUndoTextTable && theOther.itsMaxUndoLevelPtr)
    {
      const long levels = *theOther.itsMaxUndoLevelPtr;
      itsUndoTextTable = new std::string[levels];
      for (long level = 0; level < levels; level++)
        itsUndoTextTable[level] = theOther.itsUndoTextTable[level];
    }
  }
  catch (...)
  {
    Clear();
    throw;
  }
}

NFmiUndoRedoQData &NFmiUndoRedoQData::operator=(const NFmiUndoRedoQData &theOther)
{
  if (this != &theOther)
  {
    NFmiUndoRedoQData tmp(theOther);
    Swap(tmp);
  }
  return *this;
}

void NFmiUndoRedoQData::Swap(NFmiUndoRedoQData &theOther) noexcept
{
  std::swap(itsMaxUndoLevelPtr, theOther.itsMaxUndoLevelPtr);
  std::swap(itsMaxRedoLevelPtr, theOther.itsMaxRedoLevelPtr);
  std::swap(itsCurrentUndoLevelPtr, theOther.itsCurrentUndoLevelPtr);
  std::swap(itsCurrentRedoLevelPtr, theOther.itsCurrentRedoLevelPtr);
  std::swap(itsUndoTable, theOther.itsUndoTable);
  std::swap(itsUndoTextTable, theOther.itsUndoTextTable);
  std::swap(itsBufferSize, theOther.itsBufferSize);
}

void NFmiUndoRedoQData::Clear()
{
  if (itsUndoTable != nullptr)
  {
    for (int level = 0; level < (*itsMaxUndoLevelPtr); level++)
    {
      if (itsUndoTable[level])
        delete[] itsUndoTable[level];  // allocated with new char[]
    }
  }
  if (itsUndoTable)
  {
    delete[] itsUndoTable;
    itsUndoTable = nullptr;
  }
  if (itsUndoTextTable)
  {
    delete[] itsUndoTextTable;
    itsUndoTextTable = nullptr;
  }

  if (itsMaxUndoLevelPtr)
  {
    delete itsMaxUndoLevelPtr;
    itsMaxUndoLevelPtr = nullptr;
  }

  if (itsMaxRedoLevelPtr)
  {
    delete itsMaxRedoLevelPtr;
    itsMaxRedoLevelPtr = nullptr;
  }

  if (itsCurrentUndoLevelPtr)
  {
    delete itsCurrentUndoLevelPtr;
    itsCurrentUndoLevelPtr = nullptr;
  }

  if (itsCurrentRedoLevelPtr)
  {
    delete itsCurrentRedoLevelPtr;
    itsCurrentRedoLevelPtr = nullptr;
  }
  itsBufferSize = 0;
}

bool NFmiUndoRedoQData::SnapShotData(const std::string &theAction, const NFmiRawData &theRawData)
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return false;
  if (!itsUndoTable || !itsUndoTextTable)
    return false;

  if (*itsMaxUndoLevelPtr <= 0)
    return false;

  if (*itsCurrentUndoLevelPtr == *itsMaxUndoLevelPtr - 1)
    RearrangeUndoTable();

  (*itsCurrentUndoLevelPtr)++;
  theRawData.Backup(itsUndoTable[*itsCurrentUndoLevelPtr]);
  (*itsCurrentRedoLevelPtr) = (*itsCurrentUndoLevelPtr);
  (*itsMaxRedoLevelPtr) = (*itsCurrentRedoLevelPtr);

  itsUndoTextTable[*itsCurrentUndoLevelPtr] = theAction;

  return true;
}

void NFmiUndoRedoQData::RearrangeUndoTable()
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return;
  char *undoTmp = itsUndoTable[0];
  std::string undoTmpText = itsUndoTextTable[0];

  for (int i = 0; i < (*itsMaxUndoLevelPtr) - 1; i++)
  {
    itsUndoTable[i] = itsUndoTable[i + 1];
    itsUndoTextTable[i] = itsUndoTextTable[i + 1];
  }

  itsUndoTable[*itsMaxUndoLevelPtr - 1] = undoTmp;
  itsUndoTextTable[*itsMaxUndoLevelPtr - 1] = undoTmpText;

  (*itsCurrentUndoLevelPtr)--;
  (*itsCurrentRedoLevelPtr)--;
}

bool NFmiUndoRedoQData::Undo()
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return false;
  return (*itsCurrentUndoLevelPtr) >= 0;
}

bool NFmiUndoRedoQData::Redo()
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return false;
  return !((*itsCurrentRedoLevelPtr) == (*itsCurrentUndoLevelPtr) ||
           (*itsCurrentRedoLevelPtr) == ((*itsCurrentUndoLevelPtr) + 1));
}

bool NFmiUndoRedoQData::UndoData(NFmiRawData &theRawData, std::string &modificationDescription)
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return false;
  if ((*itsCurrentUndoLevelPtr) < 0)
    return false;
  if ((*itsCurrentUndoLevelPtr) == (*itsCurrentRedoLevelPtr))
  {
    std::string action(itsUndoTextTable[*itsCurrentUndoLevelPtr]);
    SnapShotData(action,
                 theRawData);     // "Ottaa kuvan" undo-toimintoa edeltäneestä tilanteesta,
    (*itsCurrentUndoLevelPtr)--;  // jos siihen halutaankin myöhemmin palata redo:lla.
  }

  theRawData.Undo(itsUndoTable[*itsCurrentUndoLevelPtr]);
  (*itsCurrentUndoLevelPtr)--;
  *itsCurrentRedoLevelPtr = (*itsCurrentUndoLevelPtr) + 2;
  modificationDescription = itsUndoTextTable[(*itsCurrentUndoLevelPtr) + 1];
  return true;
}

bool NFmiUndoRedoQData::RedoData(NFmiRawData &theRawData, std::string &modificationDescription)
{
  if (itsCurrentUndoLevelPtr == nullptr)
    return false;
  if ((*itsCurrentUndoLevelPtr) == (*itsCurrentRedoLevelPtr) ||
      ((*itsCurrentUndoLevelPtr) + 1) == (*itsCurrentRedoLevelPtr))
    return false;

  theRawData.Undo(itsUndoTable[*itsCurrentRedoLevelPtr]);
  (*itsCurrentUndoLevelPtr)++;
  if ((*itsCurrentRedoLevelPtr) + 1 <= (*itsMaxRedoLevelPtr))
  {
    (*itsCurrentRedoLevelPtr)++;
  }

  modificationDescription = itsUndoTextTable[*itsCurrentUndoLevelPtr];
  return true;
}

void NFmiUndoRedoQData::UndoLevel(long theDepth,
                                  const NFmiRawData &theRawData)  // theDepth kuvaa kuinka monta
{                                                                 // Undota voidaan tehdä.
  if (!itsUndoTable && !itsUndoTextTable)  // Muuten taulukon ulkopuolelle viittaaminen voisi
  {                                        // olla mahdollista!
    itsMaxUndoLevelPtr = new long(0);
    itsMaxRedoLevelPtr = new long(0);
    itsCurrentUndoLevelPtr = new int(-1);
    itsCurrentRedoLevelPtr = new int(-1);

    if (theDepth <= 0)
      *itsMaxUndoLevelPtr = 0;
    else
    {
      *itsMaxRedoLevelPtr = 0;
      *itsCurrentUndoLevelPtr = -1;
      *itsCurrentRedoLevelPtr = -1;
      *itsMaxUndoLevelPtr = theDepth + 1;  // Redon tekemisen mahdollistamiseksi yksi
      try
      {
        itsUndoTable = new char *[*itsMaxUndoLevelPtr];  // taso lisää.
        itsUndoTextTable = new std::string[*itsMaxUndoLevelPtr];
        for (int level = 0; level < (*itsMaxUndoLevelPtr); level++)
          itsUndoTable[level] = nullptr;  // nollataan ensin pointteri, että voidaan siivota jälkiä
                                          // jos muistin varaus pettää joskus

        itsBufferSize = theRawData.Size() * sizeof(float);
        for (int level = 0; level < (*itsMaxUndoLevelPtr); level++)
          itsUndoTable[level] = new char[itsBufferSize];
      }
      catch (...)
      {  // oletetaan että poikkeus liittyi muistin varaamiseen ja lopetetaan tähän
        // siivotaan talukot tyhjiksi.
        for (int level = 0; level < (*itsMaxUndoLevelPtr); level++)
        {
          if (itsUndoTable[level])
            delete[] itsUndoTable[level];
        }
        *itsMaxUndoLevelPtr = 0;
        delete[] itsUndoTable;
        itsUndoTable = nullptr;
        delete[] itsUndoTextTable;
        itsUndoTextTable = nullptr;

        throw;  // heitetään lopuksi poikkeus eteenpäin
      }
    }
  }
}
