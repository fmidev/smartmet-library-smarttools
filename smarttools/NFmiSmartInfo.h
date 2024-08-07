#pragma once

// Tämä luokka tulee nykyisen NFmiSmartInfo-luokan tilalle.
// Uusi luokka on tarkoitettu vain SmartMetissa olevan
// editoitavan datan käsittely. Eli täällä on tietoa mm.
// valitusta maskista (selction, display jne.).
// Undo/Redo toiminnot.
// Luokka tehdään uusiksi siksi, että sitä yksinkertaistetaan
// ja että esim. undo/redo datojen omistus ja tuhoaminen tehdään
// automaattisesti smart-pointtereilla eikä niin kuin nykyisin
// erillisen DestroyData-funktion avulla.
// TODO: keksi parempi nimi, toi poista lopuksi NFmiSmartInfo
// -luokka ja laita tämä sen nimiseksi.

#include "NFmiOwnerInfo.h"

class NFmiModifiableQDatasBookKeeping;

class NFmiSmartInfo : public NFmiOwnerInfo
{
 public:
  NFmiSmartInfo();
  NFmiSmartInfo(const NFmiOwnerInfo &theInfo);  // matala kopio, eli jaettu data
  NFmiSmartInfo(const NFmiSmartInfo &theInfo);  // matala kopio, eli jaettu data
  NFmiSmartInfo(NFmiQueryData *theOwnedData,
                NFmiInfoData::Type theDataType,
                const std::string &theDataFileName,
                const std::string &theDataFilePattern);  // ottaa datan omistukseensa emossa
  ~NFmiSmartInfo();

  NFmiSmartInfo &operator=(const NFmiSmartInfo &theInfo);  // matala kopio, eli jaettu data
  NFmiSmartInfo *Clone(
      void) const;  // syvä kopio, eli kloonille luodaan oma queryData sen omistukseen
                    // TODO: katso pitääkö metodin nimi muuttaa, koska emoissa Clone on
                    // virtuaali funktio, jossa eri paluu-luokka.
  // Apu funktio tekemään kevyitä kopoioita, koska Clone tekee raskaan kopion (= data kopioidaan
  // myös)
  static std::shared_ptr<NFmiFastQueryInfo> CreateShallowCopyOfHighestInfo(
      const std::shared_ptr<NFmiFastQueryInfo> &theInfo);
  bool NextLocation();

  bool SnapShotData(const std::string &theAction);
  bool Undo();
  bool Redo();
  bool UndoData(std::string &modificationDescription);
  bool RedoData(std::string &modificationDescription);
  void UndoLevel(long theDepth);

  bool LocationSelectionSnapShot();                      // ota maskit talteen
  bool LocationSelectionUndo();                          // kysyy onko undo mahdollinen
  bool LocationSelectionRedo();                          // kysyy onko redo mahdollinen
  bool LocationSelectionUndoData();                      // suorittaa todellisen undon
  bool LocationSelectionRedoData();                      // suorittaa todellisen redon
  void LocationSelectionUndoLevel(int theNewUndoLevel);  // undolevel asetetaan tällä
  bool LoadedFromFile();
  void LoadedFromFile(bool loadedFromFile);
  bool IsDirty() const;
  void Dirty(bool newState);

  int MaskedCount(unsigned long theMaskType, unsigned long theIndex, const NFmiRect &theSearchArea);
  void InverseMask(unsigned long theMaskType);
  void MaskAllLocations(const bool &newState, unsigned long theMaskType);
  unsigned long LocationMaskedCount(unsigned long theMaskType);
  bool Mask(const NFmiBitMask &theMask, unsigned long theMaskType);
  const NFmiBitMask &Mask(unsigned long theMaskType) const;
  void MaskLocation(const bool &newState, unsigned long theMaskType);
  void MaskType(unsigned long theMaskType);
  unsigned long MaskType();

 protected:
  void CopyClonedDatas(const NFmiSmartInfo &theOther);

  std::shared_ptr<NFmiModifiableQDatasBookKeeping> itsQDataBookKeepingPtr;
};
