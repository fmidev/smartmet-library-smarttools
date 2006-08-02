// ======================================================================
/*!
 * \file NFmiSoundingData.cpp
 *
 * Apuluokka laskemaan ja tutkimaan luotaus dataa. Osaa t�ytt�� itsens�
 * mm. mallipinta QueryDatasta (infosta).
 */
// ======================================================================

#include "NFmiSoundingData.h"
#include "NFmiFastQueryInfo.h"
#include "NFmiDataModifierAvg.h"
#include "NFmiInterpolation.h"
#include "NFmiSoundingFunctions.h"

using namespace NFmiSoundingFunctions;

// hakee l�himm�n sopivan painepinnan, mist� l�ytyy halutuille parametreille arvot
// Mutta ei sallita muokkausta ennen 1. validia leveli�!
bool NFmiSoundingData::GetTandTdValuesFromNearestPressureLevel(double P, double &theFoundP, double &theT, double &theTd)
{
	if(P != kFloatMissing)
	{
		checkedVector<float>&pV = GetParamData(kFmiPressure);
		checkedVector<float>&tV = GetParamData(kFmiTemperature);
		checkedVector<float>&tdV = GetParamData(kFmiDewPoint);
		if(pV.size() > 0) // oletus ett� parV on saman kokoinen kuin pV -vektori
		{
			double minDiffP = 999999;
			theFoundP = 999999;
			theT = kFloatMissing;
			theTd = kFloatMissing;
			bool foundLevel = false;
//			for(int i=ZeroHeightIndex(); i<static_cast<int>(pV.size()); i++)
			for(int i=0; i<static_cast<int>(pV.size()); i++)
			{
				if(i < 0)
					return false; // jos 'tyhj�' luotaus, on t�ss� aluksi -1 indeksin�
				if(pV[i] != kFloatMissing)
				{
					double pDiff = ::fabs(pV[i] - P);
					if(pDiff < minDiffP)
					{
						theFoundP = pV[i];
						minDiffP = pDiff;
						theT = tV[i];
						theTd = tdV[i];
						foundLevel = true;
					}
				}
			}
			if(foundLevel)
				return true;

			theFoundP = kFloatMissing; // 'nollataan' t�m� viel� varmuuden vuoksi
		}
	}
	return false;
}

// hakee l�himm�n sopivan painepinnan, mist� l�ytyy halutulle parametrille ei-puuttuva arvo
bool NFmiSoundingData::SetValueToPressureLevel(float P, float theParamValue, FmiParameterName theId)
{
	if(P != kFloatMissing)
	{
		checkedVector<float>&pV = GetParamData(kFmiPressure);
		checkedVector<float>&parV = GetParamData(theId);
		if(pV.size() > 0) // oletus ett� parV on saman kokoinen kuin pV -vektori
		{
			checkedVector<float>::iterator it = std::find(pV.begin(), pV.end(), P);
			if(it != pV.end())
			{
				int index = std::distance(pV.begin(), it);
				parV[index] = theParamValue;
				return true;
			}
		}
	}
	return false;
}

// paluttaa paine arvon halutulle metri korkeudelle
float NFmiSoundingData::GetPressureAtHeight(double H)
{
	if(H == kFloatMissing)
		return kFloatMissing;

	double maxDiffInH = 100; // jos ei voi interpoloida, pit�� l�ydetyn arvon olla v�hint�in n�in l�hell�, ett� hyv�ksyt��n
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&hV = GetParamData(kFmiGeomHeight);
	float value = kFloatMissing;
	if(hV.size() > 0 && pV.size() == hV.size())
	{
		unsigned int ssize = pV.size();
		float lastP = kFloatMissing;
		float lastH = kFloatMissing;
		float currentP = kFloatMissing;
		float currentH = kFloatMissing;
		bool goneOverWantedHeight = false;
		for(unsigned int i=0; i<ssize; i++)
		{
			currentP = pV[i];
			currentH = hV[i];
			if(currentH != kFloatMissing)
			{
				if(currentH > H)
				{
					if(currentP != kFloatMissing)
					{
						goneOverWantedHeight = true;
						break;
					}
				}
				if(currentP != kFloatMissing)
				{
					lastP = currentP;
					lastH = currentH;
					if(currentH == H)
						return currentP; // jos oli tarkka osuma, turha jatkaa
				}
			}
		}
		if(goneOverWantedHeight && lastP != kFloatMissing && currentP != kFloatMissing && lastH != kFloatMissing && currentH != kFloatMissing)
		{ // interpoloidaan arvo kun l�ytyi kaikki arvot
			value = static_cast<float>(NFmiInterpolation::Linear(H, currentH, lastH, currentP, lastP));
		}
		else if(lastP != kFloatMissing && lastH != kFloatMissing && ::fabs(lastH - H) < maxDiffInH)
			value = lastP;
		else if(currentP != kFloatMissing && currentH != kFloatMissing && ::fabs(currentH - H) < maxDiffInH)
			value = currentP;
	}
	return value;
}

