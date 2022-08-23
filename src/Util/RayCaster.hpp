#pragma once

#include <iostream>
#include <vector>
#include <limits>

#include <vector>
#include <armadillo>
#include <png++/rgb_pixel.hpp>

#include "../Renderer/Scene.hpp"
#include "../Util/RayCaster.hpp"
#include "../Math/Ray.hpp"
#include "../Entity/Camera.hpp"
#include "../Entity/LightSource.hpp"
#include "../Entity/Object.hpp"


int closestObjectIndex(std::vector<double> intersections);
Color getPixelPerModel(arma::colvec3 castingDirection, IntersectInfo hit, Scene scene,double tolerance, unsigned int maxDepth);
