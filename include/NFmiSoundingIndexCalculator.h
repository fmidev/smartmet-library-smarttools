// ======================================================================
/*!
 * \file NFmiSoundingIndexCalculator.h
 *
 * T�m� luokka laskee erilaisia luotausi ndeksej� annettujen querinfojen avulla.
 * Mm. CAPE, CIN, LCL, BulkShear StormRelatedHellicity jne.
 */
// ======================================================================

#ifndef NFMISOUNDINGINDEXCALCULATOR_H
#define NFMISOUNDINGINDEXCALCULATOR_H

#include "NFmiDataMatrix.h"

class NFmiSmartInfo;
class NFmiFastQueryInfo;
class NFmiSoundingData;
class NFmiDrawParam;
class NFmiMetTime;
class NFmiInfoOrganizer;
class NFmiLocation;
class NFmiPoint;

// N�m� parametrit ovat niit� joita lasketaan luotausn�yt�ss�.
// Tarkoitus on nyt pysty� laskemaan niit� my�s hilamuodossa karttan�yt�lle.
// Sit� varten tein oman parametrilistan, joka kuuluisi NFmiParameterName-enumiin.
// Mutta en halua sotkea sit� n�ill�, joten tein oman listan, jota kuitenkin k�ytet��n
// kuin se olisi originaali NFmiParameterName-enumissa.
typedef enum
{
	kSoundingParNone = 4619, // aloitan luvut jostain numerosta mik� ei mene sekaisin originaali param-listan kanssa
	// aluksi surface (sur) arvojen avulla lasketut parametrit
	kSoundingParLCLSur = 4720,
	kSoundingParLFCSur,
	kSoundingParELSur,
	kSoundingParCAPESur,
	kSoundingParCAPE0_3kmSur,
	kSoundingParCINSur,
	kSoundingParLCLHeightSur,
	kSoundingParLFCHeightSur,
	kSoundingParELHeightSur,
	kSoundingParCAPE_TT_Sur, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten 500m arvojen avulla lasketut parametrit
	kSoundingParLCL500m = 4730,
	kSoundingParLFC500m,
	kSoundingParEL500m,
	kSoundingParCAPE500m,
	kSoundingParCAPE0_3km500m,
	kSoundingParCIN500m,
	kSoundingParLCLHeight500m,
	kSoundingParLFCHeight500m,
	kSoundingParELHeight500m,
	kSoundingParCAPE_TT_500m, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten mostunstable (MostUn) arvojen avulla lasketut parametrit
	kSoundingParLCLMostUn = 4740,
	kSoundingParLFCMostUn,
	kSoundingParELMostUn,
	kSoundingParCAPEMostUn,
	kSoundingParCAPE0_3kmMostUn,
	kSoundingParCINMostUn,
	kSoundingParLCLHeightMostUn,
	kSoundingParLFCHeightMostUn,
	kSoundingParELHeightMostUn,
	kSoundingParCAPE_TT_MostUn, // cape -10 ja -40 asteen kerroksen l�pi
	// sitten normaaleja yleis indeksej�
	kSoundingParSHOW = 4750,
	kSoundingParLIFT,
	kSoundingParKINX,
	kSoundingParCTOT,
	kSoundingParVTOT,
	kSoundingParTOTL,
	// sitten viel� 'speciaali' paramereja
	kSoundingParBS0_6km = 4770, // BS=bulkshear
	kSoundingParBS0_1km,
	kSoundingParSRH0_3km, // SRH=storm related helicity
	kSoundingParSRH0_1km,
	kSoundingParWS1500m,
	kSoundingParThetaE0_3km,
	// sitten obs-surface-based (SurBas) arvojen avulla lasketut parametrit
	kSoundingParLCLSurBas = 4780,
	kSoundingParLFCSurBas,
	kSoundingParELSurBas,
	kSoundingParCAPESurBas,
	kSoundingParCAPE0_3kmSurBas,
	kSoundingParCINSurBas,
	kSoundingParLCLHeightSurBas,
	kSoundingParLFCHeightSurBas,
	kSoundingParELHeightSurBas,
	kSoundingParCAPE_TT_SurBas // cape -10 ja -40 asteen kerroksen l�pi
} FmiSoundingParameters;

class NFmiSoundingIndexCalculator
{
public:

	static bool IsSurfaceBasedSoundingIndex(FmiSoundingParameters theSoundingParameter);
	static bool FillSoundingData(NFmiFastQueryInfo *theInfo, NFmiSoundingData &theSoundingData, const NFmiMetTime &theTime, const NFmiLocation &theLocation, NFmiFastQueryInfo* theGroundDataInfo);
	static void Calc(NFmiSmartInfo *theBaseInfo, NFmiInfoOrganizer *theInfoOrganizer, NFmiDrawParam *theDrawParam, NFmiDataMatrix<float> &theValues, const NFmiMetTime &theTime, const NFmiDataMatrix<float> &theObsDataT, const NFmiDataMatrix<float> &theObsDataTd, bool fObsDataFound);
	static float Calc(NFmiSoundingData &theSoundingData, FmiSoundingParameters theParam);
	static float Calc(NFmiFastQueryInfo *theInfo, const NFmiPoint &theLatlon, const NFmiMetTime &theTime, FmiSoundingParameters theParam);
};

#endif // NFMISOUNDINGINDEXCALCULATOR_H
