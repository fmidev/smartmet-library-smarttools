//**********************************************************
// C++ Class Name : NFmiInfoOrganizer 
// ---------------------------------------------------------
// Filetype: (SOURCE)
// Filepath: D:/projekti/GDPro/GDTemp/NFmiInfoOrganizer.cpp 
// 
// 
// GDPro Properties 
// ---------------------------------------------------
//  - GD Symbol Type    : CLD_Class 
//  - GD Method         : UML ( 2.1.4 ) 
//  - GD System Name    : Met-editor Plan 2 
//  - GD View Type      : Class Diagram 
//  - GD View Name      : Markon ehdotus 
// ---------------------------------------------------  
//  Author         : pietarin 
//  Creation Date  : Tues - Feb 9, 1999 
// 
// 
//  Description: 
//   Sis�lt�� mahdollisesti listan infoja, joista 
//   luokka osaa antaa tarvittavan
//   infon pointterin parametrin nimell�. Listalla 
//   ei ole j�rjestyst�.
// 
//  Change Log: 
// 1999.08.26/Marko	Laitoin organizeriin eri tyyppisten datojen talletuksen
//					mahdollisuuden. Editoitavaa dataa voi olla vain yksi, mutta 
//					katsottavaa/maskattavaa dataa voi olla useita erilaisia ja statio-
//					n��risi� (esim. topografia) datoja voi olla my�s useita erilaisia. 
// 1999.08.26/Marko	Laitoin level tiedon DrawParam:in luonti-kutsuun varten.
// 1999.09.22/Marko	DrawParamin luonnissa otetaan huomioon datan tyyppi t�st�l�htien.
// 1999.09.22/Marko	Lis�sin EditedDatan kopion, jota k�ytet��n visualisoimaan dataan tehtyj� muutoksia.
// 
//**********************************************************
#include "NFmiInfoOrganizer.h"
#include "NFmiSmartInfo.h"
#include "NFmiDrawParamFactory.h"
#include "NFmiQueryData.h"
#include "NFmiProducerList.h"
#include "NFmiGrid.h"

#ifndef UNIX
  #include "stdafx.h" // TRACE-kutsu
#endif

//--------------------------------------------------------
// Constructor/Destructor 
//--------------------------------------------------------
// luodaan tyhj� lista
NFmiInfoOrganizer::NFmiInfoOrganizer (bool theIsToolMasterAvailable)
:itsList()
,itsIter(itsList.Start())
,itsDrawParamFactory(0)
,itsWorkingDirectory("")
,itsEditedData(0)
,itsEditedDataCopy(0)
,fToolMasterAvailable(theIsToolMasterAvailable)
{
}

// destruktori tuhoaa infot
NFmiInfoOrganizer::~NFmiInfoOrganizer (void)
{
	Clear();
	delete itsDrawParamFactory;
	if(itsEditedData)
		itsEditedData->DestroySharedData();
	delete itsEditedData;

	if(itsEditedDataCopy)
		itsEditedDataCopy->DestroySharedData();
	delete itsEditedDataCopy;
}


//--------------------------------------------------------
// Init 
//--------------------------------------------------------
bool NFmiInfoOrganizer::Init (void)
{
 	itsDrawParamFactory =new NFmiDrawParamFactory;
	itsDrawParamFactory->WorkingDirectory(WorkingDirectory());
 	return itsDrawParamFactory->Init(); 
}

//--------------------------------------------------------
// Info 
//--------------------------------------------------------

//  Palauttaa smartinfon pointterin, jossa on annettu parametri 
//  joko kokonaisena tai jonkin parametrin osaparametrina.
//	Kysyt��n ensimm�isen� listassa olevalta infolta, onko infolla theParam
//	suoraan. Jos milt��n infolta ei suoraan saada theParam:a, tutkitaan
//	l�ytyyk� theParam aliparametrina (esim. pilvisyys voi l�yty� parametrin
//	kWeatherAndCloudiness aliparametrina). Periaatteessa theParam voisi l�yty�
//	my�s aliparametrin aliparametrilta jne - tutkittavia aliparametri-tasoja
//	ei ole rajoitettu. T�ss� metodissa tutkitaan vain parameri ja tarvittaessa
//  parametrin aliparametri. 

