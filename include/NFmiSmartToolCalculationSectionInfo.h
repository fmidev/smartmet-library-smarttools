#pragma once
//**********************************************************
// C++ Class Name : NFmiSmartToolCalculationSectionInfo 
// ---------------------------------------------------------
//  Author         : pietarin 
//  Creation Date  : 8.11. 2010 
// 
// Sis�lt�� joukon smarttool laskuja, jotka kuuluvat yhteen blokkiin. Esim.
// 
// T = T +1
// P = P * 0.99
// 
// Yksi rivi on aina yksi lasku ja laskussa pit�� olla sijoitus johonkin parametriin (=).
//**********************************************************

#include "NFmiDataMatrix.h"
#include <set>
#include "boost/shared_ptr.hpp"

class NFmiSmartToolCalculationInfo;

class NFmiSmartToolCalculationSectionInfo 
{

public:

	NFmiSmartToolCalculationSectionInfo(void);
	~NFmiSmartToolCalculationSectionInfo(void);

	void Clear(void);
	void AddCalculationInfo(boost::shared_ptr<NFmiSmartToolCalculationInfo> &value);
	checkedVector<boost::shared_ptr<NFmiSmartToolCalculationInfo> >& GetCalculationInfos(void){return itsSmartToolCalculationInfoVector;}
	void AddModifiedParams(std::set<int> &theModifiedParams);

private:
	checkedVector<boost::shared_ptr<NFmiSmartToolCalculationInfo> > itsSmartToolCalculationInfoVector;

};