// Luotausten muokkausta varten pit�� tiet��n onko ensimm�inen luotaus vaihtunut (paikka, aika, origin-aika).
// Jos ei ole ja luotausta on modifioitu, piirret��n modifioitu, muuten nollataan 'modifioitu'
// luotaus ja t�ytet��n se t�ll� uudella datalla.
bool NFmiSoundingData::IsSameSounding(const NFmiSoundingData &theOtherSounding)
{
	if(Location() == theOtherSounding.Location())
		if(Time() == theOtherSounding.Time())
			if(OriginTime() == theOtherSounding.OriginTime())
				return true;
	return false;
}

// hakee ne arvot h, u ja v parametreista, mitk� ovat samalta korkeudelta ja mitk�
// eiv�t ole puuttuvia. Haetaan alhaalta yl�s p�in arvoja
bool NFmiSoundingData::GetLowestNonMissingValues(float &H, float &U, float &V)
{
	checkedVector<float>&hV = GetParamData(kFmiGeomHeight);
	checkedVector<float>&uV = GetParamData(kFmiWindUMS);
	checkedVector<float>&vV = GetParamData(kFmiWindVMS);
	if(hV.size() > 0 && hV.size() == uV.size() && hV.size() == vV.size())
	{
		for(int i=0; i<static_cast<int>(hV.size()); i++)
		{
			if(hV[i] != kFloatMissing && uV[i] != kFloatMissing && vV[i] != kFloatMissing)
			{
				H = hV[i];
				U = uV[i];
				V = vV[i];
				return true;
			}
		}
	}
	return false;
}

// laskee halutun parametrin arvon haluttuun metri korkeuteen
float NFmiSoundingData::GetValueAtHeight(FmiParameterName theId, float H)
{
	float P = GetPressureAtHeight(H);
	if(P == kFloatMissing)
		return kFloatMissing;

	return GetValueAtPressure(theId, P);
}

// Hakee halutun parametrin arvon halutulta painekorkeudelta.
float NFmiSoundingData::GetValueAtPressure(FmiParameterName theId, float P)
{
	if(P == kFloatMissing)
		return kFloatMissing;

	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&paramV = GetParamData(theId);
	float value = kFloatMissing;
	if(paramV.size() > 0 && pV.size() == paramV.size())
	{
		unsigned int ssize = pV.size();
		float lastP = kFloatMissing;
		float lastValue = kFloatMissing;
		float currentP = kFloatMissing;
		float currentValue = kFloatMissing;
		for(unsigned int i=0; i<ssize; i++)
		{
			currentP = pV[i];
			currentValue = paramV[i];
			if(currentP != kFloatMissing)
			{
				if(currentP < P)
				{
					if(currentValue != kFloatMissing)
						break;
				}
				if(currentValue != kFloatMissing)
				{
					lastP = currentP;
					lastValue = currentValue;
					if(currentP == P)
						return currentValue; // jos oli tarkka osuma, turha jatkaa
				}
			}
		}
		if(lastP != kFloatMissing && currentP != kFloatMissing && lastValue != kFloatMissing && currentValue != kFloatMissing)
		{ // interpoloidaan arvo kun l�ytyi kaikki arvot
			value = CalcLogInterpolatedValue(lastP, currentP, P, lastValue, currentValue);
		}
		else if(lastP != kFloatMissing && lastValue != kFloatMissing)
			value = lastValue;
		else if(currentP != kFloatMissing && currentValue != kFloatMissing)
			value = currentValue;
	}
	return value;
}

