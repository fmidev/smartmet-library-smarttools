//**********************************************************
// C++ Class Name : NFmiDrawParam 
// ---------------------------------------------------------
// Filetype: (HEADER)
// Filepath: D:/projekti/GDPro/GDTemp/NFmiDrawParam.h 
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
//  Creation Date  : Thur - Jan 28, 1999 
// 
// 
//  Description: 
//   En ole viel� varma t�m�n luokan tarkoituksesta/toiminnasta, 
//   t�m� on 
//   Persan idea.
// 
//  Change Log: 
// Changed 1999.09.28/Marko	Lis�sin SecondaryIsoLineColor:in k�yt�n (k�ytet��n 
//							vertailtaessa samantyyppist� dataa p��llekk�in)
// 
//**********************************************************
#ifndef  NFMIDRAWPARAM_H
#define  NFMIDRAWPARAM_H

#include "NFmiGlobals.h"
#include "NFmiParameterName.h"
#include "NFmiSaveBase.h"
#include "NFmiDataIdent.h"
#include "FmiNMeteditLibraryDefinitions.h"
#include "TFmiColor.h"
#include "NFmiPoint.h"

#include <vector>

class NFmiSmartInfo;
class NFmiDrawingEnvironment;
class NFmiString;
class NFmiDataIdent;
class NFmiMetEditorCoordinatorMapOptions;

class NFmiDrawParam : public NFmiSaveBase
{

public:
    NFmiDrawParam (void);
    NFmiDrawParam (NFmiSmartInfo* theInfo, const NFmiDataIdent& theParam, int thePriority, NFmiMetEditorCoordinatorMapOptions* theMetEditorCoordinatorMapOptions=0);
	NFmiDrawParam (NFmiSmartInfo* theInfo, const FmiParameterName& theParam, int thePriority, NFmiMetEditorCoordinatorMapOptions* theMetEditorCoordinatorMapOptions=0); // NFmiDrawParamFactory::CreateDrawParam k�ytt�� t�t�
	virtual  ~NFmiDrawParam (void);

	void Init(NFmiDrawParam* theDrawParam);
	void HideParam(FmiBoolean newValue){fHidden = newValue;};
	void EditParam(FmiBoolean newValue){fEditedParam = newValue;};
	FmiBoolean IsParamHidden(void){return fHidden;};
	FmiBoolean IsParamEdited(void){return fEditedParam;};
	FmiBoolean IsEditable(void){return fEditableParam;};
	void EditableParam(FmiBoolean newValue){fEditableParam = newValue;};

	FmiBoolean              Init (const NFmiString& theFilename = NFmiString());
	FmiBoolean              StoreData (const NFmiString& theFilename = NFmiString());

// --------------- "set" ja "get" metodit -----------------
	const NFmiString&	 ParameterAbbreviation (void) const;
	void				 ParameterAbbreviation(NFmiString theParameterAbbreviation) { itsParameterAbbreviation = theParameterAbbreviation; }
	const NFmiDataIdent& Param (void) const { return itsParameter; }; 
	const NFmiDataIdent& EditParam (void) const; 
	void                 Param (const NFmiDataIdent& theParameter) { itsParameter = theParameter; }; 
	void                 Info (NFmiSmartInfo* theInfo){ itsInfo = theInfo; }; 
	NFmiSmartInfo*       Info (void){ return itsInfo; } ; 
	void				 Priority (int thePriority){ itsPriority = thePriority; }; 
	int					 Priority (void){ return itsPriority; }; 
	void				 ViewType (const FmiEditorMapViewType& theViewType) { itsViewType = theViewType; };
	FmiEditorMapViewType ViewType (void) {return itsViewType;};
	void				 FrameColor (const TFmiColor& theFrameColor) { itsFrameColor = theFrameColor; };
	const TFmiColor&	 FrameColor (void) const { return itsFrameColor; };
	void				 FillColor (const TFmiColor& theFillColor) { itsFillColor = theFillColor; };
	const TFmiColor&	 FillColor (void) const { return itsFillColor; };
	void				 NonModifiableColor (const TFmiColor& theNonModifiableColor) { itsNonModifiableColor = theNonModifiableColor; };
	const TFmiColor&	 NonModifiableColor (void) const { return itsNonModifiableColor; };
	void				 RelativeSize (const NFmiPoint& theRelativeSize) { itsRelativeSize = theRelativeSize; };
	const NFmiPoint&	 RelativeSize (void) const { return itsRelativeSize; };
	void				 RelativePositionOffset (const NFmiPoint& theRelativePositionOffset) { itsRelativePositionOffset = theRelativePositionOffset; };
	const NFmiPoint&	 RelativePositionOffset (void) const { return itsRelativePositionOffset; };
	void				 OnlyOneSymbolRelativeSize (const NFmiPoint& theOnlyOneSymbolRelativeSize) { itsOnlyOneSymbolRelativeSize = theOnlyOneSymbolRelativeSize; };
	const NFmiPoint&	 OnlyOneSymbolRelativeSize (void) const { return itsOnlyOneSymbolRelativeSize; };
	void				 OnlyOneSymbolRelativePositionOffset (const NFmiPoint& theOnlyOneSymbolRelativePositionOffset) { itsOnlyOneSymbolRelativePositionOffset = theOnlyOneSymbolRelativePositionOffset; };
	const NFmiPoint&	 OnlyOneSymbolRelativePositionOffset (void) const { return itsOnlyOneSymbolRelativePositionOffset; };
	void				 ShowStationMarker (const FmiBoolean newState) { fShowStationMarker = newState; };
	FmiBoolean			 ShowStationMarker (void) { return fShowStationMarker; };
	void				 IsoLineGab (const double theIsoLineGab) { itsIsoLineGab = theIsoLineGab; };
	double				 IsoLineGab (void) { return itsIsoLineGab; };
	void				 ModifyingStep (const double theModifyingStep) { itsModifyingStep = theModifyingStep; };
	double				 ModifyingStep (void) { return itsModifyingStep; };
	void				 ModifyingUnit (FmiBoolean theModifyingUnit) { fModifyingUnit = theModifyingUnit; }
	FmiBoolean			 ModifyingUnit (void) { return fModifyingUnit; }
	FmiEditorMapViewType* PossibleViewTypeList (void) {return itsPossibleViewTypeList;}
	int					 PossibleViewTypeCount (void) { return itsPossibleViewTypeCount; };
	NFmiString			 InitFileName(void) { return itsInitFileName; }
	void				 InitFileName(NFmiString theFileName) { itsInitFileName = theFileName; }

