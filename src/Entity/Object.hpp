#pragma once

#include <cmath>
#include <armadillo>
#include "../Util/Epsilon.hpp"
#include "../Math/Ray.hpp"
#include "../Math/AffineTransform.hpp"
#include "../Material/Material.hpp"

//TODO: implement the getters for texture coordenates

struct faceIndices {
	int v1;
	int t1;

	int v2;
	int t2;

	int v3;
	int t3;
};

struct IntersectInfo {
	int indexId = 0;

	arma::colvec3 intscPoint;
	arma::colvec3 intscNormal;
	double outsideFace = true;
	double u = 0;
	double v = 0;
};

class Object {
	public:
		Object() {};

		virtual Material getMaterial () = 0;
		virtual double findIntersection (Ray ray, IntersectInfo& hit, bool getUV) = 0;

	private:
		//another empty field...
};

class Plane : public Object {
	public:
		double distance;
		Material material;
		Plane();
		Plane(arma::colvec3 norm, double dist, Material mat);

		void setNormal(arma::colvec3 norm);
		arma::colvec3 getNormal();

		Material getMaterial () override {return this->material;}
		double findIntersection (Ray ray, IntersectInfo& hit, bool getUV) override;

	private:
		arma::colvec3 normal;

};

class Sphere : public Object{
	public:
		arma::colvec3 center;
		double radius;
		Material material;

		Sphere();
		Sphere(arma::colvec3 pos, double rad, Material mat);

		Material getMaterial () override {return this->material;}
		double findIntersection (Ray ray, IntersectInfo& hit, bool getUV) override;

	private:
		//Empty again I guess...
};

class Triangle : public Object {
	/*
	        A *
			 ***            On right-handed frames of reference
			******          trinagle vertices are sorted in a
		   *********        counterclockwise manner.
		B ************ C    This way the normal always "comes" from the A vertex
    */
	public:
		Triangle();
		Triangle(arma::colvec3 vertexA, arma::colvec3 vertexB, arma::colvec3 vertexC, Material mat);
		Triangle(arma::colvec3 vertexA, arma::colvec3 vertexB, arma::colvec3 vertexC, double textCoords[3][2]);

		//FOR DEBUGGING!
		friend std::ostream& operator<<(std::ostream& stream, const Triangle& t);

		arma::colvec3 getPoint() {return this->A;}

		Material getMaterial () override {return this->material;}
		double findIntersection (Ray ray, IntersectInfo& hit, bool getUV) override;


	private:
		arma::colvec3 A;
		arma::colvec3 B;
		arma::colvec3 C;

		double distance;
		arma::colvec3 normal;

		Material material;
		double textureCoords[3][2];
};

class Mesh : public Object {
	public:
		Mesh();
		Mesh(const std::string objPath, Material mat);
		Mesh(const std::string objPath, arma::colvec3 pos, arma::colvec3 ori, double size, Material mat);

		Material getMaterial () override {return this->meshMaterial;}
		double findIntersection (Ray ray, IntersectInfo& hit, bool getUV) override;

		void setPosition(double x, double y, double z);
		void setSize(double a);
		void setOrientation(double yaw, double pitch, double roll);

	private:
		std::vector<Triangle> triangles;

		//Transform information
		arma::colvec3 rotation;
		arma::colvec3 translation;
		double scale;

		arma::mat44 frame;

		Material meshMaterial;

		arma::colvec3 BSphereCenter;
		double BSphereRadius;

		void setTransform();
};

