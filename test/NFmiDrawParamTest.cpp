#include "NFmiDrawParam.h"
#include <regression/tframe.h>
#include <newbase/NFmiDataIdent.h>
#include <newbase/NFmiLevel.h>
#include <newbase/NFmiDataMatrix.h>

//! Protection against conflicts with global functions
namespace NFmiDrawParamTest
{
void constructors()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 0);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;

  try
  {
    NFmiDrawParam drawParam;
  }
  catch (...)
  {
    TEST_FAILED("exception: NFmiDrawParam default constructor failed!");
  }

  try
  {
    NFmiDrawParam drawParam(dataIdent, level, priority, dataType);
  }
  catch (...)
  {
    TEST_FAILED("exception: NFmiDrawParam constructor with arguments failed!");
  }

  try
  {
    NFmiDrawParam other;
    NFmiDrawParam second(other);
  }
  catch (...)
  {
    TEST_FAILED("exception: NFmiDrawParam copy constructor failed!");
  }

  NFmiDrawParam dP;

  try
  {
    const NFmiDrawParam* other = new NFmiDrawParam(dataIdent, level, priority, dataType);

    dP.Init(other);

    delete other;
  }
  catch (...)
  {
    TEST_FAILED("exception: NFmiDrawParam Init failed!");
  }

  TEST_PASSED();
}

void generalData()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;
  NFmiMetTime metTime(2017, 3, 8);

  NFmiDrawParam dP(dataIdent, level, priority, dataType);

  try
  {
    dP.Param(param);
    if (dP.Param().GetParamIdent() != dataIdent.GetParamIdent())
      TEST_FAILED("Value is not 10");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - Param - GetParamIdent ";
    throw;
  }

  try
  {
    dP.Level(level);
    if (dP.Level().LevelValue() != 5)
      TEST_FAILED("Value is not 5");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - Level - LevelValue ";
    throw;
  }

  try
  {
    dP.ParameterAbbreviation("FooBar");
    if (dP.ParameterAbbreviation() != "FooBar")
      TEST_FAILED("Value is not FooBar");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ParameterAbbreviation ";
  }

  try
  {
    dP.InitFileName("FooBar.name");
    if (dP.InitFileName() != "FooBar.name")
      TEST_FAILED("Value is not FooBar.name");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - InitFileName ";
    throw;
  }

  try
  {
    dP.MacroParamRelativePath("/fOO/Bar");
    if (dP.MacroParamRelativePath() != "/fOO/Bar")
      TEST_FAILED("Value is not /fOO/Bar");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - MacroParamRelativePath ";
    throw;
  }

  try
  {
    if (dP.FileVersionNumber() != 3.0)
      TEST_FAILED("Value is not 3.0");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - FileVersionNumber ";
    throw;
  }

  try
  {
    dP.EditParam(true);
    if (not dP.IsParamEdited())
      TEST_FAILED("It is not edited");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - IsParamEdited ";
    throw;
  }

  try
  {
    dP.Unit("ug/m2");
    if (dP.Unit() != "ug/m2")
      TEST_FAILED("Value is not ug/m2");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - Unit ";
    throw;
  }

  try
  {
    dP.Activate(true);
    if (not dP.IsActive())
      TEST_FAILED("It is not active");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - Activate ";
    throw;
  }

  try
  {
    dP.DataType(NFmiInfoData::Type::kViewable);
    if (dP.DataType() != NFmiInfoData::Type::kViewable)
      TEST_FAILED("Value is not NFmiInfoData::Type::kViewable");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - DataType ";
    throw;
  }

  try
  {
    dP.ViewMacroDrawParam(true);
    if (not dP.ViewMacroDrawParam())
      TEST_FAILED("It is not ViewMacroDrawParam");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ViewMacroDrawParam ";
    throw;
  }

  try
  {
    dP.BorrowedParam(true);
    if (not dP.BorrowedParam())
      TEST_FAILED("It is not BorrowedParam");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - BorrowedParam ";
    throw;
  }

  try
  {
    dP.ModelOriginTime(metTime);
    if (boost::posix_time::to_simple_string(dP.ModelOriginTime().PosixTime()) !=
        "2017-Mar-08 00:00:00")
      TEST_FAILED("Value is not 2017-Mar-08 00:00:00");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ModelOriginTime ";
    throw;
  }

  try
  {
    dP.ModelRunIndex(1);
    if (dP.ModelRunIndex() != 1)
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ModelRunIndex ";
    throw;
  }

  try
  {
    dP.ModelOriginTimeCalculated(metTime);
    if (dP.ModelOriginTimeCalculated() != metTime)
      TEST_FAILED("Value is not correct");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ModelOriginTimeCalculated ";
    throw;
  }

  try
  {
    dP.TimeSerialModelRunCount(2);
    if (dP.TimeSerialModelRunCount() != 2)
      TEST_FAILED("Value is not 2");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - TimeSerialModelRunCount ";
    throw;
  }

  try
  {
    dP.ModelRunDifferenceIndex(3);
    if (dP.ModelRunDifferenceIndex() != 3)
      TEST_FAILED("Value is not 3");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ModelRunDifferenceIndex ";
    throw;
  }

  try
  {
    dP.DataComparisonProdId(1);
    if (dP.DataComparisonProdId() != 1)
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - DataComparisonProdId ";
    throw;
  }

  try
  {
    dP.DataComparisonType(NFmiInfoData::Type::kStationary);
    if (dP.DataComparisonType() != NFmiInfoData::Type::kStationary)
      TEST_FAILED("Value is not NFmiInfoData::Type::kStationary");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - DataComparisonType ";
    throw;
  }

  TEST_PASSED();
}