	double AbsoluteMinValue(void) {return itsAbsoluteMinValue;}
	void AbsoluteMinValue(double theAbsoluteMinValue) {itsAbsoluteMinValue = theAbsoluteMinValue;}	
	double AbsoluteMaxValue(void) {return itsAbsoluteMaxValue;}
	void AbsoluteMaxValue(double theAbsoluteMaxValue) {itsAbsoluteMaxValue = theAbsoluteMaxValue;}
	double TimeSeriesScaleMin(void){return itsTimeSeriesScaleMin;};
	double TimeSeriesScaleMax(void){return itsTimeSeriesScaleMax;};
	void TimeSeriesScaleMin(double theValue){itsTimeSeriesScaleMin = theValue;};
	void TimeSeriesScaleMax(double theValue){itsTimeSeriesScaleMax = theValue;};
	TFmiColor IsolineColor(void){return fUseSecondaryColors ? itsSecondaryIsolineColor : itsIsolineColor;};
	TFmiColor IsolineTextColor(void){return fUseSecondaryColors ? itsSecondaryIsolineTextColor : itsIsolineTextColor;};
	void IsolineColor(const TFmiColor& newColor){itsIsolineColor = newColor;};
	void IsolineTextColor(const TFmiColor& newColor){itsIsolineTextColor = newColor;};
	double TimeSerialModifyingLimit(void){return fModifyingUnit ? itsTimeSerialModifyingLimit : 100;};
	FmiEditorMapViewType StationDataViewType(void){return itsStationDataViewType;};
	void TimeSerialModifyingLimit(double newValue){itsTimeSerialModifyingLimit = newValue;};
	void StationDataViewType(FmiEditorMapViewType newValue){itsStationDataViewType = newValue;};


	void				 FileVersionNumber (const float theFileVersionNumber) { itsFileVersionNumber = theFileVersionNumber; };
	float				 FileVersionNumber (void) { return itsFileVersionNumber; };

	void				 Unit (const NFmiString& theUnit) { itsUnit = theUnit; };
	const NFmiString&	 Unit (void) { return itsUnit; };