// Laskee u ja v komponenttien keskiarvot halutulla v�lill�
// Huom! z korkeudet interpoloidaan, koska havaituissa luotauksissa niit� ei ole aina ja varsinkaan samoissa
// v�leiss� kuin tuulia
bool NFmiSoundingData::CalcAvgWindComponentValues(double fromZ, double toZ, double &u, double &v)
{
	u = kFloatMissing;
	v = kFloatMissing;
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	if(pV.size() > 0)
	{
		double hStep = 100; // k�yd��n dataa l�pi 100 metrin v�lein

		NFmiDataModifierAvg uAvg;
		NFmiDataModifierAvg vAvg;
		for(double h = fromZ; h < toZ + hStep/2.; h += hStep) // k�yd�� layeria 100 metrin v�lein
		{
			double pressure = GetPressureAtHeight(h);
			float tmpU = GetValueAtPressure(kFmiWindUMS, static_cast<float>(pressure));
			float tmpV = GetValueAtPressure(kFmiWindVMS, static_cast<float>(pressure));
			if(tmpU != kFloatMissing && tmpV != kFloatMissing)
			{
				uAvg.Calculate(tmpU);
				vAvg.Calculate(tmpV);
			}
		}
		u = uAvg.CalculationResult();
		v = vAvg.CalculationResult();
		return (u != kFloatMissing && v != kFloatMissing);
	}
	return false;
}

float NFmiSoundingData::FindPressureWhereHighestValue(FmiParameterName theId, float theMaxP, float theMinP)
{
	float maxValuePressure = kFloatMissing;
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&paramV = GetParamData(theId);
	if(pV.size() > 0)
	{
		float maxValue = -99999999.f;
		for(unsigned int i=0; i<pV.size(); i++)
		{
			float tmpP = pV[i];
			float tmpParam = paramV[i];
			if(tmpP != kFloatMissing && tmpParam != kFloatMissing)
			{
				if(tmpP <= theMaxP && tmpP >= theMinP) // eli ollaanko haluttujen pintojen v�liss�
				{
					if(tmpParam > maxValue)
					{
						maxValue = tmpParam;
						maxValuePressure = tmpP;
					}
				}

				if(tmpP < theMinP) // jos oltiin jo korkeammalla kuin minimi paine, voidaan lopettaa
					break;
			}
		}
	}
	return maxValuePressure;
}

// K�y l�pi luotausta ja etsi sen kerroksen arvot, jolta l�ytyy suurin theta-E ja
// palauta sen kerroksen T, Td ja P ja laskettu max Theta-e.
// Etsit��n arvoja jos pinta on alle theMinP-tason (siis alle tuon tason fyysisesti).
// HUOM! theMinP ei voi olla kFloatMissing, jos haluat ett� kaikki levelit k�yd��n l�pi laita sen arvoksi 0.
bool NFmiSoundingData::FindHighestThetaE(double &T, double &Td, double &P, double &theMaxThetaE, double theMinP)
{
	T = kFloatMissing;
	Td = kFloatMissing;
	P = kFloatMissing;
	theMaxThetaE = kFloatMissing;
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&tV = GetParamData(kFmiTemperature);
	checkedVector<float>&tdV = GetParamData(kFmiDewPoint);
	if(pV.size() > 0)
	{
		theMaxThetaE = -99999999;
		for(unsigned int i=0; i<pV.size(); i++)
		{
			float tmpP = pV[i];
			float tmpT = tV[i];
			float tmpTd = tdV[i];
			if(tmpP != kFloatMissing && tmpT != kFloatMissing && tmpTd != kFloatMissing)
			{
				if(tmpP >= theMinP) // eli ollaanko l�hempana maanpintaa kuin raja paine pinta on
				{
					double thetaE = CalcThetaE(tmpT, tmpTd, tmpP);
					if(thetaE != kFloatMissing && thetaE > theMaxThetaE)
					{
						theMaxThetaE = thetaE;
						T = tmpT;
						Td = tmpTd;
						P = tmpP;
					}
				}
				else // jos oltiin korkeammalla, voidaan lopettaa
					break;
			}
		}
	}
	return theMaxThetaE != kFloatMissing;
}

