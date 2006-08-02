//**********************************************************
// C++ Class Name : NFmiSmartToolCalculationSection 
// ---------------------------------------------------------
// Filetype: (HEADER)
// Filepath: G:/siirto/marko/oc/NFmiSmartToolCalculationSection.h 
// 
// 
// GDPro Properties 
// ---------------------------------------------------
//  - GD Symbol Type    : CLD_Class 
//  - GD Method         : UML ( 4.0 ) 
//  - GD System Name    : aSmartTools 
//  - GD View Type      : Class Diagram 
//  - GD View Name      : smarttools 1 
// ---------------------------------------------------  
//  Author         : pietarin 
//  Creation Date  : Thur - Jun 20, 2002 
// 
//  Change Log     : 
// 
// T�m� luokka hoitaa calculationsectionin yksi laskurivi kerrallaan.
// esim. 
// T = T + 1
// P = P + 2
//**********************************************************
#ifndef  NFMISMARTTOOLCALCULATIONSECTION_H
#define  NFMISMARTTOOLCALCULATIONSECTION_H

#include "NFmiDataMatrix.h"

class NFmiPoint;
class NFmiMetTime;
class NFmiSmartToolCalculation;
class NFmiSmartInfo;
class NFmiMacroParamValue;

class NFmiSmartToolCalculationSection 
{

public:
	void Calculate(const NFmiPoint &theLatlon, unsigned long theLocationIndex, const NFmiMetTime &theTime, int theTimeIndex, NFmiMacroParamValue &theMacroParamValue);
	void Clear(void);
	void SetTime(const NFmiMetTime &theTime);
	NFmiSmartInfo* FirstVariableInfo(void);

	NFmiSmartToolCalculationSection(void);
	~NFmiSmartToolCalculationSection(void);

	void AddCalculations(NFmiSmartToolCalculation* value);
	checkedVector<NFmiSmartToolCalculation*>* GetCalculations(void){return &itsCalculations;}

private:
	checkedVector<NFmiSmartToolCalculation*> itsCalculations; // omistaa+tuhoaa

};
#endif
