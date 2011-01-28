
/*!  
 * T�m� on Editorin k�ytt�m� sana kirja funktio. 
 * Kieli versiot stringeihin tulevat t��lt�.
 */

#include "NFmiDictionaryFunction.h"

// HUOM! T�m� on kopio NFmiEditMapGeneralDataDoc-luokan metodista, kun en voinut antaa t�nne dokumenttia
std::string GetDictionaryString(const char *theMagicWord)
{
	const std::string baseWords = "MetEditor::Dictionary::";

	std::string finalMagicWord(baseWords);
	finalMagicWord += theMagicWord;
	return NFmiSettings::Optional<std::string>(finalMagicWord.c_str(), std::string(theMagicWord));
}