	FmiBoolean ShowNumbers(void) {return fShowNumbers;}
	void ShowNumbers(FmiBoolean theValue) {fShowNumbers = theValue;}
	FmiBoolean	ShowMasks(void) {return fShowMasks;}
	void	ShowMasks(FmiBoolean theValue) {fShowMasks = theValue;}
	FmiBoolean	ShowColors(void) {return fShowColors;}
	void	ShowColors(FmiBoolean theValue) {fShowColors = theValue;}
	FmiBoolean	ShowColoredNumbers(void) {return fShowColoredNumbers;}
	void	ShowColoredNumbers(FmiBoolean theValue) {fShowColoredNumbers = theValue;}
	FmiBoolean	ZeroColorMean(void) {return fZeroColorMean;}
	void	ZeroColorMean(FmiBoolean theValue) {fZeroColorMean = theValue;}
	FmiBoolean IsActive(void){return fActive;};
	void Activate(FmiBoolean newState){fActive = newState;};
	FmiBoolean UseSecondaryColors(void){return fUseSecondaryColors;};
	void UseSecondaryColors(FmiBoolean newState){fUseSecondaryColors = newState;};
	FmiBoolean ShowDifference(void){return fShowDifference;};
	void ShowDifference(FmiBoolean newState){fShowDifference = newState;};
	bool ShowDifferenceToOriginalData(void) const{return fShowDifferenceToOriginalData;}
	void ShowDifferenceToOriginalData(bool newValue){fShowDifferenceToOriginalData = newValue;}
	NFmiMetEditorCoordinatorMapOptions* MetEditorCoordinatorMapOptions(void){return itsMetEditorCoordinatorMapOptions;};
	void MetEditorCoordinatorMapOptions(NFmiMetEditorCoordinatorMapOptions* theNewOptions);

//**************************************************************
//********** 'versio 2' parametrien asetusfunktiot *************
//**************************************************************
	float StationSymbolColorShadeLowValue(void){return itsStationSymbolColorShadeLowValue;}
	void StationSymbolColorShadeLowValue(float newValue){itsStationSymbolColorShadeLowValue = newValue;}
	float StationSymbolColorShadeMidValue(void){return itsStationSymbolColorShadeMidValue;}
	void StationSymbolColorShadeMidValue(float newValue){itsStationSymbolColorShadeMidValue = newValue;}
	float StationSymbolColorShadeHighValue(void){return itsStationSymbolColorShadeHighValue;}
	void StationSymbolColorShadeHighValue(float newValue){itsStationSymbolColorShadeHighValue = newValue;}
	const TFmiColor& StationSymbolColorShadeLowValueColor(void){return itsStationSymbolColorShadeLowValueColor;}
	void StationSymbolColorShadeLowValueColor(const TFmiColor& newValue){itsStationSymbolColorShadeLowValueColor = newValue;}
	const TFmiColor& StationSymbolColorShadeMidValueColor(void){return itsStationSymbolColorShadeMidValueColor;}
	void StationSymbolColorShadeMidValueColor(const TFmiColor& newValue){itsStationSymbolColorShadeMidValueColor = newValue;}
	const TFmiColor& StationSymbolColorShadeHighValueColor(void){return itsStationSymbolColorShadeHighValueColor;}
	void StationSymbolColorShadeHighValueColor(const TFmiColor& newValue){itsStationSymbolColorShadeHighValueColor = newValue;}
	int StationSymbolColorShadeClassCount(void){return itsStationSymbolColorShadeClassCount;}
	void StationSymbolColorShadeClassCount(int newValue){itsStationSymbolColorShadeClassCount = newValue;}
	bool UseSymbolsInTextMode(void){return fUseSymbolsInTextMode;}
	void UseSymbolsInTextMode(bool newValue){fUseSymbolsInTextMode = newValue;}
	int UsedSymbolListIndex(void){return itsUsedSymbolListIndex;}
	void UsedSymbolListIndex(int newValue){itsUsedSymbolListIndex = newValue;}
	int SymbolIndexingMapListIndex(void){return itsSymbolIndexingMapListIndex;}
	void SymbolIndexingMapListIndex(int newValue){itsSymbolIndexingMapListIndex = newValue;}