//  22.02.1999 lis�ttiin  tunnistin bool fSubParameter, josta saadaan tieto
//  siit�, oliko l�ydetty parametri itsen�inen vaiko jonkin parametrin aliparametri.
NFmiSmartInfo* NFmiInfoOrganizer::Info ( const FmiParameterName& theParam
									   , bool& fSubParameter 
									   , const NFmiLevel* theLevel
									   , FmiQueryInfoDataType theType)
{
	NFmiSmartInfo* aInfo = 0;
	if(itsEditedData && itsEditedData->DataType() == theType && itsEditedData->Param(theParam) && (!theLevel || (theLevel && itsEditedData->Level(*theLevel))))
	{
		fSubParameter = itsEditedData->UseSubParam();
		aInfo = itsEditedData;
	}
	else if(itsEditedDataCopy && itsEditedDataCopy->DataType() == theType && itsEditedDataCopy->Param(theParam) && (!theLevel || (theLevel && itsEditedDataCopy->Level(*theLevel))))
	{
		fSubParameter = itsEditedDataCopy->UseSubParam();
		aInfo = itsEditedDataCopy;
	}
	else
	{
		NFmiPtrList<NFmiSmartInfo>::Iterator aIter = itsList.Start();
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		while(aIter.Next())
		{
			aInfo = aIter.CurrentPtr();
			if(aInfo->DataType() == theType && aInfo->Param(theParam) && (!theLevel || (theLevel && aInfo->Level(*theLevel))))
			{
				fSubParameter = aInfo->UseSubParam();
				break;
			}
			aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
		}
	}
	if(aInfo && aInfo->SizeLevels() == 1)
		aInfo->FirstLevel();
	return aInfo; // theParam ei l�ytynyt edes aliparametrina milt��n listassa olevalta aInfo-pointterilta
}
//--------------------------------------------------------
// Info 
//--------------------------------------------------------

NFmiSmartInfo* NFmiInfoOrganizer::Info ( const NFmiDataIdent& theDataIdent
									   , bool& fSubParameter 
									   , const NFmiLevel* theLevel
									   , FmiQueryInfoDataType theType)
{
	NFmiSmartInfo* aInfo = 0;
	if(itsEditedData && itsEditedData->DataType() == theType && itsEditedData->Param(theDataIdent) && (!theLevel || (theLevel && itsEditedData->Level(*theLevel))))
	{
		fSubParameter = itsEditedData->UseSubParam();
		aInfo = itsEditedData;
	}
	else if(itsEditedDataCopy && itsEditedDataCopy->DataType() == theType && itsEditedDataCopy->Param(theDataIdent) && (!theLevel || (theLevel && itsEditedDataCopy->Level(*theLevel))))
	{
		fSubParameter = itsEditedDataCopy->UseSubParam();
		aInfo = itsEditedDataCopy;
	}
	else
	{
		// tutkitaan ensin l�ytyyk� theParam suoraan joltain listassa olevalta NFmiSmartInfo-pointterilta
		for(Reset(); Next(); )
		{
			aInfo = Current();
			if(aInfo->DataType() == theType && aInfo->Param(theDataIdent) && (!theLevel || (theLevel && aInfo->Level(*theLevel))))
			{
				fSubParameter = aInfo->UseSubParam();
				break;
			}
			aInfo = 0; // pit�� aina t�ss� nollata, muuten viimeisen j�lkeen j�� voimaan
		}
	}
	if(aInfo && aInfo->SizeLevels() == 1)
		aInfo->FirstLevel();
	return aInfo; // theParam ei l�ytynyt edes aliparametrina milt��n listassa olevalta aInfo-pointterilta
}

// itsEditedData infon parambagi
NFmiParamBag NFmiInfoOrganizer::EditedParams(void)
{
	if(itsEditedData)
		return *(itsEditedData->ParamDescriptor().ParamBag());
	return NFmiParamBag();
}

