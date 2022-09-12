#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

double Sum(vector<double> v) {
  double sum = 0;

  for (double num : v) {
    sum = sum + num;
  }

  return sum;
}

double Mean(vector<double> v) {
  double mean = 0;

  mean = Sum(v) / v.size();

  return mean;
}

double Median(vector<double> v) {
  double median = 0;
  sort(v.begin(), v.end());

  if (v.size() % 2 == 0) {
    median = (v[(v.size() / 2)] + v[(v.size() / 2) - 1]) / 2.0;
    return median;
  } else {
    median = v[(v.size() / 2)];
    return median;
  }
}

double Range(vector<double> v) {
  double range = 0;

  sort(v.begin(), v.end());
  range = v[v.size() - 1] - v[0];

  return range;
}

void Print_Stats(vector<double> v) {
  cout << "Sum: " << Sum(v) << endl;
  cout << "Mean: " << Mean(v) << endl;
  cout << "Median: " << Median(v) << endl;
  cout << "Range: " << Range(v) << endl;
}

double Covar(vector<double> x, vector<double> y) {
  double xBar = Mean(x);
  double yBar = Mean(y);
  double numerator = 0;
  double denominator = x.size() - 1;

  for (size_t i = 0; i < x.size() - 1; i++) {
    numerator += (x[i] - xBar) * (y[i] - yBar); 
  }
  
  return numerator / denominator;
}

double Cor(vector<double> x, vector<double> y) {
  double xBar = Mean(x);
  double yBar = Mean(y);
  double varianceX = 0;
  double varianceY = 0;

  for (size_t i = 0; i < x.size() - 1; i++) {
    varianceX += pow(x[i] - xBar, 2) / (x.size() - 1);
    varianceY += pow(y[i] - yBar, 2) / (y.size() - 1);
  }
  
  double sigmaX = sqrt(varianceX);
  double sigmaY = sqrt(varianceY);
  double numerator = Covar(x, y);
  double denominator = sigmaX * sigmaY;

  return numerator / denominator;
}

int main(int argc, char** argv) {
  ifstream inFS; //input file stream
  string line;
  string rm_in, medv_in;
  const int MAX_LEN = 1000;
  vector<double> rm(MAX_LEN);
  vector<double> medv(MAX_LEN);

  // Try to open file
  cout << "Opening file Boston.csv." << endl;

  inFS.open("Boston.csv");
  if (!inFS.is_open()) {
    cout << "Could not open file Boston.csv." << endl;
    return 1; // 1 indicated error
  }

  // Can now use inFS stream like cin stream
  // Boston.csv should contain two doubles

  cout << "Reading line 1" << endl;
  getline(inFS, line);

  // echo heading
  cout << "heading: " << line << endl;

  int numObservations = 0;
  while (inFS.good()) {

    getline(inFS, rm_in, ',');
    getline(inFS, medv_in, '\n');

    rm.at(numObservations) = stof(rm_in);
    medv.at(numObservations) = stof(medv_in);

    numObservations++;
  }

  rm.resize(numObservations);
  medv.resize(numObservations);

  cout << "new length " << rm.size() << endl;

  cout << "Closing file Boston.csv." << endl;
  inFS.close(); // Done with file, so close it

  cout << "Number of records: " << numObservations << endl;

  cout << "\nStats for rm" << endl;
  Print_Stats(rm);

  cout << "\nStats for medv" << endl;
  Print_Stats(medv);

  cout << "\n Covariance = " << Covar(rm, medv) << endl;

  cout << "\n Correlation = " << Cor(rm, medv) << endl;

  cout << "\n Program terminated.";

  return 0;
}


