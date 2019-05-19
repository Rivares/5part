#include "models.h"
#include "initlayer.h"

#include <vector>
#include <thread>
#include <cstdint>
#include <iostream>

using std::vector;
using std::thread;
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

extern uint32_t spaceParametrBP;
extern uint32_t spaceParametrTP;
extern uint32_t spaceParametrACU;
extern uint32_t spaceParametrEVAP;

extern double dRC;    // Hieght nozzles
extern double dh;

extern double dt;
extern unsigned long long selectN;


//--------------------------LINER MODEL-----------------------------------
bool TMTPL_MM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    double  RvT = 0.0191806, RfT = 0.0000777,
            PTV = (0.05654433 * dt) / dh,
            PTF = (0.0002291314 * dt) / dh;

    if( (PTV > 0.5) || (PTF > 0.5) )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrBP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
       for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)        // place
       {
           TV.at(i).at(j) =
                   (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTV * TV.at(i-1).at(j-1))
                   + TV.at(i-1).at(j) * (-(dt * RvT) - PTV)
                   + TV.at(i-1).at(j);

           TF.at(i).at(j) =
                   (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTF * TF.at(i-1).at(j-1))
                   + TF.at(i-1).at(j) * (-PTF - (dt * RfT))
                   + TF.at(i-1).at(j);
       }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//------------------------NON-LINER MODEL---------------------------------
bool TMTPN_MM(vector <vector <double> > &TV, vector <vector <double> > &TF)
{
    //double a0 = 0.0001152735759 or 0.00016966, What is TRUE?????????

    // -----Model's heat exchenger parameters------
    double  RvT = 0.0191806, RfT = 0.0000777, a0 = 1.6966E-4,
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0002291314 * dt) / dh;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrBP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
       for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
       {
           PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

           TV.at(i).at(j) =
                   (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                   - (PTV_N * TV.at(i-1).at(j+1))
                   - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                   + (PTV_L * TV.at(i-1).at(j-1))
                   + TV.at(i-1).at(j);

           TF.at(i).at(j) =
                   (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                   + (PTF * TF.at(i-1).at(j-1))
                   - TF.at(i-1).at(j) * (dt*RfT + PTF)
                   + TF.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//----------------INTERCONNECTED MODEL(BOTTOM PART)-----------------------
bool ETMBP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double  RvT = 0.0191806, RfT = 0.0000777, a0 = 1.6966E-4,
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0002291314 * dt) / dh;

    // -----Model's mass exchenger parameters------
    double E = 1.0E-9, RvM = 0.004302, RfM = 1.222E-5;
                       //RfM = 0.000010734, RvM = 0.0216487318;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrBP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
        CV.push_back(bmp);
        CF.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrBP);
    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrBP);
    thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrBP);
    thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrBP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrBP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrBP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

            TV.at(i).at(j) =
                    (dt * RvT * TF.at(i-1).at((spaceParametrBP-1)-j))
                    - (PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TF.at(i).at(j) =
                    (dt * RfT * TV.at(i-1).at((spaceParametrBP-1)-j))
                    + (PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RfT + PTF)
                    + TF.at(i-1).at(j);

            // -----Calculate layer mass exchenger model------

            /* New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good*/
            /* 1 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L + PTV_N + 1)
                        - CV[i-1][j-1] * (PTV_L + PTV_N);*/

            /* 1 B+
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L - PTV_N + 1)
                        + CV[i-1][j-1] * (PTV_L + PTV_N);*/

            /* 2 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L - PTV_N + 1)
                        - CV[i-1][j-1] * PTV_L
                        + CV[i-1][j+1] * PTV_N;*/
            /* 2 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L + PTV_N + 1)
                        + CV[i-1][j-1] * PTV_L
                        - CV[i-1][j+1] * PTV_N;*/

            /* 3 A-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM - PTV_L + PTV_N + 1)
                        - CV[i-1][j-1] * PTV_N
                        + CV[i-1][j+1] * PTV_L;*/

            /* 3 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L - PTV_N + 1)
                        + CV[i-1][j-1] * PTV_N
                        - CV[i-1][j+1] * PTV_L;*/

            /* 4 A+*/
            CV.at(i).at(j) =
                    - (dt * RvM * E * CF.at(i-1).at((spaceParametrBP-1)-j))
                    + CV.at(i-1).at(j) * (dt*RvM - PTV_L - PTV_N + 1)
                    + CV.at(i-1).at(j+1) * (PTV_L + PTV_N);

            /* 4 B-
            CV[i][j] =  - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                        + CV[i-1][j] * (dt*RvM + PTV_L + PTV_N + 1)
                        - CV[i-1][j+1] * (PTV_L + PTV_N);*/

            /* 1 A-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E + PTF + 1)
                        - (PTF * CF[i-1][j-1]);*/

            /* 1 B+*/
            CF.at(i).at(j) =
                    - (dt * RfM * CV.at(i-1).at(j))
                    + CF.at(i-1).at(j) * (dt*RfM*E - PTF + 1)
                    + (PTF * CF.at(i-1).at(j-1));

            /* 2 A-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E - PTF + 1)
                        + (PTF * CF[i-1][j+1]);*/

            /* 2 B-
            CF[i][j] =  - (dt * RfM * CV[i-1][(selectZ-1)-j])
                        + CF[i-1][j] * (dt*RfM*E + PTF + 1)
                        - (PTF * CF[i-1][j+1]);*/
        }

    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrBP-1); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//-----------------INTERCONNECTED MODEL(TOP PART)-------------------------