// kaikkien apudatojen parametrit yhdess� bagissa (joita voidaan katsoa/maskata)
NFmiParamBag NFmiInfoOrganizer::ViewableParams(void)
{
	return GetParams(kFmiDataTypeViewable);
}

// vain halutun indeksin parametrit (HUONO VIRITYS KORJAA!!!!)
NFmiParamBag NFmiInfoOrganizer::ViewableParams(int theIndex)
{
	NFmiSmartInfo* info = ViewableInfo(theIndex);
	if(info)
		return info->ParamBag();
	return NFmiParamBag();
}

// vain halutun indeksin parametrit (HUONO VIRITYS KORJAA!!!!)
NFmiParamBag NFmiInfoOrganizer::ObservationParams(int theIndex)
{
	NFmiSmartInfo* info = ObservationInfo(theIndex);
	if(info)
		return info->ParamBag();
	return NFmiParamBag();
}

NFmiParamBag NFmiInfoOrganizer::GetParams(FmiQueryInfoDataType theDataType)
{
	NFmiParamBag paramBag;
	for(Reset();Next();)
	{
		if(Current()->DataType() == theDataType)
			paramBag = paramBag.Combine(Current()->ParamBag());
	}

    return paramBag;
}
/*
NFmiParamBag NFmiInfoOrganizer::GetParams(FmiQueryInfoDataType theDataType)
{
// Luodaan vektori NFmiDataIdent-pointtereille . Vektorin pituus
// saadaan laskemalla kaikkien tietyn tyyppisten infojen parametrien lukum�r�t yhteen.	
	if(theDataType == kFmiDataTypeEditable)
		return EditedParams();

	long count = 0;
	for(Reset();Next();)
		if(Current()->DataType() == theDataType)
			count += Current()->SizeParams();
	if(count == 0)
		return NFmiParamBag();
	NFmiDataIdent* dataIdents = new NFmiDataIdent[count];

// K�yd��n l�pi kaikki listan itsList SmartInfot
	int ind=0;
	for (Reset();Next();)
	{
//  Kunkin SmartInfon sis�ll� (attribute itsParamDescriptor) olevat 
//  dataIdentit list�t�� vektoriin
		if(Current()->DataType() == theDataType)
			for(Current()->ResetParam();Current()->NextParam();ind++)
				dataIdents[ind] = Current()->Param();
	}
// Luodaan palautettava paramBag 
    NFmiParamBag paramBag(dataIdents, count);
    delete[] dataIdents;
    return paramBag;
}
*/
// kaikkien staattisten (ei muutu ajan mukana) datojen parambag (esim. topografia)
NFmiParamBag NFmiInfoOrganizer::StaticParams(void)
{
	return GetParams(kFmiDataTypeStationary);
}

// SmartToolModifier tarvitsee ohuen kopion (eli NFmiQueryData ei kopioidu)
NFmiSmartInfo* NFmiInfoOrganizer::CreateShallowCopyInfo(FmiParameterName theParamName, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theParamName, aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theParamName))
		{
			NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*info);
			return copyOfInfo;
		}
	}
	return 0;
}

// SmartToolModifier tarvitsee ohuen kopion (eli NFmiQueryData ei kopioidu)
NFmiSmartInfo* NFmiInfoOrganizer::CreateShallowCopyInfo(const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theDataIdent, aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theDataIdent))
		{
			NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*info);
			return copyOfInfo;
		}
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::CreateInfo(FmiParameterName theParamName, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theParamName, aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theParamName))
			return info->Clone();
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::CreateInfo(const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theDataIdent, aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theDataIdent))
			return info->Clone();
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::CreateInfo(NFmiSmartInfo* theUsedInfo, const NFmiDataIdent& theDataIdent, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
	bool aSubParam = false;	
	if(theUsedInfo && theUsedInfo->DataType() == theType && theUsedInfo->Param(theDataIdent) && (!theLevel || (theLevel && theUsedInfo->Level(*theLevel))))
	{
		aSubParam = theUsedInfo->UseSubParam();
		NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*theUsedInfo);
		return copyOfInfo;
	}
	return 0;
}

