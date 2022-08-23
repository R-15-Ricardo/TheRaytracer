#include "Object.hpp"

// ------------------ Plane class definitions ------------------
Plane::Plane() : normal(arma::colvec3({0,1,0})), distance(0), material(Material()) {}

Plane::Plane(arma::colvec3 norm, double dist, Material mat) : distance(dist), material(mat)
{
	this->normal = arma::normalise(norm);
}

void Plane::setNormal(arma::colvec3 norm)
{
	this->normal = arma::normalise(norm);
}
arma::colvec3 Plane::getNormal() {return this->normal;}

double Plane::findIntersection(Ray ray, IntersectInfo& hit, bool getUV)
{
	hit.intscNormal = arma::colvec3({0,0,0});
	hit.intscPoint = arma::colvec3({0,0,0});

	double a = arma::dot(ray.direction,this->normal);
	double epsilon = std::sqrt(getMachineEpsilon<double>());//getMachineEpsilon<double>();

	if (a == 0) return -1;
	else
	{
		double b = arma::dot(this->distance*this->normal - ray.origin, this->normal);
		if (b/a < 0) return -1;
		else
		{
			double lambda = b/a - epsilon;
			hit.intscPoint = ray.origin + lambda*ray.direction;
			if (arma::dot(ray.direction, this->normal) < 0)
			{
				hit.intscNormal = this->normal;
				hit.outsideFace = true;
			}
			else
			{
				hit.intscNormal = -this->normal;
				hit.outsideFace = false;
			}

			if (getUV)
			{
				double proyX = hit.intscPoint[0] - 5;
				double proyZ = hit.intscPoint[2] - 5;
				hit.u = proyX/10.0 - std::floor(proyX/10.0);
				hit.v = proyZ/10.0 - std::floor(proyZ/10.0);
			}

			return lambda;
		}
	}
}


// ------------------ Sphere class definitions ------------------
Sphere::Sphere() : center(arma::colvec3({0,0,0})), radius(1.0), material(Material()) {}
Sphere::Sphere(arma::colvec3 pos, double rad, Material mat) : center(pos), radius(rad), material(mat) {}

double Sphere::findIntersection(Ray ray, IntersectInfo& hit, bool getUV)
{
	hit.intscNormal = arma::colvec3({0,0,0});
	hit.intscPoint = arma::colvec3({0,0,0});

	double epsilon = std::sqrt(getMachineEpsilon<double>());
	arma::colvec3 diff = ray.origin - this->center;

	//Assume ray direction is normalized
	double B = arma::dot(2*ray.direction,diff);
	double C = arma::dot(diff, diff) - this->radius*this->radius;

	double discriminant = B*B - 4*C;

	if (0 < discriminant)
	{
		double hitDistance = (-B - std::sqrt(discriminant))/2.0 - epsilon;

		if (hitDistance <= 0)
			hitDistance = (std::sqrt(discriminant) - B)/2.0 - epsilon;

		hit.intscPoint = ray.origin + hitDistance*ray.direction;

		arma::colvec3 outwardNormal = arma::normalise(hit.intscPoint - this->center);
		if (arma::dot(ray.direction,outwardNormal) <= 0)
		{
			hit.intscNormal = outwardNormal;
			hit.outsideFace = true;
		}
		else
		{
			hit.intscNormal = -outwardNormal;
			hit.outsideFace = false;
		}

		if(getUV)
		{
			arma::colvec3 d_hat = arma::normalise(this->center - hit.intscPoint);
			hit.u = 0.5 + std::atan2(d_hat[0], d_hat[2])/(2*M_PI);
			hit.v = 0.5 - std::asin(d_hat[1])/M_PI;
		}

		return hitDistance;
	}
	else
		return -1;
}

// ------------------ Triangle class definitions ------------------

Triangle::Triangle() : A(arma::colvec3({1,0,0})), B(arma::colvec3({0,1,0})), C(arma::colvec3({0,0,1})), material(Material())
{
	this->normal = arma::normalise(arma::cross(this->B - this->A, this->C - this->A));
	this->distance = arma::dot(this->normal, this->A);

	this->textureCoords[0][0] = 0; this->textureCoords[0][1] = 0;
	this->textureCoords[1][0] = 1; this->textureCoords[1][1] = 0;
	this->textureCoords[2][0] = 0; this->textureCoords[2][1] = 1;
}