bool ETMTP_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF)
{
    // -----Model's heat exchenger parameters------
    double  RvT = 2.500, RfT = 0.000085500, a0 = 0.0034868520, // (a0 = 0.0034868520) != (a0_Simulink = 0.002702752)
            PTV_L = (a0 * 273.15 * dt) / dh,
            PTV_N = 0.0,
            PTF = (0.0000400 * dt) / dh;

    // -----Model's mass exchenger parameters------
    double E = 1.0E-9, RvM = 0.1450, RfM = 6.0E-6;

    if( (PTV_L > 0.5) || (PTF > 0.5) )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrTP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TF.push_back(bmp);
        CV.push_back(bmp);
        CF.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrTP);
    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrTP);
    thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrTP);
    thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrTP);

    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrTP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            PTV_N = (a0 * TV.at(i-1).at(j) * dt) / dh;

            TV.at(i).at(j) =
                    (dt * RvT * TF.at(i-1).at((spaceParametrTP-1)-j))
                    - (PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RvT + PTV_L - PTV_N)
                    + (PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TF.at(i).at(j) =
                    (dt * RfT * TV.at(i-1).at((spaceParametrTP-1)-j))
                    + (PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RfT + PTF)
                    + TF.at(i-1).at(j);

            // -----Calculate layer mass exchenger model------

            /* New schema: CV(i,i-1); CV(i,i-1). Error was increase..., but dynamic of process good*
            CV[i][j] = - (dt * RvM * E * CF[i-1][(selectZ-1)-j])
                    + (PTV_L + PTV_N) * CV[i-1][j-1]
                    - CV[i-1][j] * (- dt*RvM + PTV_L + PTV_N)
                    + CV[i-1][j];*/

            CF.at(i).at(j) =
                    - (dt * RfM * CV.at(i-1).at((spaceParametrTP-1)-j))
                    + (PTF * CF.at(i-1).at(j-1))
                    - CF.at(i-1).at(j) * (-(dt*RfM*E) + PTF)
                    + CF.at(i-1).at(j);

            /* Old schema: CV(i,i-1); CV(i+1,i)*/

            CV.at(i).at(j) =
                    - (dt * RvM * E * CF.at(i-1).at((spaceParametrTP-1)-j))
                    - CV.at(i-1).at(j) * (PTV_L - PTV_N + dt*RvM)
                    + (PTV_L * CV.at(i-1).at(j-1))
                    - (PTV_N * CV.at(i-1).at(j-1))
                    + CV.at(i-1).at(j);

        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrTP-1); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

