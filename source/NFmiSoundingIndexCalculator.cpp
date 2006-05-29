// ======================================================================
/*!
 * \file NFmiSoundingIndexCalculator.cpp
 *
 * T�m� luokka laskee erilaisia luotausi ndeksej� annettujen querinfojen avulla.
 * Mm. CAPE, CIN, LCL, BulkShear StormRelatedHellicity jne.
 */
// ======================================================================

#include "NFmiSoundingIndexCalculator.h"
#include "NFmiSmartInfo.h"
#include "NFmiDrawParam.h"
#include "NFmiGrid.h"
#include "NFmiInfoOrganizer.h"
#include "NFmiSoundingData.h"
#include "NFmiSoundingFunctions.h"

using namespace NFmiSoundingFunctions;

bool NFmiSoundingIndexCalculator::FillSoundingData(NFmiFastQueryInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation)
{
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation.GetLocation(), theLocation.GetName());
		else
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLocation);
	}
	return false;
}

static bool FillSoundingData(NFmiQueryInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiPoint &theLatlon)
{
	static NFmiString bsName("bsname");
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLatlon, bsName);
	}
	return false;
}

// T�m� on ulkopuolista hila datan laskemista varten tehty metodi.
// theBaseInfo:ssa on projektio ja hila pohja. T�m� hila pit�� laskea theValues parametriin.
// theDrawParam parametri antaa tiedot halutusta mallista, datatyypist� esim. EC mallipinta.
// theTime kertoo mihin ajan hetkeen parametri lasketaan.
// Oletus: theValues on jo tehty oikean kokoiseksi (theBaseInfo:n mukaan) ja t�ytetty puuttuvilla arvoilla.
void NFmiSoundingIndexCalculator::Calc(NFmiSmartInfo *theBaseInfo, NFmiInfoOrganizer *theInfoOrganizer, NFmiDrawParam *theDrawParam, NFmiDataMatrix<float> &theValues, const NFmiMetTime &theTime)
{
	NFmiDrawParam tempDrawParam(*theDrawParam); // tehd��n v�liaikais kopio drawParamista ett� voidaan muuttaa data tyyppi (soundingParamista pois) oikean l�hde datan hakua varten
	tempDrawParam.DataType(static_cast<NFmiInfoData::Type>(theDrawParam->DataType() - NFmiInfoData::kSoundingParameterData)); // pit�� v�hent�� kSoundingParameterData ett� saadaan tiet�� mink� tyyppist� dataa laskuissa halutaan k�ytt��
	tempDrawParam.Param(NFmiDataIdent(NFmiParam(kFmiTemperature, "T"), *(theDrawParam->Param().GetProducer()))); // haetaan parametria l�mp�tila, t�ll� ei ole merkityst�, mutta jotain parametria vain pit�� vain hakea (ja T l�ytyy aina?)
	NFmiSmartInfo *info = theInfoOrganizer->Info(tempDrawParam, true); // pit�� olla true, ett� haetaan crosssection dataa, jolloin levelill� ei ole v�li�
	if(info && theBaseInfo && theBaseInfo->IsGrid())
	{
		unsigned long xnum = theBaseInfo->Grid()->XNumber();
		unsigned long counter = 0;
		NFmiSoundingData soundingData;
		for(theBaseInfo->ResetLocation(); theBaseInfo->NextLocation(); counter++)
		{
			FillSoundingData(info, soundingData, theTime, *(theBaseInfo->Location())); // t�ytet��n luotaus datat yksi kerrallaan
			float value = Calc(soundingData, static_cast<FmiSoundingParameters>(theDrawParam->Param().GetParamIdent()));
			theValues[counter % xnum][counter / xnum] = value;
		}
	}
}

