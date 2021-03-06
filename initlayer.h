#ifndef INITLAYER_H
#define INITLAYER_H

#include <vector>
#include <string>

using std::vector;
using std::string;

void toFileMM(vector <vector <double> > MMM, string nameModel);

void initialLayerTV(vector <vector <double> > &TV, unsigned int it);         // Would be relize as template
void initialLayerTF(vector <vector <double> > &TF, unsigned int it);
void initialLayerCV(vector <vector <double> > &CV, unsigned int it);
void initialLayerCF(vector <vector <double> > &CF, unsigned int it);
void initialLayerTB_ACU(vector <vector <double> > &TB_ACU);
void initialLayerTB_EVAP(vector <vector <double> > &TB_EVAP);
void initialLayerTFG(vector <vector <double> > &TFG);

#endif // INITLAYER_H