void generalVisualization()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;

  NFmiDrawParam dP(dataIdent, level, priority, dataType);
  try
  {
    dP.ViewType(NFmiMetEditorTypes::View::kFmiIsoLineView);
    if (dP.ViewType() != NFmiMetEditorTypes::View::kFmiIsoLineView)
      TEST_FAILED("Value is not NFmiMetEditorTypes::View::kFmiIsoLineView");
  }
  catch (...)
  {
    std::cerr << "\n\tNFmiDrawParam - ViewType ";
    throw;
  }

  try
  {
    const int style = 2;
    dP.GridDataPresentationStyle(style);
    if (dP.GridDataPresentationStyle() != style)
      TEST_FAILED("Value is not 2");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - GridDataPresentationStyle";
    throw;
  }

  try
  {
    dP.UseIsoLineFeathering(true);
    if (not dP.UseIsoLineFeathering())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseIsoLineFeathering";
    throw;
  }

  try
  {
    dP.UseSimpleIsoLineDefinitions(true);
    if (not dP.UseSimpleIsoLineDefinitions())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseSimpleIsoLineDefinitions";
    throw;
  }

  try
  {
    dP.UseSimpleContourDefinitions(true);
    if (not dP.UseSimpleContourDefinitions())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseSimpleContourDefinitions";
    throw;
  }

  try
  {
    dP.UseSeparatorLinesBetweenColorContourClasses(true);
    if (not dP.UseSeparatorLinesBetweenColorContourClasses())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseSeparatorLinesBetweenColorContourClasses";
    throw;
  }

  try
  {
    dP.SimpleIsoLineZeroValue(-1);
    if (dP.SimpleIsoLineZeroValue() != -1)
      TEST_FAILED("Value is not -1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineZeroValue";
    throw;
  }

  try
  {
    dP.IsoLineSplineSmoothingFactor(2);
    if (dP.IsoLineSplineSmoothingFactor() != 2)
      TEST_FAILED("Value is not 2");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - IsoLineSplineSmoothingFactor";
    throw;
  }

  try
  {
    dP.DrawOnlyOverMask(true);
    if (not dP.DrawOnlyOverMask())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - DrawOnlyOverMask";
    throw;
  }

  try
  {
    dP.UseCustomColorContouring(true);
    if (not dP.UseCustomColorContouring())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseCustomColorContouring";
    throw;
  }

  try
  {
    dP.UseCustomIsoLineing(true);
    if (not dP.UseCustomIsoLineing())
      TEST_FAILED("Is not enabled");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseCustomIsoLineing";
    throw;
  }

  try
  {
    dP.Alpha(20.0);
    if (dP.Alpha() != 20.0)
      TEST_FAILED("Value is noe 20.0");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - Alpha";
    throw;
  }

  try
  {
    dP.HideParam(true);
    if (not dP.IsParamHidden())
      TEST_FAILED("Is not hidden");
    dP.HideParam(false);
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - HideParam";
    throw;
  }

  try
  {
    dP.ShowDifference(true);
    if (not dP.ShowDifference())
      TEST_FAILED("Does not show difference");
    dP.ShowDifference(false);
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ShowDifference";
    throw;
  }

  try
  {
    dP.ShowDifferenceToOriginalData(true);
    if (not dP.ShowDifferenceToOriginalData())
      TEST_FAILED("Does not show difference");
    dP.ShowDifferenceToOriginalData(false);
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ShowDifferenceToOriginalData";
    throw;
  }

  TEST_PASSED();
}

