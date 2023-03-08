#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <Eigen/Geometry>


using namespace Eigen;
using namespace std;


Matrix2d ARM(double angle){
    angle = angle * M_PI / 180;
    Matrix2d mat;
    mat << cos(angle), -sin(angle),
           sin(angle),  cos(angle);
    return mat;
}

int main(){

}