Triangle::Triangle(arma::colvec3 vertexA, arma::colvec3 vertexB, arma::colvec3 vertexC, Material mat) : A(vertexA), B(vertexB), C(vertexC), material(mat)
{
	this->normal = arma::normalise(arma::cross(vertexB - vertexA, vertexC - vertexA));
	this->distance = arma::dot(this->normal, vertexA);

	this->textureCoords[0][0] = 0; this->textureCoords[0][1] = 0;
	this->textureCoords[1][0] = 1; this->textureCoords[1][1] = 0;
	this->textureCoords[2][0] = 0; this->textureCoords[2][1] = 1;
}

Triangle::Triangle(arma::colvec3 vertexA, arma::colvec3 vertexB, arma::colvec3 vertexC, double textCoords[3][2]) : A(vertexA), B(vertexB), C(vertexC), material(Material())
{
	this->normal = arma::normalise(arma::cross(vertexB - vertexA, vertexC - vertexA));
	this->distance = arma::dot(this->normal, vertexA);

	this->textureCoords[0][0] = textCoords[0][0]; this->textureCoords[0][1] = textCoords[0][1];
	this->textureCoords[1][0] = textCoords[1][0]; this->textureCoords[1][1] = textCoords[1][1];
	this->textureCoords[2][0] = textCoords[2][0]; this->textureCoords[2][1] = textCoords[2][1];
}

std::ostream& operator<<(std::ostream& stream, const Triangle& t)
{
	stream<<"Triangle: "<<std::endl;
	stream<<"---------------"<<std::endl;
	stream<<"A "<<t.A.t()<<std::endl;
	stream<<"B "<<t.B.t()<<std::endl;
	stream<<"C "<<t.C.t()<<std::endl;
	stream<<"ṅ "<<t.normal.t()<<std::endl;
	stream<<"---------------"<<std::endl;
	return stream;
}

double Triangle::findIntersection(Ray ray, IntersectInfo& hit, bool getUV)
{
	hit.intscNormal = arma::colvec3 ({0,0,0});
	hit.intscPoint = arma::colvec3 ({0,0,0});

	double a = arma::dot(ray.direction, this->normal);
	double epsilon = std::sqrt(getMachineEpsilon<double>());

	if (a == 0) return -1;
	else
	{
		double b = arma::dot(this->A - ray.origin, this->normal);
		double t2plane = b/a;
		if (t2plane < 0) return -1;

		arma::colvec3 P = ray.origin + t2plane*ray.direction;

		arma::colvec3 edge0 = B-A;
		arma::colvec3 edge1 = C-B;
		arma::colvec3 edge2 = A-C;

		arma::colvec3 C0 = P - A;
		arma::colvec3 C1 = P - B;
		arma::colvec3 C2 = P - C;

		double test1 = arma::dot(this->normal, arma::cross(edge0, C0));
		double test2 = arma::dot(this->normal, arma::cross(edge1, C1));
		double test3 = arma::dot(this->normal, arma::cross(edge2, C2));

		if (0 <= test1 && 0 <= test2 && 0 <= test3)
		{
			double lambda = t2plane - epsilon;
			hit.intscPoint = ray.origin + lambda*ray.direction;

			if (arma::dot(ray.direction, this->normal) < 0)
			{
				hit.intscNormal = this->normal;
				hit.outsideFace = true;
			}
			else
			{
				hit.intscNormal = -this->normal;
				hit.outsideFace = false;
			}

			if (getUV)
			{
				arma::colvec3 f1 = this->A - hit.intscPoint;
				arma::colvec3 f2 = this->B - hit.intscPoint;
				arma::colvec3 f3 = this->C - hit.intscPoint;

				double area = arma::norm(arma::cross(this->B - this->A, this->C - this->A));
				double a1 = arma::norm(arma::cross(f2,f3))/area;
				double a2 = arma::norm(arma::cross(f3,f1))/area;
				double a3 = arma::norm(arma::cross(f1,f2))/area;

				hit.u = textureCoords[0][0]*a1 + textureCoords[1][0]*a2 + textureCoords[2][0] * a3;
				hit.v = textureCoords[0][1]*a1 + textureCoords[1][1]*a2 + textureCoords[2][1] * a3;
			}

			return lambda;
		}
		else return -1;
	}
}


// ------------------ Mesh class definitions ------------------
Mesh::Mesh() : meshMaterial(Material())
{
	this->triangles.push_back(Triangle());
}