//--------------------------------------------------------
// CreateDrawParam(FmiParameterName theParamName)
//--------------------------------------------------------
// Tutkii l�ytyyk� listasta itsList infoa, jossa on theParam.
// Jos t�ll�inen info l�ytyy, pyydet��n itsDrawParamFactory luomaan
// drawParam kyseiselle parametrille l�ydetyn infon avulla.
NFmiDrawParam* NFmiInfoOrganizer::CreateDrawParam(FmiParameterName theParamName, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
// Huomaa, ett� palautettava pointteri drawParam luodaan attribuutin 
// itsDrawParamFactory sis�ll� new:ll�, joten drawParam  
// pit�� muistaa tuhota  NFmiInfoOrganizer:n ulkopuolella

	NFmiDrawParam* drawParam = 0;
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theParamName, aSubParam, theLevel, theType);
	if(info)
	{
		if(info->Param(theParamName))
		{
			NFmiDataIdent dataIdent(info->Param());
			return CreateDrawParam(dataIdent, theLevel, theType);
		}
		if(aSubParam)
		{
			info->FirstParam();
			NFmiDataIdent dataIdent(info->Param());
			dataIdent.GetParam()->SetIdent(theParamName);
			return CreateDrawParam(dataIdent, theLevel, theType);
		}
	}
	return drawParam;
}

//--------------------------------------------------------
// CreateDrawParam(NFmiDataIdent& theDataIdent)
//--------------------------------------------------------
// Tutkii l�ytyyk� listasta itsList infoa, jossa on theDataIdent - siis
// etsit��n info, jonka tuottaja ja parametri saadaan theDataIdent:st�.
// Jos t�ll�inen info l�ytyy, pyydet��n itsDrawParamFactory luomaan
// drawParam kyseiselle parametrille l�ydetyn infon avulla.
NFmiDrawParam* NFmiInfoOrganizer::CreateDrawParam(const NFmiDataIdent& theIdent, const NFmiLevel* theLevel, FmiQueryInfoDataType theType)
{
// Huomaa, ett� itsDrawParamFactory luo pointterin drawParam new:ll�, joten   
// drawParam pit�� muistaa tuhota  NFmiInfoOrganizer:n ulkopuolella
	// int thePriority = 1;// toistaiseksi HARDCODE, thePriority m��ritys tehd��n my�hemmin
	NFmiDrawParam* drawParam = 0;
	bool aSubParam;	
	NFmiSmartInfo* info = Info(theIdent, aSubParam, theLevel, theType);
	if(info)
	{
		NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*info);
		drawParam = itsDrawParamFactory->CreateDrawParam(copyOfInfo, theIdent, aSubParam, theLevel, fToolMasterAvailable);	
	}
	return drawParam;
}

// Luo halutun drawparam:in, mutta k�ytt�en annettua smartinfoa. 
// K�ytet��n kun ratkaistaan parametrin tasaus + maski ongelmaa (maski muuttuu kun 
// tasausta suoritetaan, joten pit�� k�ytt�� kopiota)
NFmiDrawParam* NFmiInfoOrganizer::CreateDrawParam(NFmiSmartInfo* theUsedInfo
												 ,const NFmiDataIdent& theDataIdent
												 ,const NFmiLevel* theLevel
//												 ,FmiSmartInfoDataType theType)
												 ,FmiQueryInfoDataType theType)
{
	NFmiDrawParam* drawParam = 0;
	bool aSubParam = false;	
	if(theUsedInfo && theUsedInfo->DataType() == theType && theUsedInfo->Param(theDataIdent) && (!theLevel || (theLevel && theUsedInfo->Level(*theLevel))))
	{
		aSubParam = theUsedInfo->UseSubParam();
		NFmiSmartInfo* copyOfInfo = new NFmiSmartInfo(*theUsedInfo);
		drawParam = itsDrawParamFactory->CreateDrawParam(copyOfInfo, theDataIdent, aSubParam, theLevel, fToolMasterAvailable);	
	}
	return drawParam;
}

NFmiDrawParam* NFmiInfoOrganizer::CreateEmptyInfoDrawParam(FmiParameterName theParamName)
{
	NFmiParam param(theParamName);
	NFmiDataIdent dataIdent(param);
	NFmiDrawParam *drawParam = itsDrawParamFactory->CreateEmptyInfoDrawParam(dataIdent, fToolMasterAvailable);
	return drawParam;
}

