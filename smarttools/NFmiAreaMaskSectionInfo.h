#pragma once
//**********************************************************
// C++ Class Name : NFmiAreaMaskSectionInfo
// ---------------------------------------------------------
//  Author         : pietarin
//  Creation Date  : Thur - Jun 20, 2002
//
// Sisältää listan areaMaskInfoja, esim. IF-lauseen maskihan voi olla vaikka:
// IF(T>2) tai IF(T>2 && P<1012) jne.
//**********************************************************

#include <memory>
#include <newbase/NFmiDataMatrix.h>  // täältä tulee myös std::vector

class NFmiAreaMaskInfo;

class NFmiAreaMaskSectionInfo
{
 public:
  NFmiAreaMaskSectionInfo();
  ~NFmiAreaMaskSectionInfo();

  std::shared_ptr<NFmiAreaMaskInfo> MaskInfo(int theIndex);
  void Add(std::shared_ptr<NFmiAreaMaskInfo>& theMask);
  std::vector<std::shared_ptr<NFmiAreaMaskInfo> >& GetAreaMaskInfoVector()
  {
    return itsAreaMaskInfoVector;
  }
  const std::string& GetCalculationText() { return itsCalculationText; }
  void SetCalculationText(const std::string& theText) { itsCalculationText = theText; }

 private:
  std::vector<std::shared_ptr<NFmiAreaMaskInfo> > itsAreaMaskInfoVector;
  std::string itsCalculationText;  // originaali teksti, mistä tämä lasku on tulkittu
};