	int GridDataPresentationStyle(void){return itsGridDataPresentationStyle;}
	void GridDataPresentationStyle(int newValue){itsGridDataPresentationStyle = newValue;}
	bool UseIsoLineFeathering(void){return fUseIsoLineFeathering;}
	void UseIsoLineFeathering(bool newValue){fUseIsoLineFeathering = newValue;}
	bool IsoLineLabelsOverLapping(void){return fIsoLineLabelsOverLapping;}
	void IsoLineLabelsOverLapping(bool newValue){fIsoLineLabelsOverLapping = newValue;}
	bool ShowColorLegend(void){return fShowColorLegend;}
	void ShowColorLegend(bool newValue){fShowColorLegend = newValue;}
	bool UseSimpleIsoLineDefinitions(void){return fUseSimpleIsoLineDefinitions;}
	void UseSimpleIsoLineDefinitions(bool newValue){fUseSimpleIsoLineDefinitions = newValue;}
	bool UseSeparatorLinesBetweenColorContourClasses(void){return fUseSeparatorLinesBetweenColorContourClasses;}
	void UseSeparatorLinesBetweenColorContourClasses(bool newValue){fUseSeparatorLinesBetweenColorContourClasses = newValue;}
	float SimpleIsoLineGap(void){return itsSimpleIsoLineGap;}
	void SimpleIsoLineGap(float newValue){itsSimpleIsoLineGap = newValue;}
	float SimpleIsoLineZeroValue(void){return itsSimpleIsoLineZeroValue;}
	void SimpleIsoLineZeroValue(float newValue){itsSimpleIsoLineZeroValue = newValue;}
	float SimpleIsoLineLabelHeight(void){return itsSimpleIsoLineLabelHeight;}
	void SimpleIsoLineLabelHeight(float newValue){itsSimpleIsoLineLabelHeight = newValue;}
	bool ShowSimpleIsoLineLabelBox(void){return fShowSimpleIsoLineLabelBox;}
	void ShowSimpleIsoLineLabelBox(bool newValue){fShowSimpleIsoLineLabelBox = newValue;}
	float SimpleIsoLineWidth(void){return itsSimpleIsoLineWidth;}
	void SimpleIsoLineWidth(float newValue){itsSimpleIsoLineWidth = newValue;}
	int SimpleIsoLineLineStyle(void){return itsSimpleIsoLineLineStyle;}
	void SimpleIsoLineLineStyle(int newValue){itsSimpleIsoLineLineStyle = newValue;}
	float IsoLineSplineSmoothingFactor(void){return itsIsoLineSplineSmoothingFactor;}
	void IsoLineSplineSmoothingFactor(float newValue){itsIsoLineSplineSmoothingFactor = newValue;}
	bool UseSingleColorsWithSimpleIsoLines(void){return fUseSingleColorsWithSimpleIsoLines;}
	void UseSingleColorsWithSimpleIsoLines(bool newValue){fUseSingleColorsWithSimpleIsoLines = newValue;}
	float SimpleIsoLineColorShadeLowValue(void){return itsSimpleIsoLineColorShadeLowValue;}
	void SimpleIsoLineColorShadeLowValue(float newValue){itsSimpleIsoLineColorShadeLowValue = newValue;}
	float SimpleIsoLineColorShadeMidValue(void){return itsSimpleIsoLineColorShadeMidValue;}
	void SimpleIsoLineColorShadeMidValue(float newValue){itsSimpleIsoLineColorShadeMidValue = newValue;}
	float SimpleIsoLineColorShadeHighValue(void){return itsSimpleIsoLineColorShadeHighValue;}
	void SimpleIsoLineColorShadeHighValue(float newValue){itsSimpleIsoLineColorShadeHighValue = newValue;}
	const TFmiColor& SimpleIsoLineColorShadeLowValueColor(void){return itsSimpleIsoLineColorShadeLowValueColor;}
	void SimpleIsoLineColorShadeLowValueColor(const TFmiColor& newValue){itsSimpleIsoLineColorShadeLowValueColor = newValue;}
	const TFmiColor& SimpleIsoLineColorShadeMidValueColor(void){return itsSimpleIsoLineColorShadeMidValueColor;}
	void SimpleIsoLineColorShadeMidValueColor(const TFmiColor& newValue){itsSimpleIsoLineColorShadeMidValueColor = newValue;}
	const TFmiColor& SimpleIsoLineColorShadeHighValueColor(void){return itsSimpleIsoLineColorShadeHighValueColor;}
	void SimpleIsoLineColorShadeHighValueColor(const TFmiColor& newValue){itsSimpleIsoLineColorShadeHighValueColor = newValue;}
	int SimpleIsoLineColorShadeClassCount(void){return itsSimpleIsoLineColorShadeClassCount;}
	void SimpleIsoLineColorShadeClassCount(int newValue){itsSimpleIsoLineColorShadeClassCount = newValue;}
	std::vector<float>& SpecialIsoLineValues(void){return itsSpecialIsoLineValues;}
	void SpecialIsoLineValues(std::vector<float>& newValue){itsSpecialIsoLineValues = newValue;}
	std::vector<float>& SpecialIsoLineLabelHeight(void){return itsSpecialIsoLineLabelHeight;}
	void SpecialIsoLineLabelHeight(std::vector<float>& newValue){itsSpecialIsoLineLabelHeight = newValue;}
	std::vector<float>& SpecialIsoLineWidth(void){return itsSpecialIsoLineWidth;}
	void SpecialIsoLineWidth(std::vector<float>& newValue){itsSpecialIsoLineWidth = newValue;}
	std::vector<int>& SpecialIsoLineStyle(void){return itsSpecialIsoLineStyle;}
	void SpecialIsoLineStyle(std::vector<int>& newValue){itsSpecialIsoLineStyle = newValue;}
	std::vector<int>& SpecialIsoLineColorIndexies(void){return itsSpecialIsoLineColorIndexies;}
	void SpecialIsoLineColorIndexies(std::vector<int>& newValue){itsSpecialIsoLineColorIndexies = newValue;}
	std::vector<bool>& SpecialIsoLineShowLabelBox(void){return itsSpecialIsoLineShowLabelBox;}
	void SpecialIsoLineShowLabelBox(std::vector<bool>& newValue){itsSpecialIsoLineShowLabelBox = newValue;}
	bool UseDefaultRegioning(void){return fUseDefaultRegioning;}
	void UseDefaultRegioning(bool newValue){fUseDefaultRegioning = newValue;}
	bool UseCustomColorContouring(void){return fUseCustomColorContouring;}
	void UseCustomColorContouring(bool newValue){fUseCustomColorContouring = newValue;}
	std::vector<float>& SpecialColorContouringValues(void){return itsSpecialColorContouringValues;}
	void SpecialColorContouringValues(std::vector<float>& newValue){itsSpecialColorContouringValues = newValue;}
	std::vector<int>& SpecialColorContouringColorIndexies(void){return itsSpecialColorContouringColorIndexies;}
	void SpecialColorContouringColorIndexies(std::vector<int>& newValue){itsSpecialColorContouringColorIndexies = newValue;}
	float ColorContouringColorShadeLowValue(void){return itsColorContouringColorShadeLowValue;}
	void ColorContouringColorShadeLowValue(float newValue){itsColorContouringColorShadeLowValue = newValue;}
	float ColorContouringColorShadeMidValue(void){return itsColorContouringColorShadeMidValue;}
	void ColorContouringColorShadeMidValue(float newValue){itsColorContouringColorShadeMidValue = newValue;}
	float ColorContouringColorShadeHighValue(void){return itsColorContouringColorShadeHighValue;}
	void ColorContouringColorShadeHighValue(float newValue){itsColorContouringColorShadeHighValue = newValue;}
	const TFmiColor& ColorContouringColorShadeLowValueColor(void){return itsColorContouringColorShadeLowValueColor;}
	void ColorContouringColorShadeLowValueColor(const TFmiColor& newValue){itsColorContouringColorShadeLowValueColor = newValue;}
	const TFmiColor& ColorContouringColorShadeMidValueColor(void){return itsColorContouringColorShadeMidValueColor;}
	void ColorContouringColorShadeMidValueColor(const TFmiColor& newValue){itsColorContouringColorShadeMidValueColor = newValue;}
	const TFmiColor& ColorContouringColorShadeHighValueColor(void){return itsColorContouringColorShadeHighValueColor;}
	void ColorContouringColorShadeHighValueColor(const TFmiColor& newValue){itsColorContouringColorShadeHighValueColor = newValue;}
	int ColorContouringColorShadeClassCount(void){return itsColorContouringColorShadeClassCount;}
	void ColorContouringColorShadeClassCount(int newValue){itsColorContouringColorShadeClassCount = newValue;}
	bool UseWithIsoLineHatch1(void){return fUseWithIsoLineHatch1;}
	void UseWithIsoLineHatch1(bool newValue){fUseWithIsoLineHatch1 = newValue;}
	bool DrawIsoLineHatchWithBorders1(void){return fDrawIsoLineHatchWithBorders1;}
	void DrawIsoLineHatchWithBorders1(bool newValue){fDrawIsoLineHatchWithBorders1 = newValue;}
	float IsoLineHatchLowValue1(void){return itsIsoLineHatchLowValue1;}
	void IsoLineHatchLowValue1(float newValue){itsIsoLineHatchLowValue1 = newValue;}
	float IsoLineHatchHighValue1(void){return itsIsoLineHatchHighValue1;}
	void IsoLineHatchHighValue1(float newValue){itsIsoLineHatchHighValue1 = newValue;}
	int IsoLineHatchType1(void){return itsIsoLineHatchType1;}
	void IsoLineHatchType1(int newValue){itsIsoLineHatchType1 = newValue;}
	const TFmiColor& IsoLineHatchColor1(void){return itsIsoLineHatchColor1;}
	void IsoLineHatchColor1(const TFmiColor& newValue){itsIsoLineHatchColor1 = newValue;}
	const TFmiColor& IsoLineHatchBorderColor1(void){return itsIsoLineHatchBorderColor1;}
	void IsoLineHatchBorderColor1(const TFmiColor& newValue){itsIsoLineHatchBorderColor1 = newValue;}