//--------------------------------------------------------
// AddData 
//--------------------------------------------------------
// HUOM!!!! T�nne ei ole sitten tarkoitus antaa kFmiDataTypeCopyOfEdited-tyyppist�
// dataa, koska se luodaan kun t�nne annetaan editoitavaa dataa.
bool NFmiInfoOrganizer::AddData(NFmiQueryData* theData
									 ,const NFmiString& theDataFileName
//									 ,FmiSmartInfoDataType theDataType
									 ,FmiQueryInfoDataType theDataType
									 ,int theUndoLevel)
{
	bool status = false;
#ifndef UNIX
	TRACE("Dataa NFmiInfoOrganizerissa ennen AddData:a %d kpl.\n", CountData());
#endif
	if(theData)
	{
		NFmiQueryInfo aQueryInfo(theData);
		NFmiSmartInfo* aSmartInfo = new NFmiSmartInfo(aQueryInfo, theData, theDataFileName, theDataType);// ...otetaan k�ytt��n my�hemmin
		if(aSmartInfo)
			aSmartInfo->First();

		if(theDataType == kFmiDataTypeEditable)
		{
			if(theUndoLevel)
				aSmartInfo->UndoLevel(theUndoLevel);
			if(itsEditedData)
			{
				itsEditedData->DestroySharedData();
				delete itsEditedData;
			}
			itsEditedData = aSmartInfo;
			UpdateEditedDataCopy();
			status = true;
		}
		else
			status = Add(aSmartInfo); // muun tyyppiset datat kuin editoitavat menev�t listaan
	}
#ifndef UNIX
	TRACE("Dataa NFmiInfoOrganizerissa j�lkeen AddData:a %d kpl.\n", CountData());
#endif
	return status;
}

void NFmiInfoOrganizer::ClearData(FmiQueryInfoDataType theDataType)
{
	if(theDataType == kFmiDataTypeEditable)
	{
		if(itsEditedData)
		{
			itsEditedData->DestroySharedData();
			delete itsEditedData;
			itsEditedData = 0;
		}
	}
	else
	{ // k�yd��n lista l�pi ja tuhotaan halutun tyyppiset datat
		for(Reset(); Next(); )
		{
			if(Current()->DataType() == theDataType)
			{
				Current()->DestroySharedData();
				Remove();
			}
		}
	}
}

void NFmiInfoOrganizer::ClearThisKindOfData(NFmiQueryInfo* theInfo)
{
	if(theInfo)
	{
		if(itsEditedData)
		{
			if(IsInfosTwoOfTheKind(theInfo, itsEditedData))
			{
				itsEditedData->DestroySharedData();
				delete itsEditedData;
				itsEditedData = 0;
				return;
			}
		}

		for(Reset(); Next(); )
		{
			if(IsInfosTwoOfTheKind(theInfo, Current()))
			{
				Current()->DestroySharedData();
				Remove();
				break; // tuhotaan vain yksi!
			}
		}
	}
}

int NFmiInfoOrganizer::CountData(void)
{
	int count = 0;
	if(itsEditedData)
		count++;

	count += itsList.NumberOfItems();
	return count;
}

