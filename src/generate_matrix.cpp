#include <iostream>
#include <fstream>
#include <string> 
#include <random>

using namespace std;




int main(int argc, char *argv[])
{
    int size;
    string FILE_NAME;
    fstream matrix_file;

    if (argc != 3) {
        cout << "Invalid number of input arguments" << endl;
        return -1;
    }
    // Store matrix size
    size = atoi(argv[1]);

    // Matrix file name
    FILE_NAME = argv[2];
    matrix_file.open(FILE_NAME, ios::out | ios::app);
    double val;

    // Initialie random device
    random_device rd;
    mt19937 eng(rd());
    uniform_real_distribution<> distribution(-1,1);

    // Fill matrix
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            val = distribution(eng);
            if(j <size -1) {
                matrix_file << val << ",";
            } 
            else {
                matrix_file << val << endl;
            }
        }
    }
    matrix_file.close();
}