	bool UseWithIsoLineHatch2(void){return fUseWithIsoLineHatch2;}
	void UseWithIsoLineHatch2(bool newValue){fUseWithIsoLineHatch2 = newValue;}
	bool DrawIsoLineHatchWithBorders2(void){return fDrawIsoLineHatchWithBorders2;}
	void DrawIsoLineHatchWithBorders2(bool newValue){fDrawIsoLineHatchWithBorders2 = newValue;}
	float IsoLineHatchLowValue2(void){return itsIsoLineHatchLowValue2;}
	void IsoLineHatchLowValue2(float newValue){itsIsoLineHatchLowValue2 = newValue;}
	float IsoLineHatchHighValue2(void){return itsIsoLineHatchHighValue2;}
	void IsoLineHatchHighValue2(float newValue){itsIsoLineHatchHighValue2 = newValue;}
	int IsoLineHatchType2(void){return itsIsoLineHatchType2;}
	void IsoLineHatchType2(int newValue){itsIsoLineHatchType2 = newValue;}
	const TFmiColor& IsoLineHatchColor2(void){return itsIsoLineHatchColor2;}
	void IsoLineHatchColor2(const TFmiColor& newValue){itsIsoLineHatchColor2 = newValue;}
	const TFmiColor& IsoLineHatchBorderColor2(void){return itsIsoLineHatchBorderColor2;}
	void IsoLineHatchBorderColor2(const TFmiColor& newValue){itsIsoLineHatchBorderColor2 = newValue;}
	int IsoLineLabelDigitCount(void){return itsIsoLineLabelDigitCount;}
	void IsoLineLabelDigitCount(int newValue){itsIsoLineLabelDigitCount = newValue; if(itsIsoLineLabelDigitCount > 10)itsIsoLineLabelDigitCount = 10;}
//**************************************************************
//********** 'versio 2' parametrien asetusfunktiot *************
//**************************************************************

// ---------------------- operators ------------------------
	FmiBoolean operator == (const NFmiDrawParam & theDrawParam) const;
	FmiBoolean operator < (const NFmiDrawParam & theDrawParam) const;
	virtual std::ostream & Write (std::ostream &file);
	virtual std::istream & Read (std::istream &file);


protected:

