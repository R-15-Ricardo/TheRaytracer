#pragma once

#include <armadillo>

arma::mat44 getEulerAnglesMat (arma::colvec3 orientation);
arma::mat44 getScaleMat (double scale);
arma::mat44 getTranslateMat (arma::colvec3 position);