// T�m� on Pieter Groenemeijerin ehdottama tapa laskea LCL-laskuihin tarvittavia T, Td ja P arvoja yli halutun layerin.
// Laskee keskiarvot T:lle, Td:lle ja P:lle haluttujen korkeuksien v�lille.
// Eli laskee keskiarvon l�mp�tilalle potentiaali l�mp�tilojen avulla.
// Laskee kastepisteen keskiarvon mixing valueiden avulla.
// N�ille lasketaan keskiarvot laskemalla halutun layerin l�pi 1 hPa askelissa, ettei ep�m��r�iset
// n�yte valit painota mitenkaan laskuja.
// Paineelle otetaan suoraan pohja kerroksen arvo.
// Oletus fromZ ja toZ eiv�t ole puuttuvia.
bool NFmiSoundingData::CalcLCLAvgValues(double fromZ, double toZ, double &T, double &Td, double &P, bool fUsePotTandMix)
{
	T = kFloatMissing;
	Td = kFloatMissing;
	P = kFloatMissing;
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&tV = GetParamData(kFmiTemperature);
	checkedVector<float>&tdV = GetParamData(kFmiDewPoint);
	if(pV.size() > 0)
	{
		for(unsigned int i=0; i<pV.size(); i++)
			if(pV[i] != kFloatMissing && tV[i] != kFloatMissing && tdV[i] != kFloatMissing) // etsit��n 1. ei puuttuva paine arvo eli alin paine arvo (miss� my�s T ja Td arvot)
			{
				P = pV[i];
				break;
			}
		if(P == kFloatMissing)
			return false;
		int startP = FmiRound(GetPressureAtHeight(fromZ));
		int endP = FmiRound(GetPressureAtHeight(toZ));
		if(startP == kFloatMissing || endP == kFloatMissing || startP <= endP)
			return false;
		NFmiDataModifierAvg avgT; // riippuen moodista t�ss� lasketaan T tai Tpot keskiarvoa
		NFmiDataModifierAvg avgTd; // riippuen moodista t�ss� lasketaan Td tai w keskiarvoa
		for(int pressure = startP; pressure > endP; pressure--) // k�yd�� layeria yhden hPa:n v�lein l�pi
		{
			float temperature = GetValueAtPressure(kFmiTemperature, static_cast<float>(pressure));
			float dewpoint = GetValueAtPressure(kFmiDewPoint, static_cast<float>(pressure));
			if(temperature != kFloatMissing && dewpoint != kFloatMissing)
			{
				if(fUsePotTandMix)
				{
					avgT.Calculate(static_cast<float>(T2tpot(temperature, pressure)));
					avgTd.Calculate(static_cast<float>(CalcMixingRatio(temperature, dewpoint, pressure)));
				}
				else
				{
					avgT.Calculate(temperature);
					avgTd.Calculate(dewpoint);
				}
			}
		}
		if(avgT.CalculationResult() != kFloatMissing && avgTd.CalculationResult() != kFloatMissing )
		{
			if(fUsePotTandMix)
			{
				T = Tpot2t(avgT.CalculationResult(), P);
				Td = CalcDewPoint(T, avgTd.CalculationResult(), P);
			}
			else
			{
				T = avgT.CalculationResult();
				Td = avgTd.CalculationResult();
			}
			return (P != kFloatMissing && T != kFloatMissing && Td != kFloatMissing);
		}
	}
	return false;
}

