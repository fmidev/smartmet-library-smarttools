#include "NFmiPathUtils.h"

#include <newbase/NFmiFileString.h>
#include <newbase/NFmiSettings.h>
#include <newbase/NFmiStringTools.h>
#include "boost/algorithm/string/replace.hpp"
#ifndef UNIX
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#endif

namespace
{
bool hasDriveLetterInPath(const std::string &filePath, bool mustHaveDriveLetter)
{
  // 1) Minimum of 3 characters in path that start with a-z letter, then comes ':' and third letter
  // is slash character in either way '\' or '/'
  if (filePath.size() >= 3)
  {
    if (::isalpha(filePath[0]) && filePath[1] == ':' && (filePath[2] == '\\' || filePath[2] == '/'))
      return true;
  }

  // With 'source' paths set mustHaveDriveLetter to true and with 'destination' paths to false.
  if (!mustHaveDriveLetter)
  {
    // 2) Minimum of 3 characters in path that starts with double slashes // or \\ and then a
    // letter, that means in Windows that this is absolute network path (e.g.
    // \\server\path_in_server) and it's treated like path with drive-letter in it.
    if (filePath.size() >= 3)
    {
      if ((filePath[0] == '\\' || filePath[0] == '/') &&
          (filePath[1] == '\\' || filePath[1] == '/') && ::isalpha(filePath[2]))
        return true;
    }
  }
  return false;
}

std::vector<std::string> split(std::string path, char d)
{
  std::vector<std::string> r;
  size_t j = 0;
  for (size_t i = 0; i < path.length(); i++)
  {
    if (path[i] == d)
    {
      std::string cur = path.substr(j, i - j);
      if (cur.length())
      {
        r.push_back(cur);
      }
      j = i + 1;  // start of next match
    }
  }
  if (j < path.length())
  {
    r.push_back(path.substr(j));
  }
  return r;
}

std::string simplifyUnixPath(std::string path)
{
  std::vector<std::string> ps = split(path, '/');
  std::string p = "";
  std::vector<std::string> st;
  for (size_t i = 0; i < ps.size(); i++)
  {
    if (ps[i] == "..")
    {
      if (st.size() > 0)
      {
        st.pop_back();
      }
    }
    else if (ps[i] != ".")
    {
      st.push_back(ps[i]);
    }
  }
  for (size_t i = 0; i < st.size(); i++)
  {
    p += "/" + st[i];
  }
  return p.length() ? p : "/";
}

}  // namespace