// this kind of m��ritell��n t�ll� hetkell�:
// parametrien, leveleiden ja mahdollisen gridin avulla (ei location bagin avulla)
bool NFmiInfoOrganizer::IsInfosTwoOfTheKind(NFmiQueryInfo* theInfo1, NFmiQueryInfo* theInfo2)
{
	// parametrit ja tuottajat samoja
	if(theInfo1->ParamBag() == theInfo2->ParamBag()) 
	{
		// level jutut samoja
		if(theInfo1->VPlaceDescriptor() == theInfo2->VPlaceDescriptor())
		{
			// mahdollinen gridi samoja
			bool status3 = true;
			if(theInfo1->Grid() && theInfo2->Grid())
			{
				status3 = (theInfo1->Grid()->AreGridsIdentical(*theInfo2->Grid())) == true;
			}
			if(status3)
			{
				theInfo1->FirstParam(); // varmistaa, ett� producer l�ytyy
				theInfo2->FirstParam();
				if(*theInfo1->Producer() == *theInfo2->Producer())
					return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------
// ProducerList 
//--------------------------------------------------------
// EI TOIMI TEHTYJEN MUUTOSTEN J�LKEEN (1999.08.26/Marko), KORJAA!!!
NFmiProducerList* NFmiInfoOrganizer::ProducerList(void)
{
// duplikaattien poisto teht�v� joskus...
	NFmiProducerList* prodList = new NFmiProducerList;
	for(Reset();Next();)
	{
		NFmiProducer* produ = new NFmiProducer(*(Current()->Param().GetProducer()));
		prodList->Add(produ);
	}
	return prodList;
}

//--------------------------------------------------------
// Add 
//--------------------------------------------------------
//   Laittaa alkuper�isen pointterin listaan - uutta pointteria
//   ei luoda vaan NFmiInfoOrganizer::itsList ottaa pointterin 
//   'omistukseen'. T�m�n luokan destruktori tuhoaa infot.
bool NFmiInfoOrganizer::Add (NFmiSmartInfo* theInfo)
{
	return itsList.InsertionSort(theInfo);
}
//--------------------------------------------------------
// Clear 
//--------------------------------------------------------
// tuhoaa aina datan
bool NFmiInfoOrganizer::Clear (void)
{ 
	for( Reset(); Next(); )
	{
		Current()->DestroySharedData();
		delete Current();
	}
	itsList.Clear(false);
	return true; // jotain pit�si varmaan tsekatakin? 
}
 
//--------------------------------------------------------
// Reset 
//--------------------------------------------------------
bool NFmiInfoOrganizer::Reset (void)
{
   itsIter = itsList.Start();
   return true;
}
//--------------------------------------------------------
// Next 
//--------------------------------------------------------
bool NFmiInfoOrganizer::Next (void)
{
   return itsIter.Next();
}
//--------------------------------------------------------
// Current 
//--------------------------------------------------------
NFmiSmartInfo* NFmiInfoOrganizer::Current (void)
{
   return itsIter.CurrentPtr();
}

//--------------------------------------------------------
// Remove 
//--------------------------------------------------------
bool NFmiInfoOrganizer::Remove(void)
{
	return itsIter.Remove(true);
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ViewableInfo(void)
{
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeViewable)
			return Current();
	}
	return 0;
}

// T�m� on pikaviritys fuktio (kuten nimest�kin voi p��tell�)
// Tarvitaan viel� kun pelataan parametrin valinnassa popupien kanssa (ja niiden kanssa on vaikea pelata).
NFmiLevelBag* NFmiInfoOrganizer::GetAndCreateViewableInfoWithManyLevelsOrZeroPointer(void)
{
	NFmiLevelBag* levels = 0;
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeViewable)
			if(Current()->SizeLevels() > 1)
			{
				levels = new NFmiLevelBag(*Current()->VPlaceDescriptor().Levels());
				break;
			}
	}
	return levels;
}

// palauttaa halutun indeksin infon (huono viritys, KORJAA!!!!)
NFmiSmartInfo* NFmiInfoOrganizer::ViewableInfo(int theIndex)
{
	int ind = 0;
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeViewable)
		{
			if(ind == theIndex)
				return Current();
			ind++;
		}
	}
	return 0;
}

// palauttaa halutun indeksin infon (huono viritys, KORJAA!!!!)
NFmiSmartInfo* NFmiInfoOrganizer::ObservationInfo(int theIndex)
{
	int ind = 0;
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeObservations)
		{
			if(ind == theIndex)
				return Current();
			ind++;
		}
	}
	return 0;
}

// 28.09.1999/Marko Tekee uuden kopion editoitavasta datasta
void NFmiInfoOrganizer::UpdateEditedDataCopy(void)
{
	if(itsEditedData)
	{
		if(itsEditedDataCopy)
			itsEditedDataCopy->DestroySharedData();
		delete itsEditedDataCopy;
		itsEditedDataCopy = itsEditedData->Clone();
		itsEditedDataCopy->DataType(kFmiDataTypeCopyOfEdited);
	}
}