// T�m� hakee annettua painearvoa l�himm�t arvot, jotka l�ytyv�t kaikille halutuille parametreille.
// Palauttaa true, jos l�ytyy dataa ja false jos ei l�ydy.
// OLETUS: maanpinta arvot ovat vektorin alussa, pit�isi tarkistaa??
bool NFmiSoundingData::GetValuesStartingLookingFromPressureLevel(double &T, double &Td, double &P)
{
	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&tV = GetParamData(kFmiTemperature);
	checkedVector<float>&tdV = GetParamData(kFmiDewPoint);
	if(pV.size() > 0)
	{
		for(unsigned int i=0; i<pV.size(); i++)
		{
			if(pV[i] != kFloatMissing)// && tV[i] != kFloatMissing && tdV[i] != kFloatMissing)
			{
				if(P >= pV[i] && tV[i] != kFloatMissing && tdV[i] != kFloatMissing)
				{
					T = tV[i];
					Td = tdV[i];
					P = pV[i];
					return true;
				}
			}
		}
	}
	return false;
}

// oletuksia paljon:
// theInfo on validi, aika ja paikka on jo asetettu
bool NFmiSoundingData::FillParamData(NFmiQueryInfo* theInfo, FmiParameterName theId)
{
	try
	{
		checkedVector<float>&data = GetParamData(theId);
		data.resize(theInfo->SizeLevels(), kFloatMissing); // alustetaan vektori puuttuvalla
		bool paramFound = theInfo->Param(theId);
		if(paramFound == false && theId == kFmiDewPoint)
			paramFound = theInfo->Param(kFmiDewPoint2M); // kastepiste data voi tulla luotausten yhteydess� t�ll� parametrilla ja mallidatan yhteydess� toisella
		if(paramFound)
		{
			int i = 0;
			for(theInfo->ResetLevel(); theInfo->NextLevel(); i++)
				data[i] = theInfo->FloatValue();
			// IK�V�� CASTI KOODIA!!!!
			if(static_cast<NFmiFastQueryInfo*>(theInfo)->HeightParamIsRising() == false) // jos ei nousevassa j�rjestyksess�, k��nnet��n vektorissa olevat arvot
				std::reverse(data.begin(), data.end());
			return true;
		}
	}
	catch(std::exception & /* e */)
	{
	}
	return false;
}

bool NFmiSoundingData::FillParamData(NFmiQueryInfo* theInfo, FmiParameterName theId, const NFmiMetTime& theTime, const NFmiPoint& theLatlon)
{
	bool status = false;
	checkedVector<float>&data = GetParamData(theId);
	data.resize(theInfo->SizeLevels(), kFloatMissing); // alustetaan vektori puuttuvalla
	if(theInfo->Param(theId))
	{
		int i = 0;
		for(theInfo->ResetLevel(); theInfo->NextLevel(); i++)
			data[i] = theInfo->InterpolatedValue(theLatlon, theTime); // varmuuden vuoksi kaikki interpoloinnit p��lle, se funktio tarkistaa tarvitseeko sit� tehd�
		status = true;
	}
	else if(theId == kFmiDewPoint && theInfo->Param(kFmiHumidity))
	{
		unsigned int RHindex = theInfo->ParamIndex();
		if(!theInfo->Param(kFmiTemperature))
			return false;
		unsigned int Tindex = theInfo->ParamIndex();
		float T = 0;
		float RH = 0;
		int i = 0;
		for(theInfo->ResetLevel(); theInfo->NextLevel(); i++)
		{
			theInfo->ParamIndex(Tindex);
			T = theInfo->InterpolatedValue(theLatlon, theTime); // varmuuden vuoksi kaikki interpoloinnit p��lle, se funktio tarkistaa tarvitseeko sit� tehd�
			theInfo->ParamIndex(RHindex);
			RH = theInfo->InterpolatedValue(theLatlon, theTime); // varmuuden vuoksi kaikki interpoloinnit p��lle, se funktio tarkistaa tarvitseeko sit� tehd�
			data[i] = static_cast<float>(CalcDP(T, RH));
		}
		status = true;
	}
	else if(theId == kFmiPressure)
	{ // jos halutaan paine dataa ja parametria ei ollut datassa, oliko kyseessa painepinta data, jolloin paine pit�� irroittaa level-tiedosta
		if(theInfo->FirstLevel())
		{
			if(theInfo->Level()->LevelType() == kFmiPressureLevel)
			{
				int i = 0;
				for(theInfo->ResetLevel(); theInfo->NextLevel(); i++)
					data[i] = static_cast<float>(theInfo->Level()->LevelValue());
				status = true;
			}
		}
	}

		// IK�V�� CASTI KOODIA!!!!
	if(static_cast<NFmiFastQueryInfo*>(theInfo)->HeightParamIsRising() == false) // jos ei nousevassa j�rjestyksess�, k��nnet��n vektorissa olevat arvot
		std::reverse(data.begin(), data.end());

	return status;
}