namespace PathUtils
{
// If filePath is absolute path without drive letter, but usedAbsoluteBaseDirectory contains drive
// letter with ':' character, add that driveletter and ':' character to filePath's start. Presumes
// that filePath is absolute path with or without drive letter like "\path\zzz" or "c:\path\zzz".
// Presumes that usedAbsoluteBaseDirectory has absolute path with drive letter like "c:\path\zzz".
std::string fixMissingDriveLetterToAbsolutePath(const std::string &filePath,
                                                const std::string &usedAbsoluteBaseDirectory)
{
  if (!::hasDriveLetterInPath(filePath, false) &&
      ::hasDriveLetterInPath(usedAbsoluteBaseDirectory, true))
  {
    std::string pathWithDriveLetter(usedAbsoluteBaseDirectory.begin(),
                                    usedAbsoluteBaseDirectory.begin() + 2);
    return pathWithDriveLetter + filePath;
  }
  else
    return filePath;
}

std::string getAbsoluteFilePath(const std::string &filePath,
                                const std::string &usedAbsoluteBaseDirectory)
{
  std::string finalAbsoluteFilePath;
  NFmiFileString fileString(filePath);
  if (fileString.IsAbsolutePath())
  {
    finalAbsoluteFilePath =
        fixMissingDriveLetterToAbsolutePath(filePath, usedAbsoluteBaseDirectory);
  }
  else
  {
    std::string absolutePath = usedAbsoluteBaseDirectory;
    if (!lastCharacterIsSeparator(absolutePath))
    {
    absolutePath += kFmiDirectorySeparator;
    }
    absolutePath += filePath;
    finalAbsoluteFilePath = absolutePath;
  }
  return simplifyWindowsPath(finalAbsoluteFilePath);
}

std::string getPathSectionFromTotalFilePath(const std::string &theFilePath)
{
  NFmiFileString filePath(theFilePath);
  std::string directoryPart = filePath.Device().CharPtr();
  directoryPart += filePath.Path();
  return simplifyWindowsPath(directoryPart);
}

// Esim. theAbsoluteFilePath = D:\xxx\yyy\zzz\macro.mac, theBaseDirectory = D:\xxx ja
// theStrippedFileExtension = mac
// => yyy\zzz\macro
std::string getRelativeStrippedFileName(const std::string &theAbsoluteFilePath,
                                        const std::string &theBaseDirectory,
                                        const std::string &theStrippedFileExtension)
{
  std::string relativeFilePath = getRelativePathIfPossible(theAbsoluteFilePath, theBaseDirectory);
  std::string usedFileExtension = theStrippedFileExtension;
  if (usedFileExtension.size() && usedFileExtension[0] != '.')
    usedFileExtension =
        "." + theStrippedFileExtension;  // pit�� mahdollisesti lis�t� . -merkki alkuun
  return NFmiStringTools::ReplaceAll(relativeFilePath, usedFileExtension, "");
}

// Lis�t��n loppuun kenoviiva, jos siell� ei jo sellaista ole.
void addDirectorySeparatorAtEnd(std::string &thePathInOut)
{
  if (thePathInOut.size() && thePathInOut[thePathInOut.size() - 1] != '\\' &&
      thePathInOut[thePathInOut.size() - 1] != '/')
    thePathInOut += kFmiDirectorySeparator;
}

// Yritt�� palauttaa annetusta theFilePath:ista sen suhteellisen osion, joka j�� j�ljelle
// theBaseDirectoryPath:in j�lkeen. Jos theFilePath:in ja theBaseDirectoryPath eiv�t osu
// yhteen, palautetaan originaali arvo.
// Jos theFilePath on suhteellinen polku, palautetaan originaali arvo.
// Esim1: "C:\xxx\data.txt", "C:\xxx"   => "data.txt"
// Esim2: "C:\xxx\data.txt", "C:\yyy"   => "C:\xxx\data.txt"
// Esim3: "xxx\data.txt", "\xxx"      => "xxx\data.txt"
std::string getRelativePathIfPossible(const std::string &theFilePath,
                                      const std::string &theBaseDirectoryPath)
{
  if (!theBaseDirectoryPath.empty())
  {
    NFmiFileString filePathString(fixPathSeparators(theFilePath));
    NFmiFileString baseDirectoryPathString(fixPathSeparators(theBaseDirectoryPath));
    if (filePathString.IsAbsolutePath() && baseDirectoryPathString.IsAbsolutePath())
    {
      auto usedFilePath = doDriveLetterFix(filePathString, baseDirectoryPathString);
      std::string filePathLowerCase(usedFilePath);
      NFmiStringTools::LowerCase(filePathLowerCase);
      std::string baseDirectoryPathLowerCase(theBaseDirectoryPath);
      NFmiStringTools::LowerCase(baseDirectoryPathLowerCase);

      std::string::size_type pos = filePathLowerCase.find(baseDirectoryPathLowerCase);
      if (pos != std::string::npos)
      {
        std::string relativePath(usedFilePath.begin() + theBaseDirectoryPath.size(),
                                 usedFilePath.end());
        std::string::size_type pos2 = relativePath.find_first_not_of("\\/");
        std::string finalRelativePath;
        if (pos2 != std::string::npos)
        {
          // Otetaan viel� polun alusta pois mahdolliset kenoviivat
          finalRelativePath = std::string(relativePath.begin() + pos2, relativePath.end());
        }
        else
        {
          finalRelativePath = relativePath;
      }
        return finalRelativePath;
      }
    }
  }

  return theFilePath;
}

// Yritt�� hakea tiedostolle sen lopullisen absoluuttisen polun extensioineen kaikkineen.
// Esim1 "beta1" "D:\betaProducts" "BetaProd"                           =>
// D:\betaProducts\beta1.BetaProd" Esim2 "D:\betaProducts\beta1.1" "D:\betaProducts" "BetaProd" =>
// D:\betaProducts\beta1.1.BetaProd" Esim3 "D:\betaProducts\beta1.BetaProd" "D:\betaProducts"
// "BetaProd"  => D:\betaProducts\beta1.BetaProd" Esim4 "xxx\beta1" "D:\betaProducts" "BetaProd" =>
// D:\betaProducts\xxx\beta1.BetaProd"
std::string getTrueFilePath(const std::string &theOriginalFilePath,
                            const std::string &theRootDirectory,
                            const std::string &theFileExtension,
                            bool *extensionAddedOut)
{
  if (extensionAddedOut)
    *extensionAddedOut = false;
  std::string filePath = theOriginalFilePath;
  NFmiStringTools::Trim(filePath);  // Siivotaan annetusta polusta alusta ja lopusta white spacet

  if (filePath.empty())
    return filePath;
  else
  {
    // Tutkitaan onko kyseess� absoluuttinen vai suhteellinen polku
    // ja tehd��n lopullisesti tutkittava polku.
    NFmiFileString fileString(filePath);
    std::string finalFilePath;
    if (fileString.IsAbsolutePath())
      finalFilePath = filePath;
    else
    {
      finalFilePath = theRootDirectory;
      addDirectorySeparatorAtEnd(finalFilePath);
      finalFilePath += filePath;
    }

    // Lis�t��n viel� tarvittaessa polkuun tiedoston wmr -p��te
    std::string fileExtension = fileString.Extension().CharPtr();
    if (fileExtension.empty())
    {
      finalFilePath += "." + theFileExtension;
      if (extensionAddedOut)
        *extensionAddedOut = true;
    }
    else
    {
      // Vaikka tiedostonimess� olisi extensio, se ei tarkoita ett� se olisi oikean tyyppinen (esim.
      // beta4.1, miss� '1' on v��r�n tyyppinen extensio)
      NFmiStringTools::LowerCase(fileExtension);
      std::string wantedFileExtensionLowerCase = theFileExtension;
      NFmiStringTools::LowerCase(wantedFileExtensionLowerCase);
      if (fileExtension != wantedFileExtensionLowerCase)
      {
        finalFilePath += "." + theFileExtension;
        if (extensionAddedOut)
          *extensionAddedOut = true;
      }
    }
    return simplifyWindowsPath(finalFilePath);
  }
}

// Cloud system based configurations try to use non-drive-letter absolute paths. But when you browse
// for file there is allways drive letter involved.  In those cases, remove drive letter from files
// absolute path, e.g.: C:\basepath\macros\macro.vmr   =>   \basepath\macros\macro.vmr
std::string doDriveLetterFix(const NFmiFileString &filePathString,
                             const NFmiFileString &baseDirectoryPathString)
{
  if (baseDirectoryPathString.Device() == "")
    return std::string(filePathString.Path() + filePathString.FileName());
  else
    return std::string(filePathString);
}

std::string GetRootName_CPP17Fix(const std::string &pathString)
{
#ifndef UNIX
  std::experimental::filesystem::path originalPath(fixedSeparatorPathString);
  // Return E.g. C: or D: on Windows
  return originalPath.root_name().string();
#else
  // Linux has no root names
  return "";
#endif
}

std::string GetRootDirectory_CPP17Fix(const std::string &pathString)
{
#ifndef UNIX
  std::experimental::filesystem::path originalPath(fixedSeparatorPathString);
  // Returns '\' on Windows
  return originalPath.root_directory().string();
#else
  // Linux
  return "/";
#endif
}

std::string GetRelativePath_CPP17Fix(const std::string &pathString)
{
#ifndef UNIX
  std::experimental::filesystem::path originalPath(fixedSeparatorPathString);
  // Returns '\' on Windows
  return originalPath.relative_path().string();
#else
  // Linux
  if(!pathString.empty() && pathString[0] == '/')
    return std::string(pathString.begin() + 1, pathString.end());
  return pathString;
#endif
}

std::string simplifyWindowsPath(const std::string &pathString)
{
  auto fixedSeparatorPathString = fixPathSeparators(pathString);
  auto rootName = GetRootName_CPP17Fix(fixedSeparatorPathString);
  auto rootDirectory = GetRootDirectory_CPP17Fix(fixedSeparatorPathString);
  auto relativePath = GetRelativePath_CPP17Fix(fixedSeparatorPathString);
  // Here comes absolute path without Windows drive letter, esim. \xxx\yyy
  std::string basicRootPathString = rootDirectory + relativePath;
  auto unixRootPathString = boost::replace_all_copy(basicRootPathString, "\\", "/");
  auto simplifiedUnixRootPathString = simplifyUnixPath(unixRootPathString);
  auto simplifiedFinalRootPathString =
      rootName + simplifiedUnixRootPathString;
#ifndef UNIX
simplifiedFinalRootPathString = 
       boost::replace_all_copy(simplifiedFinalRootPathString, "/", "\\");
#endif
  if (lastCharacterIsSeparator(pathString))
    PathUtils::addDirectorySeparatorAtEnd(simplifiedFinalRootPathString);
  return simplifiedFinalRootPathString;
}

std::string fixPathSeparators(const std::string &pathstring)
{
#ifdef UNIX
  return boost::replace_all_copy(pathstring, "\\", "/");
#else
  std::experimental::filesystem::path originalPath(pathstring);
  // K��nnet��n kaikki separaattorit oikein p�in
  originalPath = originalPath.make_preferred();
  // Poistetaan kaikki tupla tai useammat per�kk�iset separaattorit
  // Esim. dir1\\dir2\dir3\file => dir1\dir2\dir3\file
  // paitsi jos kyse on Winows polusta miss� on serveri nimi alussa
  // Esim. t�ss� ei alun tuplaa 'korjata': \\servername\dir1\dir2\file
  auto fixedPathString = originalPath.string();
  if (fixedPathString.size() < 2)
  {
    return fixedPathString;
  }

  std::string searchStr{kFmiDirectorySeparator, kFmiDirectorySeparator};
  std::string replaceStr{kFmiDirectorySeparator};
  bool isWinServerStart = (fixedPathString.front() == kFmiDirectorySeparator &&
                           fixedPathString[1] == kFmiDirectorySeparator);
  if (isWinServerStart)
  {
    // poistetaan 1. kirjain t�ss� poikkeustapauksessa
    fixedPathString.erase(0, 1);
  }

  for (;;)
  {
    // Tehd��n korjauksia niin kauan, kunnes tulos-stringin koko ei eroa originaalin koosta.
    auto originalStr = fixedPathString;
    boost::replace_all(fixedPathString, searchStr, replaceStr);
    if (originalStr.size() == fixedPathString.size())
    {
      break;
    }
  }

  if (isWinServerStart)
  {
    std::string finalStr;
    finalStr += kFmiDirectorySeparator;
    finalStr += fixedPathString;
    return finalStr;
  }
  else
    return fixedPathString;
#endif    
}

bool lastCharacterIsSeparator(const std::string &aPath)
{
  if (aPath.empty())
    return false;
  else
    return (aPath.back() == '\\' || aPath.back() == '/');
}

std::string getFixedAbsolutePathFromSettings(const std::string &theSettingsKey,
                                             const std::string &theAbsoluteWorkingPath,
                                             bool fEnsureEndDirectorySeparator)
{
  std::string settingPath = NFmiSettings::Require<std::string>(theSettingsKey);
  return makeFixedAbsolutePath(settingPath, theAbsoluteWorkingPath, fEnsureEndDirectorySeparator);
}

std::string makeFixedAbsolutePath(const std::string &thePath,
                                  const std::string &theAbsoluteWorkingPath,
                                  bool fEnsureEndDirectorySeparator)
{
  auto fixedPath = getAbsoluteFilePath(thePath, theAbsoluteWorkingPath);
  if (fEnsureEndDirectorySeparator)
    addDirectorySeparatorAtEnd(fixedPath);
  return fixedPath;
}

std::string getFilename(const std::string &filePath)
{
#ifndef UNIX
  std::experimental::filesystem::path originalPath(filePath);
  return originalPath.stem().string();
#else
  NFmiFileString fileString(filePath);
  return fileString.FileName().CharPtr(); 
#endif
}

}  // namespace PathUtils
