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


#endif // MODELS_H
