#pragma once

#include <armadillo>

class Camera {
	public:
		arma::colvec3 position;

		Camera();
		Camera(arma::colvec3 camPos, arma::colvec3 camDir, double degreeFOV);

		//degree -> radian
		void setFOV(double degreeFOV);
		//in radians
		double getFOV() {return this->radiansFOV;}
		arma::colvec3 up() const {return this->y_hat;}
		arma::colvec3 front() const {return this->z_hat;}
		arma::colvec3 right() const {return this->x_hat;}

	private:
		double radiansFOV;

		arma::colvec3 x_hat;
		arma::colvec3 y_hat;
		arma::colvec3 z_hat;
};

