#ifndef MODELS_H
#define MODELS_H

#include <vector>

using std::vector;

bool TMTPL_MM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool TMTPN_MM(vector <vector <double> > &TV, vector <vector <double> > &TF);
bool ETMBP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
              vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ETMTP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
              vector <vector <double> > &CV, vector <vector <double> > &CF);
bool ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TB);
bool EVAP_MM(vector <vector <double> > &TF, vector <vector <double> > &TB,
             vector <vector <double> > &TFG);

bool TOP_ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                vector <vector <double> > &CV, vector <vector <double> > &CF,
                vector<vector<double> > &TB);

bool TOP_BOT_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                vector <vector <double> > &CV, vector <vector <double> > &CF);

bool BOT_EVAP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
                 vector <vector <double> > &CV, vector <vector <double> > &CF,
                 vector <vector <double> > &TB, vector <vector <double> > &TFG);

bool FULL_RC_MM(vector <vector <double> > &ACU_TB,
                vector <vector <double> > &BOT_TOP_ACU_TV,
                vector <vector <double> > &TOP_TF,
                vector <vector <double> > &BOT_EVAP_TF,
                vector <vector <double> > &BOT_CV,
                vector <vector <double> > &BOT_CF,
                vector <vector <double> > &TOP_CV,
                vector <vector <double> > &TOP_CF,
                vector <vector <double> > &EVAP_TB,
                vector <vector <double> > &EVAP_TFG);

#endif // MODELS_H
