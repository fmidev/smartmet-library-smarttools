#pragma once
//**********************************************************
// C++ Class Name : NFmiSmartToolCalculationSectionInfo
// ---------------------------------------------------------
//  Author         : pietarin
//  Creation Date  : 8.11. 2010
//
// Sisältää joukon smarttool laskuja, jotka kuuluvat yhteen blokkiin. Esim.
//
// T = T +1
// P = P * 0.99
//
// Yksi rivi on aina yksi lasku ja laskussa pitää olla sijoitus johonkin parametriin (=).
//**********************************************************

#include <memory>
#include <map>
#include <set>
#include <vector>

class NFmiSmartToolCalculationInfo;

class NFmiSmartToolCalculationSectionInfo
{
 public:
  NFmiSmartToolCalculationSectionInfo();
  ~NFmiSmartToolCalculationSectionInfo();

  void Clear();
  void AddCalculationInfo(std::shared_ptr<NFmiSmartToolCalculationInfo> &value);
  std::vector<std::shared_ptr<NFmiSmartToolCalculationInfo> > &GetCalculationInfos()
  {
    return itsSmartToolCalculationInfoVector;
  }
  void AddModifiedParams(std::map<int, std::string> &theModifiedParams);

 private:
  std::vector<std::shared_ptr<NFmiSmartToolCalculationInfo> > itsSmartToolCalculationInfoVector;
};