// T�lle anntaan asema dataa ja ei tehd� mink��nlaisia interpolointeja.
bool NFmiSoundingData::FillSoundingData(NFmiQueryInfo* theInfo, const NFmiMetTime& theTime, const NFmiMetTime& theOriginTime, const NFmiLocation& theLocation)
{
	ClearDatas();
	if(theInfo && !theInfo->IsGrid())
	{
		fObservationData = true;
		if(theInfo->Time(theTime))
		{
			if(theInfo->Location(theLocation))
			{
				itsLocation = theLocation;
				itsTime = theTime;
				itsOriginTime = theOriginTime;

				FillParamData(theInfo, kFmiTemperature);
				FillParamData(theInfo, kFmiDewPoint);
				FillParamData(theInfo, kFmiPressure);
				if(!FillParamData(theInfo, kFmiGeopHeight))
					FillParamData(theInfo, kFmiGeomHeight); // eri datoissa on geom ja geop heightia, kokeillaan molempia tarvittaessa
				FillParamData(theInfo, kFmiWindSpeedMS);
				FillParamData(theInfo, kFmiWindDirection);
				FillParamData(theInfo, kFmiWindUMS);
				FillParamData(theInfo, kFmiWindVMS);
				InitZeroHeight();
				return true;
			}
		}
	}
	return false;
}

// T�lle annetaan hiladataa, ja interpolointi tehd��n tarvittaessa ajassa ja paikassa.
bool NFmiSoundingData::FillSoundingData(NFmiQueryInfo* theInfo, const NFmiMetTime& theTime, const NFmiMetTime& theOriginTime, const NFmiPoint& theLatlon, const NFmiString &theName)
{
	ClearDatas();
	if(theInfo && theInfo->IsGrid())
	{
		fObservationData = false;
		itsLocation = NFmiLocation(theLatlon);
		itsLocation.SetName(theName);
		itsTime = theTime;
		itsOriginTime = theOriginTime;

		FillParamData(theInfo, kFmiTemperature, theTime, theLatlon);
		FillParamData(theInfo, kFmiDewPoint, theTime, theLatlon);
		FillParamData(theInfo, kFmiPressure, theTime, theLatlon);
		if(!FillParamData(theInfo, kFmiGeopHeight, theTime, theLatlon))
			FillParamData(theInfo, kFmiGeomHeight, theTime, theLatlon); // eri datoissa on geom ja geop heightia, kokeillaan molempia tarvittaessa
		FillParamData(theInfo, kFmiWindSpeedMS, theTime, theLatlon);
		FillParamData(theInfo, kFmiWindDirection, theTime, theLatlon);
		FillParamData(theInfo, kFmiWindUMS, theTime, theLatlon);
		FillParamData(theInfo, kFmiWindVMS, theTime, theLatlon);
		InitZeroHeight();
		return true;
	}
	return false;
}

// t�t� kutsutaan FillParamData-metodeista
// sill� katsotaan mist� korkeudesta luotaus oikeasti alkaa
// Tehd��n aloitus korkeus seuraavasti:
// Se pinta, milt� kaikki arvot l�ytyv�t 1. kertaa ei puuttuvana
// tai jos sellaista ei l�ydy asetetaan arvoksi 0.
void NFmiSoundingData::InitZeroHeight(void)
{
	itsZeroHeight = 0;
	itsZeroHeightIndex = -1;
	// oletus, kaikki vektorit on alustettu saman kokoisiksi kuin paine vektori
	if(itsPressureData.size() > 0)
	{
		for(int i=0; i<static_cast<int>(itsPressureData.size()); i++)
		{
			if(itsPressureData[i] != kFloatMissing &&
//				itsTemperatureData[i] != kFloatMissing && // l�mp�tilaa ei ehk� tarvitse olla etsitt�ess� ensimm�ist� validia kerrosta
//				itsDewPointData[i] != kFloatMissing && // kastepistett� ei ehk� tarvitse olla etsitt�ess� ensimm�ist� validia kerrosta
				itsGeomHeightData[i] != kFloatMissing
//				&& itsWindSpeedData[i] != kFloatMissing // tuulta ei tarvitse olla etsitt�ess� ensimm�ist� validia kerrosta
				)
			{
				itsZeroHeight = itsGeomHeightData[i];
				itsZeroHeightIndex = i;
				break; // lopetetaan kun 1. l�ytyi
			}
		}
	}
}

