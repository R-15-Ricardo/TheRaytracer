#pragma once

#include <armadillo>
#include "../Material/Material.hpp"

struct LightSource {
	arma::colvec3 position;
	Color ligthColor;
	double intensity;

	LightSource (arma::colvec3 pos, Color col, double ints) : position(pos), ligthColor(col), intensity(ints) {}
};