// kaikkien staattisten (ei muutu ajan mukana) datojen parambag (esim. topografia)
NFmiParamBag NFmiInfoOrganizer::ObservationParams(void)
{
	return GetParams(kFmiDataTypeObservations);
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ObservationInfo(void)
{
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeObservations)
			return Current();
	}
	return 0;
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::KepaDataInfo(void)
{
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeKepaData)
			return Current();
	}
	return 0;
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::ClimatologyInfo(void)
{
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeClimatologyData)
			return Current();
	}
	return 0;
}

NFmiSmartInfo* NFmiInfoOrganizer::AnalyzeDataInfo(void) // t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
{
	return FindInfo(kFmiDataTypeAnalyzeData);
}

NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(FmiQueryInfoDataType theDataType) // Hakee 1. tietyn datatyypin infon
{
	return FindInfo(theDataType, 0); // indeksi alkaa 0:sta!!!
}

NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(FmiQueryInfoDataType theDataType, int theIndex) // Hakee indeksin mukaisen tietyn datatyypin infon
{
	int ind = 0;
	for(Reset(); Next();)
	{
		if(Current()->DataType() == theDataType)
		{
			if(ind == theIndex)
				return Current();
			ind++;
		}
	}
	return 0;
}

// Palauttaa vectorin viewable infoja, vectori ei omista pointtereita, 
// joten infoja ei saa tuhota.
std::vector<NFmiSmartInfo*> NFmiInfoOrganizer::GetInfos(FmiQueryInfoDataType theDataType)
{
	std::vector<NFmiSmartInfo*> infoVector;
	for(Reset(); Next();)
	{
		if(Current()->DataType() == theDataType)
			infoVector.push_back(Current());
	}
	return infoVector;
}

// Haetaan halutun datatyypin, tuottajan joko pinta tai level dataa (mahd indeksi kertoo sitten konfliktin
// yhteydess�, monesko otetaan)
NFmiSmartInfo* NFmiInfoOrganizer::FindInfo(FmiQueryInfoDataType theDataType, const NFmiProducer &theProducer, bool fGroundData, int theIndex)
{
	if(theDataType == kFmiDataTypeEditable)
		return EditedInfo();
	else if(theDataType == kFmiDataTypeCopyOfEdited)
		return EditedInfoCopy();
	else
	{
		int ind = 0;
		for(Reset(); Next();)
		{
			if(Current()->DataType() == theDataType)
			{
				Current()->FirstParam(); // pit�� varmistaa, ett� producer l�ytyy
				if(*Current()->Producer() == theProducer)
				{
					int levSize = Current()->SizeLevels();
					if((levSize == 1 && fGroundData) || (levSize > 1 && (!fGroundData)))
					{
						if(ind == theIndex)
							return Current();
						ind++;
					}
				}
			}
		}
	}
	return 0;
}

// t�m� toimii vajavaisesti, koska se palauttaa aina 1. kyseisen tyyppisen infon
NFmiSmartInfo* NFmiInfoOrganizer::CompareModelsInfo(void)
{
	for(Reset(); Next();)
	{
		if(Current()->DataType() == kFmiDataTypeCompareModels)
			return Current();
	}
	return 0;
}

// palauttaa vain 1. kyseisen datan parametrit!
NFmiParamBag NFmiInfoOrganizer::CompareModelsParams(void)
{
//	return GetParams(kFmiDataTypeCompareModels);
	NFmiSmartInfo* info = CompareModelsInfo();
	if(info)
		return *(info->ParamDescriptor().ParamBag());
	return NFmiParamBag();
}

NFmiSmartInfo* NFmiInfoOrganizer::CompareModelsInfo(const NFmiProducer& theProducer)
{
	for(Reset(); Next();)
	{
		NFmiSmartInfo* info = Current();
		if(!info->IsParamUsable())
			info->FirstParam();
		NFmiProducer* producer = info->Producer();
		if(info->DataType() == kFmiDataTypeCompareModels && (*producer == theProducer))
			return Current();
	}
	return 0;
}
