#include "NFmiDrawParam.h"
#include <regression/tframe.h>
#include <newbase/NFmiDataIdent.h>
#include <newbase/NFmiLevel.h>

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