float NFmiSoundingIndexCalculator::Calc(NFmiSoundingData &theSoundingData, FmiSoundingParameters theParam)
{
	double value = kFloatMissing;
	double xxxxValue = kFloatMissing; // t�m� on ns. hukka parametri, koska jotkut parametrit syntyv�t sivutuotteena ja t�h�n sijoitetaan aina se ei haluttu parametri
	switch(theParam)
	{
	// **** 1. yksinkertaiset indeksit, tarvitaan vain soundingdata ***
	case kSoundingParSHOW:
		value = CalcSHOWIndex(theSoundingData);
		break;
	case kSoundingParLIFT:
		value = CalcLIFTIndex(theSoundingData);
		break;
	case kSoundingParKINX:
		value = CalcKINXIndex(theSoundingData);
		break;
	case kSoundingParCTOT:
		value = CalcCTOTIndex(theSoundingData);
		break;
	case kSoundingParVTOT:
		value = CalcVTOTIndex(theSoundingData);
		break;
	case kSoundingParTOTL:
		value = CalcTOTLIndex(theSoundingData);
		break;

	// **** 2. indeksit joissa tarvitaan my�s pintakerros lasku tyyppi soundingdatan lis�ksi ja mahd. korkeus parametri ***
	// **** surface ****
	case kSoundingParLCLSur:
		value = CalcLCLIndex(theSoundingData, kLCLCalcSurface);
		break;
	case kSoundingParCAPESur:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcSurface);
		break;
	case kSoundingParCAPE0_3kmSur:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcSurface, 3000);
		break;
	case kSoundingParCINSur:
		value = CalcCINIndex(theSoundingData, kLCLCalcSurface);
		break;

	// **** 500 m mixing ****
	case kSoundingParLCL500m:
		value = CalcLCLIndex(theSoundingData, kLCLCalc500m2);
		break;
	case kSoundingParCAPE500m:
		value = CalcCAPE500Index(theSoundingData, kLCLCalc500m2);
		break;
	case kSoundingParCAPE0_3km500m:
		value = CalcCAPE500Index(theSoundingData, kLCLCalc500m2, 3000);
		break;
	case kSoundingParCIN500m:
		value = CalcCINIndex(theSoundingData, kLCLCalc500m2);
		break;

	// **** most unstable case ****
	case kSoundingParLCLMostUn:
		value = CalcLCLIndex(theSoundingData, kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPEMostUn:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcMostUnstable);
		break;
	case kSoundingParCAPE0_3kmMostUn:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcMostUnstable, 3000);
		break;
	case kSoundingParCINMostUn:
		value = CalcCINIndex(theSoundingData, kLCLCalcMostUnstable);
		break;

	// **** 3. indeksit jotka lasketaan jonkun muun indeksin yhteydess�, tarvitaan my�s mahdollisesti pintakerros lasku tyyppi ja soundingdata ***
	case kSoundingParLFCSur:
		value = CalcLFCIndex(theSoundingData, kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELSur:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalcSurface, value);
		break;

	case kSoundingParLFC500m:
		value = CalcLFCIndex(theSoundingData, kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParEL500m:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalc500m2, value);
		break;

	case kSoundingParLFCMostUn:
		value = CalcLFCIndex(theSoundingData, kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELMostUn:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalcMostUnstable, value);
		break;

	// **** 4. indeksit joiden laskuissa tarvitaan korkeus parametreja ja soundingdata ***
	case kSoundingParBS0_6km:
		value = CalcBulkShearIndex(theSoundingData, 0, 6);
		break;
	case kSoundingParBS0_1km:
		value = CalcBulkShearIndex(theSoundingData, 0, 1);
		break;
	case kSoundingParSRH0_3km:
		value = CalcSRHIndex(theSoundingData, 0, 3);
		break;
	case kSoundingParSRH0_1km:
		value = CalcSRHIndex(theSoundingData, 0, 1);
		break;
	case kSoundingParWS1500m:
		value = CalcWSatHeightIndex(theSoundingData, 1500);
		break;
	case kSoundingParThetaE0_3km:
		value = CalcThetaEDiffIndex(theSoundingData, 0, 3);
		break;
	case kSoundingParNone:
		value = kFloatMissing;
		break;
	}

	return static_cast<float>(value);
}

float NFmiSoundingIndexCalculator::Calc(NFmiQueryInfo *theInfo, const NFmiPoint &theLatlon, const NFmiMetTime &theTime, FmiSoundingParameters theParam)
{
	NFmiSoundingData soundingData;
	if(::FillSoundingData(theInfo, soundingData, theTime, theLatlon))
		return Calc(soundingData, theParam);
	return kFloatMissing;
}
