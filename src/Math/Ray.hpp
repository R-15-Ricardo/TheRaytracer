#pragma once

#include <armadillo>

struct Ray {
	arma::colvec3 origin;
	arma::colvec3 direction;

	Ray(arma::colvec3 o, arma::colvec3 d) : origin(o), direction(d) {}
};