    NFmiDataIdent itsParameter;
	NFmiString itsParameterAbbreviation;
    int itsPriority;

//  Parametrin oletus n�ytt�tyyppi (symboli, 
//  isoviiva, teksti...)
	FmiEditorMapViewType itsViewType;
	FmiEditorMapViewType itsStationDataViewType; // jos viewtype on isoviiva, mutta data on asema dataa, pit�� olla varalla joku n�ytt�tyyppi ett� voidaan piirt�� t�ll�in
	TFmiColor itsFrameColor;
	TFmiColor itsFillColor;

//   Jos jonkin paikan arvo on ei-maskattu, eli 
//   sit� ei voi muuttaa koska
//   parametrimaskaus ei salli, v�ritet��n data 
//   t�ll� v�rill� (oletusarvo).
	TFmiColor itsNonModifiableColor;
//   Mink� kokoinen n�ytt��n piirrett�v� 'symbolidata' 
//   on suhteessa
//   annettuun asemalle/hilalle varattuun 'datalaatikkoon'. 
//   (oletusarvo)
	NFmiPoint itsRelativeSize;	// n�m� ovat asemadata symboli kokoja
	NFmiPoint itsRelativePositionOffset;
	NFmiPoint itsOnlyOneSymbolRelativeSize;
	NFmiPoint itsOnlyOneSymbolRelativePositionOffset;
	FmiBoolean fShowStationMarker;
	double itsIsoLineGab;
	double itsModifyingStep;
	FmiBoolean fModifyingUnit;	//(= 0, jos yksikk� on %, = 1, jos yksikk� on sama kuin itsUnit)
	double itsTimeSerialModifyingLimit; // aikasarjan�yt�n muutos akselin minimi ja maksimi arvo
	TFmiColor itsIsolineColor;
	TFmiColor itsIsolineTextColor;
	TFmiColor itsSecondaryIsolineColor;
	TFmiColor itsSecondaryIsolineTextColor;
	FmiBoolean fUseSecondaryColors;

	double itsAbsoluteMinValue;
	double itsAbsoluteMaxValue;