checkedVector<float>& NFmiSoundingData::GetParamData(FmiParameterName theId)
{
	static checkedVector<float> dummy;
	switch(theId)
	{
	case kFmiTemperature:
		return itsTemperatureData;
	case kFmiDewPoint:
		return itsDewPointData;
	case kFmiPressure:
		return itsPressureData;
	case kFmiGeopHeight:
	case kFmiGeomHeight:
		return itsGeomHeightData;
	case kFmiWindSpeedMS:
		return itsWindSpeedData;
	case kFmiWindDirection:
		return itsWindDirectionData;
	case kFmiWindUMS:
		return itsWindComponentUData;
	case kFmiWindVMS:
		return itsWindComponentVData;
	default:
	  throw std::runtime_error(std::string("NFmiSoundingData::GetParamData - wrong paramId given (Error in Program?): ") + NFmiStringTools::Convert<int>(theId));
	}
}

void NFmiSoundingData::ClearDatas(void)
{
	checkedVector<float>().swap(itsTemperatureData);
	checkedVector<float>().swap(itsDewPointData);
	checkedVector<float>().swap(itsPressureData);
	checkedVector<float>().swap(itsGeomHeightData);
	checkedVector<float>().swap(itsWindSpeedData);
	checkedVector<float>().swap(itsWindDirectionData);
	checkedVector<float>().swap(itsWindComponentUData);
	checkedVector<float>().swap(itsWindComponentVData);
}

bool NFmiSoundingData::ModifyT2DryAdiapaticBelowGivenP(double P, double T)
{
	if(P == kFloatMissing || T == kFloatMissing)
		return false;

	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&tV = GetParamData(kFmiTemperature);
	if(pV.size() > 0 && pV.size() == tV.size())
	{
		unsigned int ssize = pV.size();
		float wantedTPot = static_cast<float>(::T2tpot(T, P));
		float currentP = 1000;
		for(unsigned int i=0; i<ssize; i++)
		{
			currentP = pV[i];
			if(currentP >= P)
			{ // muutoksia siis tehtiin niin kauan kuin oltiin alle annetun paineen
				float wantedT = static_cast<float>(::Tpot2t(wantedTPot, currentP));
				tV[i] = wantedT;
			}
			else
				break;
		}
		return true;
	}
	return false;
}

bool NFmiSoundingData::ModifyTd2MoistAdiapaticBelowGivenP(double P, double Td)
{
	if(P == kFloatMissing || Td == kFloatMissing)
		return false;

	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&tdV = GetParamData(kFmiDewPoint);
	if(pV.size() > 0 && pV.size() == tdV.size())
	{
		unsigned int ssize = pV.size();

//		float wantedTPot = static_cast<float>(::T2tpot(Td, P));
		float AOS = static_cast<float>(NFmiSoundingFunctions::OS(Td, P));
		float currentP = 1000;
		for(unsigned int i=0; i<ssize; i++)
		{
			currentP = pV[i];
			if(currentP >= P)
			{ // muutoksia siis tehtiin niin kauan kuin oltiin alle annetun paineen
				float ATSA  = static_cast<float>(NFmiSoundingFunctions::TSA(AOS, currentP));
//				float wantedT = static_cast<float>(::Tpot2t(wantedTPot, currentP));
				tdV[i] = ATSA;
			}
			else
				break;
		}
		return true;
	}
	return false;
}

