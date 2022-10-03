#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
using namespace std;

double Gaussian(double v, double mean, double variance)
{
    return ((1 / sqrt(2 * M_PI * variance)) * exp(-pow(v - mean, 2) / (2 * variance)));
}

vector<double> RawProbability(int pclass, int sex, double age,
                              vector<vector<double>> lh_pclass, vector<vector<double>> lh_sex, vector<double> apriori,
                              vector<double> age_mean, vector<double> age_variance)
{
    pclass = pclass - 1;
    vector<double> result{0, 0};
    double num_survive = lh_pclass[1][pclass] * lh_sex[1][sex] * apriori[1] * Gaussian(age, age_mean[1], age_variance[1]) ;
    double num_die = lh_pclass[0][pclass] * lh_sex[0][sex] * apriori[0] * Gaussian(age, age_mean[0], age_variance[0]) ;
    double denominator = num_survive + num_die;
    result[0] = num_die/denominator;
    result[1] = num_survive/denominator;
    return result;
}


int main(int argc, char **argv)
{
    ifstream inFS; // input file stream
    string line;
    string pclass_in, survived_in, sex_in, age_in, blank;
    const int MAX_LEN = 1500;
    const int TRAINING_CUTOFF = 800;
    vector<int> pclass(MAX_LEN);
    vector<int> survived(MAX_LEN);
    vector<int> sex(MAX_LEN);
    vector<double> age(MAX_LEN);

    // Try to open file
    cout << "Opening file titanic_project.csv." << endl;

    inFS.open("titanic_project.csv");
    if (!inFS.is_open())
    {
        cout << "Could not open file titanic_project.csv." << endl;
        return 1; // 1 indicated error
    }

    // Can now use inFS stream like cin stream
    getline(inFS, line);

    int numObservations = 0;
    while (inFS.good())
    {

        getline(inFS, blank, ',');
        getline(inFS, pclass_in, ',');
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, ',');
        getline(inFS, age_in, '\n');

        pclass.at(numObservations) = stof(pclass_in);
        survived.at(numObservations) = stof(survived_in);
        sex.at(numObservations) = stof(sex_in);
        age.at(numObservations) = stof(age_in);

        numObservations++;
    }

    pclass.resize(numObservations);
    survived.resize(numObservations);
    sex.resize(numObservations);
    age.resize(numObservations);

    cout << "Closing file titanic_project.csv." << endl;
    inFS.close(); // Done with file, so close it

    // Start Training Clock
    auto start = std::chrono::high_resolution_clock::now();

    // Calculate prior probabilities
    vector<double> apriori{0, 0};
    // Get the counts for survived and died passengers
    double survivedCount;
    double diedCount;
    for (size_t i = 0; i < TRAINING_CUTOFF; i++)
    {
        if (survived[i] == 1)
        {
            survivedCount++;
        }
    }
    diedCount = TRAINING_CUTOFF - survivedCount;
    apriori[0] = diedCount / TRAINING_CUTOFF;
    apriori[1] = survivedCount / TRAINING_CUTOFF;

    // Calculate likelihood for discrete data
    vector<vector<int>> sex_vs_survived_count{{0, 0},
                                              {0, 0}};
    vector<vector<int>> pclass_vs_survived_count{{0, 0, 0},
                                                 {0, 0, 0}};
    // Run through the training data once and store the counts
    for (size_t i = 0; i < TRAINING_CUTOFF; i++)
    {
        if (survived[i] == 0 && sex[i] == 0)
        {
            sex_vs_survived_count[0][0]++;
        }
        if (survived[i] == 1 && sex[i] == 0)
        {
            sex_vs_survived_count[1][0]++;
        }
        if (survived[i] == 0 && sex[i] == 1)
        {
            sex_vs_survived_count[0][1]++;
        }
        if (survived[i] == 1 && sex[i] == 1)
        {
            sex_vs_survived_count[1][1]++;
        }
        if (survived[i] == 0 && pclass[i] == 1)
        {
            pclass_vs_survived_count[0][0]++;
        }
        if (survived[i] == 1 && pclass[i] == 1)
        {
            pclass_vs_survived_count[1][0]++;
        }
        if (survived[i] == 0 && pclass[i] == 2)
        {
            pclass_vs_survived_count[0][1]++;
        }
        if (survived[i] == 1 && pclass[i] == 2)
        {
            pclass_vs_survived_count[1][1]++;
        }
        if (survived[i] == 0 && pclass[i] == 3)
        {
            pclass_vs_survived_count[0][2]++;
        }
        if (survived[i] == 1 && pclass[i] == 3)
        {
            pclass_vs_survived_count[1][2]++;
        }
    }

    // Get the actual likelihoods by dividing the counts by the total counts for their respective category
    vector<vector<double>> lh_sex{{0, 0},
                                  {0, 0}};
    vector<vector<double>> lh_pclass{{0, 0, 0},
                                     {0, 0, 0}};

    lh_sex[0][0] = sex_vs_survived_count[0][0] / diedCount;
    lh_sex[0][1] = sex_vs_survived_count[0][1] / diedCount;
    lh_sex[1][0] = sex_vs_survived_count[1][0] / survivedCount;
    lh_sex[1][1] = sex_vs_survived_count[1][1] / survivedCount;

    lh_pclass[0][0] = pclass_vs_survived_count[0][0] / diedCount;
    lh_pclass[0][1] = pclass_vs_survived_count[0][1] / diedCount;
    lh_pclass[0][2] = pclass_vs_survived_count[0][2] / diedCount;
    lh_pclass[1][0] = pclass_vs_survived_count[1][0] / survivedCount;
    lh_pclass[1][1] = pclass_vs_survived_count[1][1] / survivedCount;
    lh_pclass[1][2] = pclass_vs_survived_count[1][2] / survivedCount;

    // Calculate likelihood for Continuous Data
    // Get the sums
    double age_survived_sum = 0;
    double age_died_sum = 0;
    vector<double> age_mean{0, 0};
    vector<double> age_variance{0, 0};

    for (size_t i = 0; i < TRAINING_CUTOFF + 1; i++)
    {
        if (survived[i] == 1)
        {
            age_survived_sum += age[i];
        }
        else
        {
            age_died_sum += age[i];
        }
    }
    // Get the means
    age_mean[1] = age_survived_sum / survivedCount;
    age_mean[0] = age_died_sum / diedCount;

    // Get the variances
    for (size_t i = 0; i < TRAINING_CUTOFF; i++)
    {
        if (survived[i] == 1)
        {
            age_variance[1] += pow(age[i] - age_mean[1], 2);
        }
        else
        {
            age_variance[0] += pow(age[i] - age_mean[0], 2);
        }
    }

    age_variance[1] = age_variance[1] / (survivedCount - 1);
    age_variance[0] = age_variance[0] / (diedCount - 1);

    // Stop Training Clock
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    // Apply model on Test data and collect metrics
    double TP = 0;
    double TN = 0;
    double FP = 0;
    double FN = 0;
    for (size_t i = TRAINING_CUTOFF; i < numObservations; i++)
    {
        vector<double> probs = RawProbability(pclass[i], sex[i], age[i], lh_pclass, lh_sex, apriori, age_mean, age_variance);
        if (probs[0] > probs[1] && survived[i] == 0)
        {
            TP++;
        }
        if (probs[1] > probs[0] && survived[i] == 1)
        {
            TN++;
        }
        if (probs[0] > probs[1] && survived[i] == 1)
        {
            FP++;
        }
        if (probs[1] > probs[0] && survived[i] == 0)
        {
            FN++;
        }
    }
    double accuracy = (TP+TN)/(TP+TN+FP+FN);
    double sensitivity = (TP)/(TP+FN);
    double specificity = (TN)/(TN+FP);

    // Print metrics and model information
    cout << endl;
    cout << "Time to train: " << duration.count() << " microseconds" << endl;
    cout << endl;
    cout << "PRIOR PROBABILITIES" << endl;
    cout << "Survived: " << apriori[1] << " Died: " << apriori[0] << endl;
    cout << endl;
    cout << "LIKELIHOODS" << endl;
    cout << "pclass" << endl;
    cout << "pclass1 -> Died: " << lh_pclass[0][0] << " Survived: " << lh_pclass[1][0] << endl;
    cout << "pclass2 -> Died: " << lh_pclass[0][1] << " Survived: " << lh_pclass[1][1] << endl;
    cout << "pclass3 -> Died: " << lh_pclass[0][2] << " Survived: " << lh_pclass[1][2] << endl;
    cout << "Sex" << endl;
    cout << "female -> Died: " << lh_sex[0][0] << " Survived: " << lh_sex[1][0] << endl;
    cout << "male -> Died: " << lh_sex[0][1] << " Survived: " << lh_sex[1][1] << endl;
    cout << "Age" << endl;
    cout << "Mean -> Died: " << age_mean[0] << " Survived: " << age_mean[1] << endl;
    cout << "Standard Deviation -> Died: " << sqrt(age_variance[0]) << " Survived: " << sqrt(age_variance[1]) << endl;
    cout << endl;
    cout << "METRICS" << endl;
    cout << "Confusion Matrix" << endl;
    cout << TP << "   " << FP << endl;
    cout << FN << "   " << TN << endl;
    cout << "Accuracy: " << accuracy << endl;
    cout << "Sensitivity: " << sensitivity << endl;
    cout << "Specificity: " << specificity << endl;
}