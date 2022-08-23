#include "RayCaster.hpp"

#include <random>

double random_double()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

int closestObjectIndex(std::vector<double> intersections)
{
	int noOfObjects = intersections.size();
	if (noOfObjects == 0)
		return -1;

	double minValue = std::numeric_limits<double>::max();
	double minIndex = -1;

	for (int i = 0; i < noOfObjects; i++)
	{
		//std::cout<<intersections[i]<<", ";
		if (intersections[i] > 0 && intersections[i] < minValue)
		{
			minValue = intersections[i];
			minIndex = i;
		}
	}

	//std::cout<<std::endl;
	return minIndex;
}

Color getPixelPerModel(arma::colvec3 castingDirection, IntersectInfo hit, Scene scene, double tolerance, unsigned int maxDepth)
{
	Material strokeMaterial = scene.objects[hit.indexId]->getMaterial();

	Color strokeColor;

	if (strokeMaterial.hasTexture())
	{
		double u = hit.u;
		double v = hit.v;

		double textureW = strokeMaterial.texture.getWidth();
		double textureH = strokeMaterial.texture.getHeight();

		//std::cout<<textureW<<" "<<textureH<<std::endl;

		int x = std::floor(u*textureW);
		int y = std::floor(v*textureH);

		strokeColor = strokeMaterial.texture.getColor(x,y);
	}
	else
		strokeColor = strokeMaterial.baseColor;

	Color finalColor = scene.ambientLight*strokeColor;

	if (strokeMaterial.indexOfRefraction > 1 && maxDepth > 0)
	{
		double refractionRatio = (hit.outsideFace) ? (1.0/strokeMaterial.indexOfRefraction) : strokeMaterial.indexOfRefraction;
		double cos_theta = fmin(arma::dot(-castingDirection, hit.intscNormal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

		bool cannot_refract = refractionRatio * sin_theta > 1.0;

		//Calculate index of refractance
		double r0 = (1-refractionRatio)/(1+refractionRatio);
		r0 = r0*r0;
		double reflectance = r0 + (1-r0)*pow((1-cos_theta),5);

		if (cannot_refract)
		{
			strokeMaterial.reflectiveness = reflectance;
		}
		else
		{
			if (reflectance >= 0.1) strokeMaterial.reflectiveness = reflectance;
			else
			{
				if (random_double() > 0.01)
					strokeMaterial.reflectiveness = reflectance;
			}

			arma::colvec3 r_out_perp = refractionRatio * (castingDirection + cos_theta*hit.intscNormal);
			arma::colvec3 r_out_parallel = -std::sqrt(std::abs( 1.0 - arma::dot(r_out_perp, r_out_perp))) * hit.intscNormal;
			arma::colvec3 refractedDirection = arma::normalise(r_out_perp + r_out_parallel);

			Ray refractionRay (hit.intscPoint + refractedDirection ,refractedDirection);

			std::vector<double> refractionIntersecDistance;
			std::vector<IntersectInfo> refractionHitInfo;

			for (Object* o  : scene.objects)
			{
				IntersectInfo auxRefrHitInfo;
				refractionIntersecDistance.push_back(o->findIntersection(refractionRay, auxRefrHitInfo, true));
				refractionHitInfo.push_back(auxRefrHitInfo);
			}

			IntersectInfo closestRefractionInfo;
			int indexOfClosestRefraction = closestObjectIndex(refractionIntersecDistance);

			closestRefractionInfo = refractionHitInfo[indexOfClosestRefraction];
			closestRefractionInfo.indexId = indexOfClosestRefraction;

			if (closestRefractionInfo.indexId != -1)
			{
				if (refractionIntersecDistance[indexOfClosestRefraction])
				{
					Color refractedColor = getPixelPerModel(refractedDirection, closestRefractionInfo, scene, tolerance, maxDepth);
					finalColor = finalColor + refractedColor;
				}
			}
			else
			{
				int skyU = std::floor((0.5 + std::atan2(-refractionRay.direction[0], -refractionRay.direction[2])/(2*M_PI))*scene.skybox.getWidth());
				int skyV = std::floor((0.5 - std::asin(-refractionRay.direction[1])/M_PI) * scene.skybox.getHeight());

				Color skyColor = scene.skybox.getColor(skyU,skyV);
				finalColor = finalColor + skyColor;
			}
		}
		//strokeMaterial.reflectiveness = reflectance;
	}

	if (0 < strokeMaterial.reflectiveness && maxDepth > 0)
	{
		double dot1 = arma::dot(hit.intscNormal, -castingDirection);
		arma::colvec3 reflectionDirection = 2*(dot1*hit.intscNormal + castingDirection) - castingDirection;
		reflectionDirection = arma::normalise(reflectionDirection);

		Ray reflection_ray (hit.intscPoint, reflectionDirection);

		std::vector<double> reflectionIntersecDistance;
		std::vector<IntersectInfo> reflectionHitInfo;

		for (Object* o  : scene.objects)
		{
			IntersectInfo auxRefHitInfo;
			reflectionIntersecDistance.push_back(o->findIntersection(reflection_ray, auxRefHitInfo, true));
			reflectionHitInfo.push_back(auxRefHitInfo);
		}

		IntersectInfo closestReflectionInfo;
		int indexOfClosestReflection = closestObjectIndex(reflectionIntersecDistance);

		closestReflectionInfo = reflectionHitInfo[indexOfClosestReflection];
		closestReflectionInfo.indexId = indexOfClosestReflection;

		if (closestReflectionInfo.indexId != -1)
		{
			if (reflectionIntersecDistance[indexOfClosestReflection] > tolerance)
			{
				Color reflectedColor = getPixelPerModel(reflectionDirection, closestReflectionInfo, scene, tolerance, maxDepth-1);
				finalColor = finalColor + strokeMaterial.reflectiveness*reflectedColor;
			}
		}
		else
		{
			int skyU = std::floor((0.5 + std::atan2(-reflection_ray.direction[0], -reflection_ray.direction[2])/(2*M_PI))*scene.skybox.getWidth());
			int skyV = std::floor((0.5 - std::asin(-reflection_ray.direction[1])/M_PI) * scene.skybox.getHeight());

			Color skyColor = scene.skybox.getColor(skyU,skyV);
			finalColor = finalColor + strokeMaterial.reflectiveness*skyColor;
		}

	}

	for (LightSource* light : scene.lightSources)
	{
		arma::colvec3 lightDirection = light->position - hit.intscPoint;

		float distance2Light = arma::norm(lightDirection);
		lightDirection = arma::normalise(lightDirection);

		double cosine_angle = arma::dot(hit.intscNormal, lightDirection);

		if (cosine_angle > 0)
		{
			bool shadowed = false;
			Ray shadowTestRay (hit.intscPoint, lightDirection);

			std::vector<double> shadowTestIntersections;
			//This is just a place holder. I don't use this for anything
			IntersectInfo expendable;
			//---------------------------------

			for (Object* o : scene.objects)
				shadowTestIntersections.push_back(o->findIntersection(shadowTestRay, expendable, false));

			for (double shadowIntersection: shadowTestIntersections)
			{
				if (shadowed) break;
				if (shadowIntersection > tolerance)
				{
					if (shadowIntersection <= distance2Light)
						shadowed = true;
				}
			}

			if (!shadowed)
			{
				double finalLight = light->intensity*cosine_angle;
				finalColor = finalColor + finalLight * strokeColor*light->ligthColor;
				if (0 < strokeMaterial.gloss && strokeMaterial.gloss <= 1)
				{
					double dot1 = arma::dot(hit.intscNormal,-castingDirection);
					arma::colvec3 scalar1 = 2*(dot1*hit.intscNormal + castingDirection) - castingDirection;
					arma::colvec3 reflectionDirection = arma::normalise(scalar1);

					double specular = arma::dot(reflectionDirection, lightDirection);
					if (specular > 0)
					{
						specular = std::pow(specular, 10) * strokeMaterial.gloss;
						finalColor = finalColor + specular*light->ligthColor;
					}
				}
			}
		}
	}

	finalColor.clip();

	return finalColor;
}
