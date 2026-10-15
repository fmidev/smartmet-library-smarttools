#pragma once

#include <cstddef>
#include <deque>
#include <string>

class NFmiRawData;

class NFmiUndoRedoQData
{
 public:
  NFmiUndoRedoQData();
  ~NFmiUndoRedoQData();
  // Deep copies: the undo buffers are owned
  NFmiUndoRedoQData(const NFmiUndoRedoQData& theOther);
  NFmiUndoRedoQData& operator=(const NFmiUndoRedoQData& theOther);

  bool SnapShotData(const std::string& theAction, const NFmiRawData& theRawData);
  void RearrangeUndoTable();
  bool Undo();
  bool Redo();
  bool UndoData(NFmiRawData& theRawData, std::string& modificationDescription);
  bool RedoData(NFmiRawData& theRawData, std::string& modificationDescription);
  void UndoLevel(long theDepth, const NFmiRawData& theRawData);

 private:
  void Clear();
  void Swap(NFmiUndoRedoQData& theOther) noexcept;

  long* itsMaxUndoLevelPtr;
  long* itsMaxRedoLevelPtr;
  int* itsCurrentUndoLevelPtr;
  int* itsCurrentRedoLevelPtr;

  char** itsUndoTable;
  std::string* itsUndoTextTable;
  std::size_t itsBufferSize = 0;  // size of each undo buffer in bytes
};