//------------------AIR-COOLING UNIT MODEL(ACU)---------------------------
bool ACU_MM(vector<vector<double> > &TV, vector<vector<double> > &TB)
{
    // -----Model's heat parameters------
    double  RvT = 8.400, a0 = 0.06,     // a0 = 0.06 ?
            PTV_L = (a0 * 273.150) / dh;

    // -----Model's boarder parameters------
    double RB1 = 0.00661, RB2 = 0.480
         , TE = 12.380                  // Temperature of enviroment
         , CP = 134.0;                  // Count piplines;

    if((dt * PTV_L) > 0.5 )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrACU, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TB.erase(TB.begin(), TB.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(bmp);
        TB.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrACU);
    thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrACU);

    threadInitialLayerTV.join();
    threadInitialLayerTB.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrACU; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrACU; ++j)
    {
        cout << TB.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(size_t j = 1; j < (spaceParametrACU-1); ++j)
        {
            // -----Calculate layer heat exchenger model------
            TV.at(i).at(j) =
                    (dt * RvT * TB.at(i-1).at((spaceParametrACU-1)-j))
                    - TV.at(i-1).at(j) * ((dt*RvT) + (dt*PTV_L))
                    + (dt * PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TB.at(i).at(j) =
                    (dt * RB2 * TV.at(i-1).at(j))              // TV[i-1][j] ??? j - ????
                    + (CP*dt*RB1*TE)                        // ? TE - const or function?
                    - TB.at(i-1).at(j) * (CP*dt*RB1 + dt*RB2)
                    + TB.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}

bool EVAP_MM(vector <vector <double> > &TF, vector <vector <double> > &TB, vector <vector <double> > &TFG)
{
    // -----Model's gas parameters------
    double  RFG = 0.470, PTFG = 4.60 / dh;

    // -----Model's fluide parameters------
    double  RF = 0.0030, PTF = 0.0030 / dh;

    // -----Model's boarder parameters------
    double  RFB = 0.220, RFGB = 0.0080;

    if((dt * PTFG) > 0.5 )
    {
        cout << "The scheme diverges!!!";
        return false;
    }

    vector <double> bmp;
    bmp.assign(spaceParametrEVAP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.erase(TF.begin(), TF.end());
        TB.erase(TB.begin(), TB.end());
        TFG.erase(TFG.begin(), TFG.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TF.push_back(bmp);
        TB.push_back(bmp);
        TFG.push_back(bmp);
    }

    beginPoint = 0;

    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrEVAP);
    thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrEVAP);
    thread threadInitialLayerTFG(initialLayerTFG, std::ref(TFG));

    threadInitialLayerTF.join();
    threadInitialLayerTB.join();
    threadInitialLayerTFG.join();

    cout << endl << "Initial values:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TB.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrEVAP; ++j)
    {
        cout << TFG.at(0).at(j) << " | ";
    }   cout << endl;

    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)
    {
        for(size_t j = 1; j < (spaceParametrEVAP-1); ++j)
        {
            TF.at(i).at(j) =
                    (dt * RF * TB.at(i-1).at(j+1)) // ?
                    + (dt * PTF * TF.at(i-1).at(j+1))
                    - TF.at(i-1).at(j) * (dt*RF + dt*PTF)
                    + TF.at(i-1).at(j);

            TB.at(i).at(j) =
                    (dt * RFB * TF.at(i-1).at(j)) // ?
                    + (dt * RFGB * TFG.at(i-1).at((spaceParametrEVAP-1)-j))// ?
                    - TB.at(i-1).at(j) * (dt*RFGB + dt*RFB)
                    + TB.at(i-1).at(j);

            TFG.at(i).at(j) =
                    (dt * RFG * TB.at(i-1).at(j+1)) // ?
                    + (dt * PTFG * TFG.at(i-1).at(j-1))
                    - TFG.at(i-1).at(j) * (dt*RFG + dt*PTFG)
                    + TFG.at(i-1).at(j);
        }
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 1; j < (spaceParametrEVAP-1); ++j)
    {
        cout << TFG.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;


    return true;
}

bool TOP_ACU_MM(vector <vector <double> > &TV, vector <vector <double> > &TF,
             vector <vector <double> > &CV, vector <vector <double> > &CF,
             vector<vector<double> > &TB)
{
    // RC_TOP:
    // -----Model's heat exchenger parameters------
    double  RC_TOP_RvT = 2.500, RC_TOP_RfT = 0.000085500, RC_TOP_a0 = 0.0034868520 // (a0 = 0.0034868520) != (a0_Simulink = 0.002702752)
          , RC_TOP_PTV_L = (RC_TOP_a0 * 273.15 * dt) / 1.17 // dh
          , RC_TOP_PTV_N = 0.0
          , RC_TOP_PTF = (0.0000400 * dt) / 1.17; // dh

    // -----Model's mass exchenger parameters------
    double RC_TOP_E = 1.0E-9, RC_TOP_RvM = 0.1450, RC_TOP_RfM = 6.0E-6;

    if( (RC_TOP_PTV_L > 0.5) || (RC_TOP_PTF > 0.5) )
    {
        cout << "The scheme RC_TOP diverges!!!";
        return false;
    }

    //--------------------------------------------------------

    // ACU:
    // -----Model's heat parameters------
    double  ACU_RvT = 8.400, ACU_a0 = 0.06     // a0 = 0.06 ?
          , ACU_PTV_L = (ACU_a0 * 273.150) / 1.333; // dh

    // -----Model's boarder parameters------
    double ACU_RB1 = 0.00661, ACU_RB2 = 0.480
         , ACU_TE = 12.380                  // Temperature of enviroment
         , ACU_CP = 134.0;                  // Count piplines;

    if((dt * ACU_PTV_L) > 0.5 )
    {
        cout << "The scheme ACU diverges!!!";
        return false;
    }

    //--------------------------------------------------------

    vector <double> countSpacePoints_1, countSpacePoints_2;
    countSpacePoints_1.assign(spaceParametrTP + spaceParametrACU - 2, 0.0);
    countSpacePoints_2.assign(spaceParametrTP, 0.0);

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.erase(TV.begin(), TV.end());
        TF.erase(TF.begin(), TF.end());
        CV.erase(CV.begin(), CV.end());
        CF.erase(CF.begin(), CF.end());

        TB.erase(TB.begin(), TB.end());
    }

    for (unsigned long long i = 0; i < static_cast <size_t>(selectN / dt); ++i)
    {
        TV.push_back(countSpacePoints_1);
        TF.push_back(countSpacePoints_2);
        CV.push_back(countSpacePoints_2);
        CF.push_back(countSpacePoints_2);

        TB.push_back(countSpacePoints_2);
    }

    //--------------------------------------------------------

    beginPoint = 0;

    thread threadInitialLayerTV(initialLayerTV, std::ref(TV), spaceParametrTP + spaceParametrACU - 2);
    thread threadInitialLayerTF(initialLayerTF, std::ref(TF), spaceParametrTP);
    thread threadInitialLayerCV(initialLayerCV, std::ref(CV), spaceParametrTP);
    thread threadInitialLayerCF(initialLayerCF, std::ref(CF), spaceParametrTP);

    thread threadInitialLayerTB(initialLayerTB, std::ref(TB), spaceParametrACU);


    threadInitialLayerTV.join();
    threadInitialLayerTF.join();
    threadInitialLayerCV.join();
    threadInitialLayerCF.join();

    threadInitialLayerTB.join();


    cout << endl << "Initial values RC_TOP:" << endl;
    std::cout.precision(8);

    for(size_t j = 0; j < spaceParametrTP + spaceParametrACU - 2; ++j)
    {
        cout << TV.at(0).at(j) << std::fixed << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << TF.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CV.at(0).at(j) << " | ";
    }   cout << endl;

    for(size_t j = 0; j < spaceParametrTP; ++j)
    {
        cout << CF.at(0).at(j) << " | ";
    }   cout << endl;


    beginPoint = spaceParametrTP - 1;
    endPoint = spaceParametrTP + spaceParametrACU - 2;
    // Calculate model
    for(size_t i = 1; i < static_cast <size_t>(selectN / dt); ++i)// time
    {
        for(size_t j = 1; j < (spaceParametrTP-1); ++j)      // place
        {
            // -----Calculate layer heat exchenger model------
            RC_TOP_PTV_N = (RC_TOP_a0 * TV.at(i-1).at(j) * dt) / 1.17; //dh;

            TV.at(i).at(j) =
                    (dt * RC_TOP_RvT * TF.at(i-1).at((spaceParametrTP-1)-j))
                    - (RC_TOP_PTV_N * TV.at(i-1).at(j+1))
                    - TV.at(i-1).at(j) * (dt*RC_TOP_RvT + RC_TOP_PTV_L - RC_TOP_PTV_N)
                    + (RC_TOP_PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TF.at(i).at(j) =
                    (dt * RC_TOP_RfT * TV.at(i-1).at((spaceParametrTP-1)-j))
                    + (RC_TOP_PTF * TF.at(i-1).at(j-1))
                    - TF.at(i-1).at(j) * (dt*RC_TOP_RfT + RC_TOP_PTF)
                    + TF.at(i-1).at(j);

            CF.at(i).at(j) =
                    - (dt * RC_TOP_RfM * CV.at(i-1).at((spaceParametrTP-1)-j))
                    + (RC_TOP_PTF * CF.at(i-1).at(j-1))
                    - CF.at(i-1).at(j) * (-(dt*RC_TOP_RfM*RC_TOP_E) + RC_TOP_PTF)
                    + CF.at(i-1).at(j);


            CV.at(i).at(j) =
                    - CV.at(i-1).at(j) * (RC_TOP_PTV_L - 1 - RC_TOP_PTV_N + dt*RC_TOP_RvM)
                    + (RC_TOP_PTV_L * CV.at(i-1).at(j-1))
                    - (RC_TOP_PTV_N * CV.at(i-1).at(j-1))
                    - (dt * RC_TOP_RvM * RC_TOP_E * CF.at(i-1).at((spaceParametrTP-1)-j));
        }

        for(size_t j = beginPoint; j < endPoint - 1; ++j)
        {
            // -----Calculate layer heat exchenger model------
            TV.at(i).at(j) =
                    (dt*ACU_RvT * TB.at(i-1).at((endPoint - 1) - j))
                    - TV.at(i-1).at(j) * ((dt*ACU_RvT) + (dt*ACU_PTV_L))
                    + (dt * ACU_PTV_L * TV.at(i-1).at(j-1))
                    + TV.at(i-1).at(j);

            TB.at(i).at(j-beginPoint + 1) =
                    (dt * ACU_RB2 * TV.at(i-1).at(endPoint + 2 - j))
                    + (ACU_CP*dt*ACU_RB1*ACU_TE)
                    - TB.at(i-1).at(j-beginPoint) * (ACU_CP*dt*ACU_RB1 + dt*ACU_RB2)
                    + TB.at(i-1).at(j-beginPoint);
        }

        TF.at(i).at(0) = TV.at(i).at(endPoint-2);   // Connection between ACU(TV) -> RC_TOP(TF)
    }

    cout << endl << "Steady-state values:" << endl;
    for(uint32_t j = 0; j < (endPoint); ++j)
    {
        cout << TV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 0; j < (spaceParametrTP); ++j)
    {
        cout << TF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 0; j < (spaceParametrTP); ++j)
    {
        cout << CV.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    for(uint32_t j = 0; j < (spaceParametrTP); ++j)
    {
        cout << CF.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;


    cout << endl << "Steady-state values TB:" << endl;
    for(uint32_t j = 1; j < (spaceParametrACU-1); ++j)
    {
        cout << TB.at(static_cast <size_t>((selectN-2) / dt)).at(j) << " | ";
    }cout << endl;

    return true;
}
