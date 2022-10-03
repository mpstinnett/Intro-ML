#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>


using namespace std;
using namespace std::chrono;

void accuracy(double TP, double FP, double TN, double FN)
{
    cout << "accuracy: " << ((TP + TN) / (TP + TN + FP + FN)) << endl;
}

void sensitivity(double TP, double FP, double TN, double FN)
{
    cout << "sensitivity: " << (TP / (TP + FN)) << endl;
}

void specificity(double TP, double FP, double TN, double FN)
{
    cout << "sensitivity: " << (TN / (TN + FP)) << endl;
}

int main()
{
    ifstream inFS;
    string line;
    string pc, sv, sx, ag, nt;
    vector<double> entry;
    vector<double> pclass;
    vector<double> survived;
    vector<double> sex;
    vector<double> age;

    // credit to cpluplus.com for setting up idx, second parameter of stof
    string::size_type sz;

    cout << "Opening file titanic_project.csv" << endl;

    inFS.open("titanic_project.csv");
    if(!inFS.is_open())
    {
        cout << "Could not open file titanic_project.csv." << endl;
        return 1;
    }
    getline(inFS, line);
    cout << "heading: " << line << endl;

    while(inFS.good())
    {
        getline(inFS, nt, ',');
        getline(inFS, pc, ',');
        getline(inFS, sv, ',');
        getline(inFS, sx, ',');
        getline(inFS, ag, '\n');

        entry.push_back(stof(pc, &sz));
        pclass.push_back(stof(pc, &sz));
        survived.push_back(stof(sv, &sz));
        sex.push_back(stof(sx, &sz));
        age.push_back(stof(ag, &sz));
    }

    cout << "Closing file titanic_project.csv." << endl;
    inFS.close();

    double B[] = {0, 0};
    double predicted = 0;
    double learningrate = 0.01;
    double weight = 0;
    double loss = 0;
    double dB0 = 0;
    double dB1 = 0;

    auto start = high_resolution_clock::now();

    for(double i = 1; i <= 800; i++)
    {
        weight = B[0] * sex.at(i - 1) + B[1];
        predicted = (1 / pow(2.71828 , ((-1) * weight)));

        // loss = survived.at(i - 1) * log(predicted) + (1 - survived.at(i - 1)) * log(1 - predicted);

        for(int j = 0; j < 800; j++)
        {        
            dB0 += (predicted - survived.at(j)) * sex.at(j);
            dB1 += (predicted - survived.at(j));
        }
        
        dB0 = (1.0 / 800.0) * dB0;
        dB1 = (1.0 / 800.0) * dB1;

        B[0] = B[0] - learningrate * dB0;
        B[1] = B[1] - learningrate * dB1;
    }
    
    cout << "B0: " << B[0] << endl;
    cout << "B1: " << B[1] << endl;

    int TP = 0;
    int FP = 0;
    int FN = 0;
    int TN = 0;


    for(int i = 801; i < survived.size(); i++)
    {
        double p = ( 1 / exp((-1) * B[0] * (sex.at(i) + B[1])) );
        if(p > 0.5)
        {
            if(survived.at(i) == 1)
            {
                TP++;
            }
            else
            {
                FP++;
            }
        }
        else
        {
            if(survived.at(i) == 0)
            {
                TN++;
            }
            else
            {
                FN++;
            }
        }
    }
    
    cout << "TP: " << TP << endl;
    cout << "FP: " << FP << endl;
    cout << "TN: " << TN << endl;
    cout << "FN: " << FN << endl;

    accuracy(TP, FP, TN, FN);
    sensitivity(TP, FP, TN, FN);
    specificity(TP, FP, TN, FN);

    auto stop = high_resolution_clock::now();
    auto time = duration_cast<milliseconds>(stop - start);
    cout << "Time to Run: " << time.count() << " milliseconds";
}