Mesh::Mesh(const std::string objPath, Material mat) : meshMaterial(mat)
{
	std::ifstream objFile(objPath, std::ios::in);
	if (!objFile)
	{
		std::cerr<<"Error! Cannot open "<<objPath<<"!"<<std::endl;
		return;
	}

	std::string line;
	std::vector<arma::colvec3> vertices;
	std::vector<arma::colvec2> textureUV;

	while (std::getline(objFile, line))
	{
		if (line.substr(0,2) == "v ")
		{
			std::istringstream vertexLine(line.substr(2));
			double x, y, z;
			vertexLine>>x;
			vertexLine>>y;
			vertexLine>>z;
			vertices.push_back(arma::colvec3({x,y,z}));
		}
		else if (line.substr(0,2) == "vt")
		{
			std::istringstream textureUVLine(line.substr(3));
			double u,v;
			textureUVLine>>u;
			textureUVLine>>v;
			textureUV.push_back(arma::colvec2({u,v}));
		}
		else if (line.substr(0,2) == "f ")
		{
			int viA, viB, viC;
			int tiA, tiB, tiC;

			const char* chh = line.c_str();
			sscanf(chh, "f %i/%i %i/%i %i/%i",&viA,&tiA,&viB,&tiB,&viC,&tiC);

			double textureCoords[3][2];
			textureCoords[0][0] = textureUV[tiA-1][0]; textureCoords[0][1] = textureUV[tiA-1][1];
			textureCoords[1][0] = textureUV[tiB-1][0]; textureCoords[1][1] = textureUV[tiB-1][1];
			textureCoords[2][0] = textureUV[tiC-1][0]; textureCoords[2][1] = textureUV[tiC-1][1];

			//std::cout<<vertices[viA-1] + pos<<vertices[viB-1] + pos<<vertices[viC-1] + pos<<std::endl;
			Triangle t(vertices[viA-1], vertices[viB-1], vertices[viC-1], textureCoords);


			this->triangles.push_back(t);
		}

	}

	//Calculate Ritter's bounding sphere
	arma::colvec3 P1 = vertices[0];

	double maxDistance = 0;
	arma::colvec3 P2;
	for (arma::colvec3 v : vertices)
		P2 = (arma::norm(v - P1) > maxDistance) ? v : P2;

	maxDistance = 0;
	arma::colvec3 P3;
	for (arma::colvec3 v : vertices)
		P3 = (arma::norm(v - P2) > maxDistance) ? v : P3;

	arma::colvec3 guessCenter = 0.5*(P2 + P3);
	double guessRadius = arma::norm(P2 - P3)/2;

	std::vector<arma::colvec3> exterior;
	for (arma::colvec3 v : vertices)
	{
		if (arma::norm(v - guessCenter) > guessRadius)
			exterior.push_back(v);
	}

	while (!exterior.empty())
	{
		arma::colvec3 pt = exterior.back();
		exterior.pop_back();

		if (arma::norm(pt - guessCenter) > guessRadius)
		{
			guessRadius = arma::norm(pt - guessCenter);
		}
	}

	this->BSphereCenter = guessCenter;
	this->BSphereRadius = guessRadius;
}