void simpleIsoline()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;

  NFmiDrawParam dP(dataIdent, level, priority, dataType);
  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.IsolineLabelBoxFillColor(color);
    if (dP.IsolineLabelBoxFillColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - IsolineLabelBoxFillColor ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.ContourLabelBoxFillColor(color);
    if (dP.ContourLabelBoxFillColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ContourLabelBoxFillColor ";
    throw;
  }

  try
  {
    dP.IsoLineGab(0.1);
    if (dP.IsoLineGab() != 0.1)
      TEST_FAILED("Value is not 0.1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - IsoLineGab ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.IsolineColor(color);
    if (dP.IsolineColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - IsoLineColor ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.IsolineTextColor(color);
    if (dP.IsolineTextColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - IsoLineTextColor ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineGap(0.1f);
    if (dP.SimpleIsoLineGap() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineGab ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineLabelHeight(0.1f);
    if (dP.SimpleIsoLineLabelHeight() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineLabelHeight ";
    throw;
  }

  try
  {
    dP.ShowSimpleIsoLineLabelBox(true);
    if (not dP.ShowSimpleIsoLineLabelBox())
      TEST_FAILED("Is not enabled");
    dP.ShowSimpleIsoLineLabelBox(false);
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ShowSimpleIsoLineLabelBox ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineWidth(0.1f);
    if (dP.SimpleIsoLineWidth() != 0.1f)
      TEST_FAILED("Value is not 0.1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineWidth ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineLineStyle(2);
    if (dP.SimpleIsoLineLineStyle() != 2)
      TEST_FAILED("Value is not 2");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineLineStyle ";
    throw;
  }

  try
  {
    dP.UseSingleColorsWithSimpleIsoLines(true);
    if (not dP.UseSingleColorsWithSimpleIsoLines())
      TEST_FAILED("Value is not enebled");
    dP.UseSingleColorsWithSimpleIsoLines(false);
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseSingleColorsWithSimpleIsoLines ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineColorShadeLowValue(0.1f);
    if (dP.SimpleIsoLineColorShadeLowValue() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeLowValue ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineColorShadeMidValue(0.2f);
    if (dP.SimpleIsoLineColorShadeMidValue() != 0.2f)
      TEST_FAILED("Value is not 0.2f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeMidValue ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineColorShadeHighValue(0.3f);
    if (dP.SimpleIsoLineColorShadeHighValue() != 0.3f)
      TEST_FAILED("Value is not 0.3f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeHighValue ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineColorShadeHigh2Value(0.2f);
    if (dP.SimpleIsoLineColorShadeHigh2Value() != 0.2f)
      TEST_FAILED("Value is not 0.2f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeHigh2Value ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.SimpleIsoLineColorShadeLowValueColor(color);
    if (dP.SimpleIsoLineColorShadeLowValueColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeLowValueColor ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.SimpleIsoLineColorShadeMidValueColor(color);
    if (dP.SimpleIsoLineColorShadeMidValueColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeMidValueColor ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.SimpleIsoLineColorShadeHighValueColor(color);
    if (dP.SimpleIsoLineColorShadeHighValueColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeHighValueColor ";
    throw;
  }

  try
  {
    NFmiColor color(1.0, 0.0, 0.0, 0.5);
    dP.SimpleIsoLineColorShadeHigh2ValueColor(color);
    if (dP.SimpleIsoLineColorShadeHigh2ValueColor() != color)
      TEST_FAILED("Color is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeHigh2ValueColor ";
    throw;
  }

  try
  {
    dP.SimpleIsoLineColorShadeClassCount(3);
    if (dP.SimpleIsoLineColorShadeClassCount() != 3)
      TEST_FAILED("Value is not 3");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleIsoLineColorShadeClassCount ";
    throw;
  }

  TEST_PASSED();
}

void simpleColorContour()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;
  NFmiColor color(1.0, 0.0, 0.0, 0.5);

  NFmiDrawParam dP(dataIdent, level, priority, dataType);

  try
  {
    dP.ContourGab(0.1);
    if (dP.ContourGab() != 0.1)
      TEST_FAILED("Value is not 0.1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ContourGab ";
    throw;
  }

  try
  {
    dP.ContourColor(color);
    if (dP.ContourColor() != color)
      TEST_FAILED("Value is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ContourColor ";
    throw;
  }

  try
  {
    dP.SimpleContourWidth(0.1f);
    if (dP.SimpleContourWidth() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleContourWidth ";
    throw;
  }

  try
  {
    dP.SimpleContourLineStyle(2);
    if (dP.SimpleContourLineStyle() != 2)
      TEST_FAILED("Value is not 2");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SimpleContourLineStyle ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeLowValue(0.1f);
    if (dP.ColorContouringColorShadeLowValue() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeLowValue ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeMidValue(0.1f);
    if (dP.ColorContouringColorShadeMidValue() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeMidValue ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeHighValue(0.1f);
    if (dP.ColorContouringColorShadeHighValue() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeHighValue ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeHigh2Value(0.1f);
    if (dP.ColorContouringColorShadeHigh2Value() != 0.1f)
      TEST_FAILED("Value is not 0.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeHigh2Value ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeLowValueColor(color);
    if (dP.ColorContouringColorShadeLowValueColor() != color)
      TEST_FAILED("Value is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeLowValueColor ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeMidValueColor(color);
    if (dP.ColorContouringColorShadeMidValueColor() != color)
      TEST_FAILED("Value is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeMidValueColor; ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeHighValueColor(color);
    if (dP.ColorContouringColorShadeHighValueColor() != color)
      TEST_FAILED("Value is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeHighValueColor ";
    throw;
  }

  try
  {
    dP.ColorContouringColorShadeHigh2ValueColor(color);
    if (dP.ColorContouringColorShadeHigh2ValueColor() != color)
      TEST_FAILED("Value is not red");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - ColorContouringColorShadeHigh2ValueColor ";
    throw;
  }

  TEST_PASSED();
}

void customIsoline()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;
  NFmiColor color(1.0, 0.0, 0.0, 0.5);
  checkedVector<float> floatVector;
  floatVector.push_back(-1.0f);
  floatVector.push_back(0.0f);
  floatVector.push_back(1.1f);

  NFmiDrawParam dP(dataIdent, level, priority, dataType);

  try
  {
    dP.SetSpecialIsoLineValues(floatVector);
    if (dP.SpecialIsoLineValues().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineValues ";
    throw;
  }

  try
  {
    dP.SetSpecialContourValues(floatVector);
    if (dP.SpecialContourValues().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam -  SpecialContourValues ";
    throw;
  }

  try
  {
    dP.SetSpecialIsoLineLabelHeight(floatVector);
    if (dP.SpecialIsoLineLabelHeight().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineLabelHeight ";
    throw;
  }

  try
  {
    dP.SetSpecialContourLabelHeight(floatVector);
    if (dP.SpecialContourLabelHeight().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialContourLabelHeight ";
    throw;
  }

  try
  {
    dP.SetSpecialIsoLineWidth(floatVector);
    if (dP.SpecialIsoLineWidth().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineWidth ";
    throw;
  }

  try
  {
    dP.SetSpecialcontourWidth(floatVector);
    if (dP.SpecialcontourWidth().at(2) != floatVector.at(2))
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialContourWidth ";
    throw;
  }

  checkedVector<int> intVector;
  intVector.push_back(-1);
  intVector.push_back(0);
  intVector.push_back(1);

  try
  {
    dP.SetSpecialIsoLineStyle(intVector);
    if (dP.SpecialIsoLineStyle().at(2) != intVector.at(2))
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineStyle ";
    throw;
  }

  try
  {
    dP.SetSpecialContourStyle(intVector);
    if (dP.SpecialContourStyle().at(2) != intVector.at(2))
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialContourStyle ";
    throw;
  }

  try
  {
    dP.SetSpecialIsoLineColorIndexies(intVector);
    if (dP.SpecialIsoLineColorIndexies().at(2) != intVector.at(2))
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineColorIndexies ";
    throw;
  }

  try
  {
    dP.SetSpecialContourColorIndexies(intVector);
    if (dP.SpecialContourColorIndexies().at(2) != intVector.at(2))
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialContourColorIndexies ";
    throw;
  }

  checkedVector<bool> boolVector;
  boolVector.push_back(false);
  boolVector.push_back(true);
  boolVector.push_back(false);

  try
  {
    dP.SpecialIsoLineShowLabelBox(boolVector);
    if (dP.SpecialIsoLineShowLabelBox().at(2) != boolVector.at(2))
      TEST_FAILED("Value is not false");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialIsoLineShowLabelBox ";
    throw;
  }

  TEST_PASSED();
}

void customColorContour()
{
  NFmiParam param(10);
  NFmiDataIdent dataIdent(param);
  NFmiLevel level(kFmiAnyLevelType, 5);
  int priority = 2;
  NFmiInfoData::Type dataType = NFmiInfoData::kEditable;
  NFmiColor color(1.0, 0.0, 0.0, 0.5);

  NFmiDrawParam dP(dataIdent, level, priority, dataType);

  try
  {
    dP.UseIsoLineGabWithCustomContours(true);
    if (not dP.UseIsoLineGabWithCustomContours())
      TEST_FAILED("Value is not true");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseIsoLineGabWithCustomContours ";
    throw;
  }

  try
  {
    dP.UseContourGabWithCustomContours(true);
    if (not dP.UseContourGabWithCustomContours())
      TEST_FAILED("Value is not true");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - UseContourGabWithCustomContours ";
    throw;
  }

  checkedVector<float> floatVector;
  floatVector.push_back(-1.0f);
  floatVector.push_back(0.0f);
  floatVector.push_back(1.1f);

  try
  {
    dP.SetSpecialColorContouringValues(floatVector);
    if (dP.SpecialColorContouringValues().at(2) != 1.1f)
      TEST_FAILED("Value is not 1.1f");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialColorContouringValues ";
    throw;
  }

  checkedVector<int> intVector;
  intVector.push_back(-1);
  intVector.push_back(0);
  intVector.push_back(1);

  try
  {
    dP.SpecialColorContouringColorIndexies(intVector);
    if (dP.SpecialColorContouringColorIndexies().at(2) != 1)
      TEST_FAILED("Value is not 1");
  }
  catch (...)
  {
    std::cerr << "\n\tFmiDrawParam - SpecialColorContouringColorIndexies ";
    throw;
  }

  TEST_PASSED();
}

void init()
{
}
// ----------------------------------------------------------------------
/*!
 * The actual test suite
 */
// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  virtual const char* error_message_prefix() const { return "\n\t"; }
  void test(void)
  {
    TEST(NFmiDrawParamTest::constructors);
    TEST(NFmiDrawParamTest::generalData);
    TEST(NFmiDrawParamTest::generalVisualization);
    TEST(NFmiDrawParamTest::simpleIsoline);
    TEST(NFmiDrawParamTest::simpleColorContour);
    TEST(NFmiDrawParamTest::customIsoline);
    TEST(NFmiDrawParamTest::customColorContour);
  }
};
}

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "NFmiDrawParamTest tester" << endl << "================" << endl;
  NFmiDrawParamTest::tests t;
  return t.run();
}

// ======================================================================
