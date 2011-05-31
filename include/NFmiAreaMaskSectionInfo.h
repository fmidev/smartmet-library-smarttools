#pragma once
//**********************************************************
// C++ Class Name : NFmiAreaMaskSectionInfo 
// ---------------------------------------------------------
//  Author         : pietarin 
//  Creation Date  : Thur - Jun 20, 2002 
// 
// Sis�lt�� listan areaMaskInfoja, esim. IF-lauseen maskihan voi olla vaikka:
// IF(T>2) tai IF(T>2 && P<1012) jne.
//**********************************************************

#include "NFmiDataMatrix.h" // t��lt� tulee my�s checkedVector
#include <string>
#include "boost/shared_ptr.hpp"

class NFmiAreaMaskInfo;

class NFmiAreaMaskSectionInfo 
{

public:

	NFmiAreaMaskSectionInfo();
	~NFmiAreaMaskSectionInfo();

	boost::shared_ptr<NFmiAreaMaskInfo> MaskInfo(int theIndex);
	void Add(boost::shared_ptr<NFmiAreaMaskInfo> &theMask);
	checkedVector<boost::shared_ptr<NFmiAreaMaskInfo> >& GetAreaMaskInfoVector(void) {return itsAreaMaskInfoVector;}
	const std::string& GetCalculationText(void){return itsCalculationText;}
	void SetCalculationText(const std::string& theText){itsCalculationText = theText;}

private:
	checkedVector<boost::shared_ptr<NFmiAreaMaskInfo> > itsAreaMaskInfoVector;
	std::string itsCalculationText; // originaali teksti, mist� t�m� lasku on tulkittu

};

