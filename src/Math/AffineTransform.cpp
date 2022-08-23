#include "AffineTransform.hpp"


arma::mat44 getEulerAnglesMat (arma::colvec3 orientation)
{
	//arma::mat44 X, Y, Z;
	double yaw = orientation[0] * M_PI/180.0;
	double pitch = orientation[1] * M_PI/180.0;
	double roll = orientation[2] * M_PI/180.0;

	double cyaw   = cos(yaw),   syaw   = sin(yaw),
	       cpitch = cos(pitch), spitch = sin(pitch),
	       croll  = cos(roll),  sroll  = sin(roll);

	arma::mat44 Y ({{ cyaw, 0.0, syaw, 0.0},
		 			{  0.0, 1.0,  0.0, 0.0},
		 			{-syaw, 0.0, cyaw, 0.0},
		 			{  0.0, 0.0,  0.0, 1.0}});

	arma::mat44 X ({{1.0,    0.0,     0.0, 0.0},
		 			{0.0, cpitch, -spitch, 0.0},
		 			{0.0, spitch,  cpitch, 0.0},
		 			{0.0,    0.0,     0.0, 1.0}});

	arma::mat44 Z ({{croll, -sroll, 0.0, 0.0},
		 			{sroll,  croll, 0.0, 0.0},
		 			{  0.0,    0.0, 1.0, 0.0},
		 			{  0.0,    0.0, 0.0, 1.0}});

	return Z*X*Y;
}

arma::mat44 getScaleMat (double scale)
{
	arma::mat44 S(arma::zeros(4,4));

	S.at(0,0) = scale;
	S.at(1,1) = scale;
	S.at(2,2) = scale;
	S.at(3,3) = 1.0;

	return S;
}

arma::mat44 getTranslateMat (arma::colvec3 position)
{
	arma::colvec4 affineTranslation ({position[0],position[1],position[2],1.0});

	arma::mat44 T (arma::eye(4,4));

	T.col(3) = affineTranslation;


	return T;
}
