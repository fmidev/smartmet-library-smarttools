#pragma once
//**********************************************************
// C++ Class Name : NFmiAreaMaskInfo 
// ---------------------------------------------------------
//  Author         : pietarin 
//  Creation Date  : 8.11. 2010 
// 
//**********************************************************

#include "NFmiAreaMask.h"
#include "NFmiDataIdent.h"
#include "NFmiCalculationCondition.h"
#include "NFmiPoint.h"
#include "NFmiSoundingIndexCalculator.h"

class NFmiLevel;

class NFmiAreaMaskInfo 
{

public:

	NFmiAreaMaskInfo(const std::string &theOrigLineText = "");
	NFmiAreaMaskInfo(const NFmiAreaMaskInfo &theOther);
	~NFmiAreaMaskInfo(void);

	void SetDataIdent(const NFmiDataIdent& value) {itsDataIdent = value;}
	const NFmiDataIdent& GetDataIdent(void) const {return itsDataIdent;}
	void SetUseDefaultProducer(bool value) {fUseDefaultProducer = value;}
	bool GetUseDefaultProducer(void) const {return fUseDefaultProducer;}
	void SetMaskCondition(const NFmiCalculationCondition &value){itsMaskCondition = value;}
	const NFmiCalculationCondition& GetMaskCondition(void) const {return itsMaskCondition;}
	NFmiAreaMask::CalculationOperationType GetOperationType(void) const {return itsOperationType;}
	void SetOperationType(NFmiAreaMask::CalculationOperationType newValue) {itsOperationType = newValue;}
	NFmiAreaMask::CalculationOperator GetCalculationOperator(void) const {return itsCalculationOperator;}
	void SetCalculationOperator(NFmiAreaMask::CalculationOperator newValue) {itsCalculationOperator = newValue;}
	NFmiAreaMask::BinaryOperator GetBinaryOperator(void) const{return itsBinaryOperator;}
	void SetBinaryOperator(NFmiAreaMask::BinaryOperator theValue) {itsBinaryOperator = theValue;}
	NFmiInfoData::Type GetDataType(void) const {return itsDataType;}
	void SetDataType(NFmiInfoData::Type newValue){itsDataType = newValue;}
	NFmiLevel* GetLevel(void) const {return itsLevel;}
	void SetLevel(NFmiLevel *theLevel);
	const std::string& GetMaskText(void) const{return itsMaskText;}
	void SetMaskText(const std::string& theText){itsMaskText = theText;}
	const std::string& GetOrigLineText(void) const{return itsOrigLineText;}
	void SetOrigLineText(const std::string& theText){itsOrigLineText = theText;}
	NFmiAreaMask::FunctionType GetFunctionType(void)const {return itsFunctionType;}
	void SetFunctionType(NFmiAreaMask::FunctionType newType){itsFunctionType = newType;}
	NFmiAreaMask::FunctionType GetSecondaryFunctionType(void)const {return itsSecondaryFunctionType;}
	void SetSecondaryFunctionType(NFmiAreaMask::FunctionType newType){itsSecondaryFunctionType = newType;}
	const NFmiPoint& GetOffsetPoint1(void) const{return itsOffsetPoint1;}
	void SetOffsetPoint1(const NFmiPoint& newValue){itsOffsetPoint1 = newValue;}
	const NFmiPoint& GetOffsetPoint2(void) const{return itsOffsetPoint2;}
	void SetOffsetPoint2(const NFmiPoint& newValue){itsOffsetPoint2 = newValue;}
	NFmiAreaMask::MathFunctionType GetMathFunctionType(void) const {return itsMathFunctionType;};
	void SetMathFunctionType(NFmiAreaMask::MathFunctionType newValue) {itsMathFunctionType = newValue;};
	int IntegrationFunctionType(void) const {return itsIntegrationFunctionType;}
	void IntegrationFunctionType(int newValue) {itsIntegrationFunctionType = newValue;}
	int FunctionArgumentCount(void) const {return itsFunctionArgumentCount;}
	void FunctionArgumentCount(int newValue) {itsFunctionArgumentCount = newValue;}
	FmiSoundingParameters SoundingParameter(void) const {return itsSoundingParameter;}
	void SoundingParameter(FmiSoundingParameters newValue) {itsSoundingParameter = newValue;}
	int ModelRunIndex(void) const {return itsModelRunIndex;}
	void ModelRunIndex(int newValue) {itsModelRunIndex = newValue;}
private:
	NFmiDataIdent itsDataIdent;
	bool fUseDefaultProducer;
	NFmiCalculationCondition itsMaskCondition;
	NFmiAreaMask::CalculationOperationType itsOperationType; // t�m� menee p��llekk�in erilaisten maski ja info tyyppien kanssa, mutta piti tehd� 
						   // smarttooleja varten viel� t�mm�inen mm. hoitamaan laskuj�rjestyksi� ja sulkuja jne.
	NFmiAreaMask::CalculationOperator itsCalculationOperator; // jos operation, t�ss� tieto mist� niist� on kyse esim. +, -, * jne.
	NFmiAreaMask::BinaryOperator itsBinaryOperator;
	NFmiInfoData::Type itsDataType; // jos kyseess� infoVariable, tarvitaan viel� datan tyyppi, ett� parametri saadaan tietokannasta (=infoOrganizerista)
	NFmiLevel *itsLevel; // mahd. level tieto, omistaa ja tuhoaa
	std::string itsMaskText; // originaali teksti, mist� t�m� maskinfo on tulkittu, t�m� on siis vain yksi sana tai luku
	std::string itsOrigLineText; // originaali koko rivin teksti, mist� t�m� currentti sana (itsMaskText) on otettu (t�t� k�ytet��n virhe teksteiss�)
	NFmiAreaMask::FunctionType itsFunctionType; // onko mahd. funktio esim. min, max jne. (ei matemaattisia funktioita kuten sin, cos, pow, jne.)
	NFmiAreaMask::FunctionType itsSecondaryFunctionType; // T�h�n laitetaan mm. vertikaali funktioissa k�ytetty korkeus tyyppi esim. VertP tai VertZ
	NFmiPoint itsOffsetPoint1; // esim. aikaoffset (x alku ja y loppu) tai paikkaoffset (alku x ja y offset)
	NFmiPoint itsOffsetPoint2; // paikkaoffset (loppu x ja y offset)
	NFmiAreaMask::MathFunctionType itsMathFunctionType;
	int itsIntegrationFunctionType; // 1=SumT tyylinen ja 2=SumZ tyylinen ja 3=MinH tyylinen funktio
	int itsFunctionArgumentCount; // kuinka monta pilkulla eroteltua argumenttia on odotettavissa t�h�n 'meteorologiseen' funktioon (mm. grad, adv, div, lap, rot jne....).
	FmiSoundingParameters itsSoundingParameter;
	int itsModelRunIndex;
};
