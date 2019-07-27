#include "initlayer.h"

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

using std::ofstream;
using std::ios_base;
using std::vector;
using std::string;
using std::cout;
using std::endl;

extern uint32_t beginPoint;
extern uint32_t endPoint;

extern vector <double> initLayerTV;
extern vector <double> initLayerTF;
extern vector <double> initLayerCV;
extern vector <double> initLayerCF;
extern vector <double> initLayerTB;
extern vector <double> initLayerTFG;

//---------------Borders--------------//
extern vector <double> initLayerTV_0, initLayerTV_1;
extern vector <double>  initLayerTF_0, initLayerTF_1;
extern vector <double>  initLayerCV_0, initLayerCV_1;
extern vector <double>  initLayerCF_0, initLayerCF_1;
extern double initLayerTB_0, initLayerTB_1;
extern double initLayerTFG_0, initLayerTFG_1;

extern uint32_t spaceParametrBP;
extern uint32_t spaceParametrTP;
extern uint32_t spaceParametrACU;
extern uint32_t spaceParametrEVAP;

extern double dt;
extern unsigned long long selectN;


void initialLayerTV(vector <vector <double> > &TV, unsigned int it)
{
    size_t i = 0, j = 0, k = 1;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.at(i).at(beginPoint) = initLayerTV_0.at(0);
        TV.at(i).at(it-1) = initLayerTV_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        TV.at(0).at(j) = initLayerTV.at(j-k);
    }
}

void initialLayerTF(vector <vector <double> > &TF, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.at(i).at(beginPoint) = initLayerTF_0.at(0);
        TF.at(i).at(it-1) = initLayerTF_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        TF.at(0).at(j) = initLayerTF.at(j-1);
    }
}

void initialLayerCV(vector <vector <double> > &CV, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CV.at(i).at(beginPoint) = initLayerCV_0.at(0);
        CV.at(i).at(it-1) = initLayerCV_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        CV.at(0).at(j) = initLayerCV.at(j-1);
    }
}

void initialLayerCF(vector <vector <double> > &CF, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        CF.at(i).at(beginPoint) = initLayerCF_0.at(0);
        CF.at(i).at(it-1) = initLayerCF_1.at(0);
    }

    // Initial values
    for(j = beginPoint + 1; j <= (it-2); ++j)
    {
        CF.at(0).at(j) = initLayerCF.at(j-1);
    }
}

void initialLayerTB(vector <vector <double> > &TB, unsigned int it)
{
    size_t i = 0, j = 0;

    // Borders
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TB.at(i).at(0) = initLayerTB_0;
        TB.at(i).at(it-1) = initLayerTB_1;
    }

    // Initial values
    for(j = 1; j <= (it-2); ++j)
    {
        TB.at(0).at(j) = initLayerTB.at(j-1);
    }
}

void initialLayerTFG(vector <vector <double> > &TFG)
{
    size_t i = 0, j = 0;

    // Bordersuint32_t
    for(i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TFG.at(i).at(0) = initLayerTFG_0;
        TFG.at(i).at(spaceParametrEVAP-1) = initLayerTFG_1;
    }

    // Initial values
    for(j = 1; j <= (spaceParametrEVAP-2); ++j)
    {
        TFG.at(0).at(j) = initLayerTFG.at(j-1);
    }
}

//?????----Why it here?????????
void toFileMM(vector <vector <double> > MMM, string nameModel)
{
    uint32_t sizeModel = 0;

    // In future - RegExp
    if(nameModel == "TP_ACU_TV")
    {
        sizeModel = spaceParametrTP + spaceParametrACU - 2;
    }
    else
    {
        sizeModel = spaceParametrTP - 1;
    }

    if( (nameModel == "TV_ACU") || (nameModel == "TB_ACU") )
    {
        sizeModel = spaceParametrACU - 1;
    }

    nameModel = "MM_" + nameModel + ".dat";

    ofstream foutMM(nameModel, ios_base::out | ios_base::trunc);

    for(size_t i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(uint32_t j = 1; j < (sizeModel); j++)
        {
            foutMM << MMM.at(i).at(j) << " ";
        }
        foutMM << endl;
    }
    foutMM.close();
}
