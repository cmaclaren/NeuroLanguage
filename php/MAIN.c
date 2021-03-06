#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <locale.h>
#include <time.h>
#include "train.h"
#include "test.h"
#include "computations.h"

void cleanText(wchar_t *itemMemory, wchar_t *alphMemory, wchar_t *unknown, int imSize);

int main(int argc, char** argv) {
    //Setting locale(for utf-8 purposes)
    if (!setlocale(LC_ALL, "en_US.utf8")) {
      printf( "Can't set the specified locale! "
              "Check LANG, LC_CTYPE, LC_ALL.\n");
      return -1;
    }

    //For randomizing
    srand(time(0));


/*************************************************/
    int D = atoi(argv[1]);
    int N = 3;
/*************************************************/


    //For training & testing
    char langLabels[][4] = {"afr", "bul", "ces", "dan", "deu", "nld", "ell", "eng", "est", "fin", "fra", "hun", "isl", "ita", "lat", "lit", "nor", "pol", "por", "ron", "rus", "slk", "slv", "spa", "swe", "tuk", "ukr"};
	char actualLabels[][11] = {"Afrikaanas", "Bulgarian", "Czech", "Danish", "German", "Dutch", "Greek", "English", "Estonian", "Finish", "French", "Hungarian", 
        "Icelandic", "Italian", "Latvian", "Lithuanian", "Norwegian", "Polish", "Portugese", "Romanian", "Russian", "Slovak", "Slovenian", "Spanish", 
        "Swedish", "Turkish", "Ukranian"};
    int length =  (sizeof langLabels)/(sizeof langLabels[0]); //size of langLabels


/******************************************************/
/*                  Item Memory                      */
    wchar_t *itemMemory = L" abcdefghijklmnopqrstuvwxyzăãāàáąåäâæßçčćďðéėēèêěëęģğîïíìįīķłļľñńňŉņōôóòõőøöœþšșśşťřŕúűūůųùûüÿýỳžżабвгґдеёжзэєїіийклмнопрстуфхцчшщъьыюяαβγδέεζήηθίικλμνξόοπρστυφχψώω";
    wchar_t *alphMemory = L" ABCDEFGHIJKLMNOPQRSTUVWXYZĂÃĀÀÁĄÅÄÂÆßÇČĆĎÐÉĖĒÈÊĚËĘĢĞÎÏÍÌĮĪĶŁĻĽÑŃŇŉŅŌÔÓÒÕŐØÖŒÞŠȘŚŞŤŘŔÚŰŪŮŲÙÛÜŸÝỲŽŻАБВГҐДЕЁЖЗЭЄЇІИЙКЛМНОПРСТУФХЦЧШЩЪЬЫЮЯΑΒΓΔΈΕΖΉΗΘΊΙΚΛΜΝΞΌΟΠΡΣΤΥΦΧΨΏΩ";
    int imSize= wcslen(itemMemory);
/*****************************************************/


/*****************************************************/
/*Conversion of argv[2] to be compatable for testing*/
    const char *str = argv[2];
    int maxSize = 10000;
    wchar_t *unknown = (wchar_t *)malloc(maxSize * sizeof(wchar_t));
    mbstowcs(unknown, str, maxSize);
    cleanText(itemMemory, alphMemory, unknown, imSize);
/*****************************************************/

    //Variable Declaration  
    int *langAM = (int*)malloc(length * D * sizeof(int));
    int *iMHV = (int*)malloc(imSize * D * sizeof(int));

    train(N, D, itemMemory, imSize, iMHV, langLabels, length);
    test(N, D, langAM, iMHV, itemMemory, imSize, actualLabels, langLabels, length, unknown);

    free(unknown);
    free(iMHV);
    free(langAM);
    return 0;
}

void cleanText(wchar_t *itemMemory, wchar_t *alphMemory, wchar_t *unknown, int imSize) {
    int unknownSize= wcslen(unknown);
    int check=0;
    for(int i=0; i<unknownSize; i++) {
        if (unknown[i] == L'-') {
            unknown[i] = ' ';
        }
        else if(unknown[i] == L'—') {
            unknown[i] = ' ';
        }
        else if(unknown[i] == L'–') {
            unknown[i] = ' ';
        }
        else if(unknown[i] == L'/') {
            unknown[i] = ' ';
        }     
        for(int j=0; j<imSize; j++) {
            if(unknown[i] == alphMemory[j]) {
                unknown[i] = itemMemory[j];
                break;
            }
        }
    }

    for(int i=0; i<unknownSize; i++) {
        for(int j=0; j<imSize; j++) {
            if(unknown[i] == itemMemory[j]) {
                check=0;
                break;
            }
            else {
                check=1;
            }
        }

        if(check==1) {
            wmemmove(&unknown[i], &unknown[i+1], unknownSize-i);
        }
    }   
}