static float FixCircularValues(float theValue, float modulorValue)
{
	if(theValue != kFloatMissing)
	{
		if(theValue < 0)
			return modulorValue - ::fmod(-theValue, modulorValue);
		else
			return ::fmod(theValue, modulorValue);
	}
	return theValue;
}

static float FixValuesWithLimits(float theValue, float minValue, float maxValue)
{
	if(theValue != kFloatMissing)
	{
		if(minValue != kFloatMissing)
			theValue = FmiMax(theValue, minValue);
		if(maxValue != kFloatMissing)
			theValue = FmiMin(theValue, maxValue);
	}
	return theValue;
}

bool NFmiSoundingData::Add2ParamAtNearestP(float P, FmiParameterName parId, float addValue, float minValue, float maxValue, bool fCircularValue)
{
	if(P == kFloatMissing)
		return false;

	checkedVector<float>&pV = GetParamData(kFmiPressure);
	checkedVector<float>&paramV = GetParamData(parId);
	if(pV.size() > 0 && pV.size() == paramV.size())
	{
		float currentP = kFloatMissing;
		float currentParam = kFloatMissing;
		float lastP = kFloatMissing;
		float lastParam = kFloatMissing;
		float closestPdiff = kFloatMissing;
		unsigned int closestIndex = 0;
		unsigned int ssize = pV.size();
		for(unsigned int i=0; i<ssize; i++)
		{
			currentP = pV[i];
			currentParam = paramV[i];
			if(currentP != kFloatMissing && currentParam != kFloatMissing)
			{
				if(closestPdiff > ::fabs(P-currentP))
				{
					closestPdiff = ::fabs(P-currentP);
					closestIndex = i;
				}
			}
			if(currentP < P && closestPdiff != kFloatMissing)
				break;
		}

		if(closestPdiff != kFloatMissing)
		{
			float closestParamValue = paramV[closestIndex];
			closestParamValue += addValue;
			if(fCircularValue)
				closestParamValue = ::FixCircularValues(closestParamValue, maxValue);
			else
				closestParamValue = ::FixValuesWithLimits(closestParamValue, minValue, maxValue);

			paramV[closestIndex] = closestParamValue;
			return true;
		}
	}
	return false;
}

static float CalcU(float WS, float WD)
{
	if(WD == 999) // jos tuulensuunta on vaihtelevaa (999), palautetaan 0 arvo (voisi olla my�s puuttuvaa)
		return 0.f;
	float value = kFloatMissing;
	if(WS != kFloatMissing && WD != kFloatMissing)
	{
		value = WS * sin(((static_cast<int>(180 + WD) % 360)/360.f)*(2.f*static_cast<float>(kPii))); // huom! tuulen suunta pit�� ensin k��nt�� 180 astetta ja sitten muuttaa radiaaneiksi kulma/360 * 2*pii
	}
	return value;
}

static float CalcV(float WS, float WD)
{
	if(WD == 999) // jos tuulensuunta on vaihtelevaa (999), palautetaan 0 arvo (voisi olla my�s puuttuvaa)
		return 0;
	float value = kFloatMissing;
	if(WS != kFloatMissing && WD != kFloatMissing)
	{
		value = WS * cos(((static_cast<int>(180 + WD) % 360)/360.f)*(2.f*static_cast<float>(kPii))); // huom! tuulen suunta pit�� ensin k��nt�� 180 astetta ja sitten muuttaa radiaaneiksi kulma/360 * 2*pii
	}
	return value;
}

void NFmiSoundingData::UpdateUandVParams(void)
{
	checkedVector<float>&wsV = GetParamData(kFmiWindSpeedMS);
	checkedVector<float>&wdV = GetParamData(kFmiWindDirection);
	checkedVector<float>&uV = GetParamData(kFmiWindUMS);
	checkedVector<float>&vV = GetParamData(kFmiWindVMS);
	if(wsV.size() > 0 && wsV.size() == wdV.size() && wsV.size() == uV.size() && wsV.size() == vV.size())
	{
		unsigned int ssize = wsV.size();
		for(unsigned int i=0; i<ssize; i++)
		{
			uV[i] = ::CalcU(wsV[i], wdV[i]);
			vV[i] = ::CalcV(wsV[i], wdV[i]);
		}
	}
}
