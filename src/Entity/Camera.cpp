#include "Camera.hpp"

Camera::Camera()
{
	this->position = arma::colvec3({0,0,0});
	this->x_hat = arma::colvec3({1,0,0});
	this->y_hat = arma::colvec3({0,1,0});
	this->z_hat = arma::colvec3({0,0,1});

	this->setFOV(45);
}

Camera::Camera(arma::colvec3 camPos, arma::colvec3 camDir, double degreeFOV)
{
	this->position = camPos;

	this->z_hat = arma::normalise(camDir - camPos);
	this->x_hat = arma::normalise(arma::cross(this->z_hat, arma::colvec3({0,1,0})));
	this->y_hat = arma::normalise(arma::cross(this->x_hat,this->z_hat));

	this->setFOV(degreeFOV);
}

void Camera::setFOV(double degreeFOV)
{
	if (90 <= degreeFOV) degreeFOV = 89;
	else if (degreeFOV <= 0) degreeFOV = 1;

	this->radiansFOV = degreeFOV * M_PI/180.0;
}