Mesh::Mesh(const std::string objPath, arma::colvec3 pos, arma::colvec3 ori, double size, Material mat) : meshMaterial(mat)
{
	this->translation = pos;
	this->rotation = ori;
	this->scale = size;

	this->setTransform();

	std::ifstream objFile(objPath, std::ios::in);
	if (!objFile)
	{
		std::cerr<<"Error! Cannot open "<<objPath<<"!"<<std::endl;
		return;
	}

	std::string line;
	std::vector<arma::colvec3> vertices;
	std::vector<arma::colvec2> textureUV;
	std::vector<faceIndices> indices;

	while (std::getline(objFile, line))
	{
		if (line.substr(0,2) == "v ")
		{
			std::istringstream vertexLine(line.substr(2));
			double x, y, z;
			vertexLine>>x;
			vertexLine>>y;
			vertexLine>>z;
			vertices.push_back(arma::colvec3({x,y,z}));
		}
		else if (line.substr(0,2) == "vt")
		{
			std::istringstream textureUVLine(line.substr(3));
			double u,v;
			textureUVLine>>u;
			textureUVLine>>v;
			textureUV.push_back(arma::colvec2({u,v}));
		}
		else if (line.substr(0,2) == "f ")
		{
			int viA, viB, viC;
			int tiA, tiB, tiC;

			const char* chh = line.c_str();
			sscanf(chh, "f %i/%i %i/%i %i/%i",&viA,&tiA,&viB,&tiB,&viC,&tiC);

			faceIndices nowIndex = {viA,tiA,viB,tiB,viC,tiC};
			indices.push_back(nowIndex);
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		arma::colvec4 affineVertex ({vertices[i][0],vertices[i][1],vertices[i][2],1.0});

		affineVertex = frame*affineVertex;

		vertices[i] = affineVertex.subvec(0,2);
	}


	//Calculate Ritter's bounding sphere
	arma::colvec3 P1 = vertices[0];

	double maxDistance = 0;
	arma::colvec3 P2;
	for (arma::colvec3 v : vertices)
		P2 = (arma::norm(v - P1) > maxDistance) ? v : P2;

	maxDistance = 0;
	arma::colvec3 P3;
	for (arma::colvec3 v : vertices)
		P3 = (arma::norm(v - P2) > maxDistance) ? v : P3;

	arma::colvec3 guessCenter = 0.5*(P2 + P3);
	double guessRadius = arma::norm(P2 - P3)/2;

	std::vector<arma::colvec3> exterior;
	for (arma::colvec3 v : vertices)
	{
		if (arma::norm(v - guessCenter) > guessRadius)
			exterior.push_back(v);
	}

	while (!exterior.empty())
	{
		arma::colvec3 pt = exterior.back();
		exterior.pop_back();

		if (arma::norm(pt - guessCenter) > guessRadius)
		{
			guessRadius = arma::norm(pt - guessCenter);
		}
	}

	this->BSphereCenter = guessCenter;
	this->BSphereRadius = guessRadius;

	for (faceIndices face : indices)
	{
		double textureCoords[3][2];
		textureCoords[0][0] = textureUV[face.t1-1][0]; textureCoords[0][1] = textureUV[face.t1-1][1];
		textureCoords[1][0] = textureUV[face.t2-1][0]; textureCoords[1][1] = textureUV[face.t2-1][1];
		textureCoords[2][0] = textureUV[face.t3-1][0]; textureCoords[2][1] = textureUV[face.t3-1][1];

		Triangle t(vertices[face.v1-1], vertices[face.v2-1], vertices[face.v3-1], textureCoords);
		this->triangles.push_back(t);
	}

}

double Mesh::findIntersection(Ray ray, IntersectInfo& hit, bool getUV)
{
	hit.intscNormal = arma::colvec3 ({0,0,0});
	hit.intscPoint = arma::colvec3 ({0,0,0});

	//Test against bounding sphere
	double B = arma::dot(2*ray.direction,ray.origin - this->BSphereCenter);
	double C = arma::dot(ray.origin - this->BSphereCenter, ray.origin - this->BSphereCenter) - this->BSphereRadius*this->BSphereRadius;

	double discriminant = B*B - 4*C;

	if (0 > discriminant) return -1;

	//Ray hitted the bounding sphere
	std::vector<double> intersectionsDistance;
	std::vector<IntersectInfo> hitInfo;

	for (Triangle t: this->triangles)
	{
		IntersectInfo auxHitInfo;
		intersectionsDistance.push_back(t.findIntersection(ray, auxHitInfo, getUV));
		hitInfo.push_back(auxHitInfo);
	}

	int noOfObjects = intersectionsDistance.size();
	if (noOfObjects == 0)
		return -1;

	double minValue = std::numeric_limits<double>::max();
	double minIndex = -1;

	for (int i = 0; i < noOfObjects; i++)
	{
		if (intersectionsDistance[i] > 0 && intersectionsDistance[i] < minValue)
		{
			minValue = intersectionsDistance[i];
			minIndex = i;
		}
	}
	if (minIndex != -1)
	{
		hit = hitInfo[minIndex];
		return intersectionsDistance[minIndex];
	}
	return -1;
}

void Mesh::setTransform()
{
	arma::mat44 R = getEulerAnglesMat(this->rotation);
	arma::mat44 T = getTranslateMat(this->translation);
	arma::mat44 S = getScaleMat(this->scale);

	this->frame = S*T*R;
}

void Mesh::setPosition(double x, double y, double z)
{
	this->translation = arma::colvec3({x,y,z});
	this->setTransform();
}

void Mesh::setSize(double a)
{
	this->scale = a;
	this->setTransform();
}

void Mesh::setOrientation(double yaw, double pitch, double roll)
{
	this->rotation = arma::colvec3({yaw,pitch,roll});
	this->setTransform();
}