	double itsTimeSeriesScaleMin; // k�ytet��n aikasarjaeditorissa
	double itsTimeSeriesScaleMax; // k�ytet��n aikasarjaeditorissa

//   Lista mahdollisista n�ytt�tyypeist� kyseiselle 
//   parametrille.
	FmiEditorMapViewType itsPossibleViewTypeList[5];
	int itsPossibleViewTypeCount;

	// Tekstin�ytt�:
	FmiBoolean	fShowNumbers;
	FmiBoolean	fShowMasks; // t�m� on turha!!!!!
	FmiBoolean	fShowColors; // asema teksti� varten
	FmiBoolean	fShowColoredNumbers; // asema teksti� varten
	FmiBoolean	fZeroColorMean; // asema teksti� varten

//***********************************************
//********** 'versio 2' parametreja *************
//***********************************************
	float itsStationSymbolColorShadeLowValue; //v�ri skaalaus alkaa t�st� arvosta
	float itsStationSymbolColorShadeMidValue; //v�ri skaalauksen keskiarvo
	float itsStationSymbolColorShadeHighValue; //v�ri skaalaus loppuu t�h�n arvoon
	TFmiColor itsStationSymbolColorShadeLowValueColor;
	TFmiColor itsStationSymbolColorShadeMidValueColor;
	TFmiColor itsStationSymbolColorShadeHighValueColor;
	int itsStationSymbolColorShadeClassCount; // kuinka monta v�ri luokkaa tehd��n skaalaukseen
	bool fUseSymbolsInTextMode; // k�ytet��nk� teksti� vai mapataanko arvoja kohden jokin symboli ruudulle?
	int itsUsedSymbolListIndex; // 0=ei mit��n, 1=synopfont, 2=hessaa, ...
	int itsSymbolIndexingMapListIndex; // indeksi johonkin symbolilistaan, jossa on mapattu arvot haluttuihin symboleihin

