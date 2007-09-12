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

static bool FillSoundingData(NFmiFastQueryInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiPoint &theLatlon)
{
	static NFmiString bsName("bsname");
	if(theInfo)
	{
		if(theInfo->IsGrid())
			return theSoundingData.FillSoundingData(theInfo, theTime, theInfo->OriginTime(), theLatlon, bsName);
	}
	return false;
}

bool NFmiSoundingIndexCalculator::IsSurfaceBasedSoundingIndex(FmiSoundingParameters theSoundingParameter)
{
	if(theSoundingParameter >= kSoundingParLCLSurBas && theSoundingParameter <= kSoundingParCAPE_TT_SurBas)
		return true;
	else
		return false;
}

static bool FillSurfaceValuesFromInfo(NFmiSmartInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiPoint &theLatLon)
{
	theInfo->Param(kFmiTemperature);
	float T = theInfo->InterpolatedValue(theLatLon);
	theInfo->Param(kFmiDewPoint);
	float Td = theInfo->InterpolatedValue(theLatLon);
	if(T != kFloatMissing && Td != kFloatMissing)
	{
		theSoundingData.SetTandTdSurfaceValues(T, Td);
		return true;
	}
	else
		return false;
}

// T�m� on ulkopuolista hila datan laskemista varten tehty metodi.
// theBaseInfo:ssa on projektio ja hila pohja. T�m� hila pit�� laskea theValues parametriin.
// theDrawParam parametri antaa tiedot halutusta mallista, datatyypist� esim. EC mallipinta.
// theTime kertoo mihin ajan hetkeen parametri lasketaan.
// Oletus: theValues on jo tehty oikean kokoiseksi (theBaseInfo:n mukaan) ja t�ytetty puuttuvilla arvoilla.
void NFmiSoundingIndexCalculator::Calc(NFmiSmartInfo *theBaseInfo, NFmiInfoOrganizer *theInfoOrganizer, NFmiDrawParam *theDrawParam, NFmiDataMatrix<float> &theValues, const NFmiMetTime &theTime, const NFmiDataMatrix<float> &theObsDataT, const NFmiDataMatrix<float> &theObsDataTd, bool fObsDataFound)
{
	NFmiDrawParam tempDrawParam(*theDrawParam); // tehd��n v�liaikais kopio drawParamista ett� voidaan muuttaa data tyyppi (soundingParamista pois) oikean l�hde datan hakua varten
	tempDrawParam.DataType(static_cast<NFmiInfoData::Type>(theDrawParam->DataType() - NFmiInfoData::kSoundingParameterData)); // pit�� v�hent�� kSoundingParameterData ett� saadaan tiet�� mink� tyyppist� dataa laskuissa halutaan k�ytt��
	tempDrawParam.Param(NFmiDataIdent(NFmiParam(kFmiTemperature, "T"), *(theDrawParam->Param().GetProducer()))); // haetaan parametria l�mp�tila, t�ll� ei ole merkityst�, mutta jotain parametria vain pit�� vain hakea (ja T l�ytyy aina?)
	NFmiSmartInfo *info = theInfoOrganizer->Info(tempDrawParam, true); // pit�� olla true, ett� haetaan crosssection dataa, jolloin levelill� ei ole v�li�

	FmiSoundingParameters soundingParameter = static_cast<FmiSoundingParameters>(theDrawParam->Param().GetParamIdent());
	bool surfaceBasedCalculation = IsSurfaceBasedSoundingIndex(soundingParameter); // onko surfacebased???
	NFmiSmartInfo *analyzeData = 0;
	bool useAnalyzeData = false;
	if(surfaceBasedCalculation)
	{
		// onko analysi dataa halutulle ajalle ja halutut parametrit? jos on k�ytet��n sit�
		analyzeData = theInfoOrganizer->AnalyzeDataInfo();
		if(analyzeData && analyzeData->Time(theTime))
		{
			if(analyzeData->Param(kFmiTemperature) && analyzeData->Param(kFmiDewPoint)) // pit�� l�yty� my�s T ja Td parametrit ett� olisi jotain hy�ty�
				useAnalyzeData = true;
		}
	}

	if(surfaceBasedCalculation == true && (useAnalyzeData == false && fObsDataFound == false))
		return ; // ei mit��n teht�viss� kun lasketaan surfacebasedlaskuja, joten 'palautetaan' puuttuva matriisi

	if(info && theBaseInfo && theBaseInfo->IsGrid())
	{
		unsigned long xnum = theBaseInfo->Grid()->XNumber();
		unsigned long counter = 0;
		NFmiSoundingData soundingData;
		for(theBaseInfo->ResetLocation(); theBaseInfo->NextLocation(); counter++)
		{
			bool surfaceBaseStatus = false;
			FillSoundingData(info, soundingData, theTime, *(theBaseInfo->Location())); // t�ytet��n luotaus datat yksi kerrallaan
			if(useAnalyzeData)
				surfaceBaseStatus = FillSurfaceValuesFromInfo(analyzeData, soundingData, theBaseInfo->LatLon()); // t�yt� pinta arvot analyysist�
			else if(fObsDataFound)
				; // t�yt� pinta arvot obs-datasta
			if(surfaceBasedCalculation && surfaceBaseStatus == false)
				continue; // jos esim. analyysi/havainto pinta data ei l�ytynyt t�st� kohtaa, j�� puuttuva arvo voimaan

			// HUOM!!!! muista muuttaa luotaus-parametri pelk�ksi surface arvoksi, koska loppu menee itsest��n sitten
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
	case kSoundingParLCLSurBas:
		value = CalcLCLIndex(theSoundingData, kLCLCalcSurface);
		break;
	case kSoundingParCAPESur:
	case kSoundingParCAPESurBas:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcSurface);
		break;
	case kSoundingParCAPE0_3kmSur:
	case kSoundingParCAPE0_3kmSurBas:
		value = CalcCAPE500Index(theSoundingData, kLCLCalcSurface, 3000);
		break;
	case kSoundingParCAPE_TT_Sur:
	case kSoundingParCAPE_TT_SurBas:
		value = CalcCAPE_TT_Index(theSoundingData, kLCLCalcSurface, -10, -40);
		break;
	case kSoundingParCINSur:
	case kSoundingParCINSurBas:
		value = CalcCINIndex(theSoundingData, kLCLCalcSurface);
		break;
	case kSoundingParLCLHeightSur:
	case kSoundingParLCLHeightSurBas:
		value = CalcLCLHeightIndex(theSoundingData, kLCLCalcSurface);
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
	case kSoundingParCAPE_TT_500m:
		value = CalcCAPE_TT_Index(theSoundingData, kLCLCalc500m2, -10, -40);
		break;
	case kSoundingParCIN500m:
		value = CalcCINIndex(theSoundingData, kLCLCalc500m2);
		break;
	case kSoundingParLCLHeight500m:
		value = CalcLCLHeightIndex(theSoundingData, kLCLCalc500m2);
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
	case kSoundingParCAPE_TT_MostUn:
		value = CalcCAPE_TT_Index(theSoundingData, kLCLCalcMostUnstable, -10, -40);
		break;
	case kSoundingParCINMostUn:
		value = CalcCINIndex(theSoundingData, kLCLCalcMostUnstable);
		break;
	case kSoundingParLCLHeightMostUn:
		value = CalcLCLHeightIndex(theSoundingData, kLCLCalcMostUnstable);
		break;

	// **** 3. indeksit jotka lasketaan jonkun muun indeksin yhteydess�, tarvitaan my�s mahdollisesti pintakerros lasku tyyppi ja soundingdata ***
	case kSoundingParLFCSur:
		value = CalcLFCIndex(theSoundingData, kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELSur:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalcSurface, value);
		break;
	case kSoundingParLFCHeightSur:
		value = CalcLFCHeightIndex(theSoundingData, kLCLCalcSurface, xxxxValue);
		break;
	case kSoundingParELHeightSur:
		xxxxValue = CalcLFCHeightIndex(theSoundingData, kLCLCalcSurface, value);
		break;

	case kSoundingParLFC500m:
		value = CalcLFCIndex(theSoundingData, kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParEL500m:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalc500m2, value);
		break;
	case kSoundingParLFCHeight500m:
		value = CalcLFCHeightIndex(theSoundingData, kLCLCalc500m2, xxxxValue);
		break;
	case kSoundingParELHeight500m:
		xxxxValue = CalcLFCHeightIndex(theSoundingData, kLCLCalc500m2, value);
		break;

	case kSoundingParLFCMostUn:
		value = CalcLFCIndex(theSoundingData, kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELMostUn:
		xxxxValue = CalcLFCIndex(theSoundingData, kLCLCalcMostUnstable, value);
		break;
	case kSoundingParLFCHeightMostUn:
		value = CalcLFCHeightIndex(theSoundingData, kLCLCalcMostUnstable, xxxxValue);
		break;
	case kSoundingParELHeightMostUn:
		xxxxValue = CalcLFCHeightIndex(theSoundingData, kLCLCalcMostUnstable, value);
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

float NFmiSoundingIndexCalculator::Calc(NFmiFastQueryInfo *theInfo, const NFmiPoint &theLatlon, const NFmiMetTime &theTime, FmiSoundingParameters theParam)
{
	NFmiSoundingData soundingData;
	if(::FillSoundingData(theInfo, soundingData, theTime, theLatlon))
		return Calc(soundingData, theParam);
	return kFloatMissing;
}