	int itsGridDataPresentationStyle; // 1=asema muodossa, 2=isoviiva, 3=color contouring, 4= contour+isoviiva, 5=quick color contouring
	bool fUseIsoLineFeathering; // isoviivojen harvennus optio
	bool fIsoLineLabelsOverLapping; // voivatko isoviiva labelit menn� p��llekk�in vai ei?
	bool fShowColorLegend; // piirret��nko rivin viimeiseen ruutuun color contouringin v�ri selitys laatikko?
	bool fUseSimpleIsoLineDefinitions; // otetaanko isoviivam��ritykset simppelill� tavalla, vai kaikki arvot spesiaali tapauksina m��ritettyin�
	bool fUseSeparatorLinesBetweenColorContourClasses; // piirr� viivat arvo/v�ri luokkien v�lille
	float itsSimpleIsoLineGap;
	float itsSimpleIsoLineZeroValue; // t�m�n arvon kautta isoviivat joutuvat menem��n
	float itsSimpleIsoLineLabelHeight; // relatiivinen vai mm? (0=ei n�ytet� ollenkaan)
	bool fShowSimpleIsoLineLabelBox; // ei viel� muita attribuutteja isoviiva labelille (t�m� tarkoittaa lukua ymp�roiv�� laatikkoa)
	float itsSimpleIsoLineWidth; // relatiivinen vai mm?
	int itsSimpleIsoLineLineStyle; // 1=yht. viiva, 2=pisteviiva jne.
	float itsIsoLineSplineSmoothingFactor; // 0-10, 0=ei pehmennyst�, 10=maksimi py�ritys
	bool fUseSingleColorsWithSimpleIsoLines; // true=sama v�ri kaikille isoviivoille, false=tehd��n v�riskaala
// isoviivan v�riskaalaus asetukset
	float itsSimpleIsoLineColorShadeLowValue; //v�ri skaalaus alkaa t�st� arvosta
	float itsSimpleIsoLineColorShadeMidValue; //v�ri skaalauksen keskiarvo
	float itsSimpleIsoLineColorShadeHighValue; //v�ri skaalaus loppuu t�h�n arvoon
	TFmiColor itsSimpleIsoLineColorShadeLowValueColor;
	TFmiColor itsSimpleIsoLineColorShadeMidValueColor;
	TFmiColor itsSimpleIsoLineColorShadeHighValueColor;
	int itsSimpleIsoLineColorShadeClassCount; // kuinka monta v�ri luokkaa tehd��n skaalaukseen
// speciaali isoviiva asetukset (otetaan k�ytt��n, jos fUseSimpleIsoLineDefinitions=false)
	std::vector<float> itsSpecialIsoLineValues; // t�h�n laitetaan kaikki arvot, johon halutaan isoviiva
	std::vector<float> itsSpecialIsoLineLabelHeight; // isoviivalabeleiden korkeudet (0=ei labelia)
	std::vector<float> itsSpecialIsoLineWidth; // viivan paksuudet
	std::vector<int> itsSpecialIsoLineStyle; // viiva tyylit
	std::vector<int> itsSpecialIsoLineColorIndexies; // eri viivojen v�ri indeksit (pit�� tehd� n�ytt� taulukko k�ytt�j�lle)
	std::vector<bool> itsSpecialIsoLineShowLabelBox; // eri viivojen v�ri indeksit (pit�� tehd� n�ytt� taulukko k�ytt�j�lle)
// colorcontouring ja quick contouring asetukset
	bool fUseDefaultRegioning; // jos true, data piirret��n vain defaultregionin (maan) p��lle
	bool fUseCustomColorContouring; // true ja saa m��ritell� luokka rajat ja v�rit
	std::vector<float> itsSpecialColorContouringValues; // t�h�n laitetaan kaikki arvot, johon halutaan color contour luokka rajoiksi
	std::vector<int> itsSpecialColorContouringColorIndexies; // eri viivojen v�ri indeksit (pit�� tehd� n�ytt� taulukko k�ytt�j�lle)
	float itsColorContouringColorShadeLowValue; //v�ri skaalaus alkaa t�st� arvosta
	float itsColorContouringColorShadeMidValue; //v�ri skaalauksen keskiarvo
	float itsColorContouringColorShadeHighValue; //v�ri skaalaus loppuu t�h�n arvoon
	TFmiColor itsColorContouringColorShadeLowValueColor;
	TFmiColor itsColorContouringColorShadeMidValueColor;
	TFmiColor itsColorContouringColorShadeHighValueColor;
	int itsColorContouringColorShadeClassCount; // kuinka monta v�ri luokkaa tehd��n skaalaukseen
// isoviivojen kanssa voi k�ytt�� my�s hatch�ttyj� alueita (2 kpl)
	bool fUseWithIsoLineHatch1;
	bool fDrawIsoLineHatchWithBorders1;
	float itsIsoLineHatchLowValue1; // hatch alueen ala-arvo
	float itsIsoLineHatchHighValue1; // hatch alueen yl�arvo
	int itsIsoLineHatchType1; // hatch tyyppi 1=vinoviiva oikealle, 2=vinoviiva vasemmalle jne.
	TFmiColor itsIsoLineHatchColor1;
	TFmiColor itsIsoLineHatchBorderColor1;
	bool fUseWithIsoLineHatch2;
	bool fDrawIsoLineHatchWithBorders2;
	float itsIsoLineHatchLowValue2; // hatch alueen ala-arvo
	float itsIsoLineHatchHighValue2; // hatch alueen yl�arvo
	int itsIsoLineHatchType2; // hatch tyyppi 1=vinoviiva oikealle, 2=vinoviiva vasemmalle jne.
	TFmiColor itsIsoLineHatchColor2;
	TFmiColor itsIsoLineHatchBorderColor2;
	int itsIsoLineLabelDigitCount; // isoviiva labelin n�ytett�vien digitaalien lukum��r� 
//***********************************************
//********** 'versio 2' parametreja *************
//***********************************************

private:

	static float itsFileVersionNumber;
	float itsInitFileVersionNumber;

//   T�h�n talletetaan tiedoston nimi, mist� luetaan/mihin 
//   kirjoitetaan 
//   kyseisen luokan tiedot.
	NFmiString itsInitFileName;

	FmiBoolean fHidden;			// n�ytt� voidaan piiloittaa t�m�n mukaisesti
	FmiBoolean fEditedParam;	// vain yksi parametreista voidaan editoida yht� aikaa
	FmiBoolean fEditableParam;	// onko parametri suoraan editoitavissa ollenkaan? (esim. HESSAA tai tuulivektori eiv�t ole)

	NFmiSmartInfo* itsInfo;
	NFmiString itsUnit;			
	FmiBoolean fActive;			// onko kyseinen parametri n�yt�n aktiivinen parametri (jokaisella n�ytt�rivill� aina yksi aktivoitunut parametri)
	FmiBoolean fShowDifference;	// n�ytet��nk� kartalla parametrin arvo, vai erotus edelliseen aikaan (ei ole viel� talletettu tiedostoon)
	bool fShowDifferenceToOriginalData;

	NFmiMetEditorCoordinatorMapOptions* itsMetEditorCoordinatorMapOptions; // t�t� k�ytet��n koordinaatio tarkasteluissa
};

#endif
