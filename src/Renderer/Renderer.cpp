#include "Renderer.hpp"

#include "../Util/RayCaster.hpp"

void RenderImage(png::image<png::rgb_pixel>& finalImage, int aadepth, Scene& scene, std::atomic<int>* report)
{
	int W = finalImage.get_width();
	RenderChunk(finalImage,true,0, W, aadepth,scene,report);
}

void RenderChunk(png::image<png::rgb_pixel>& finalImage, bool splitDirection, int offset, int size, int aadepth, Scene scene, std::atomic<int>* report)
{
	int imageWidth = finalImage.get_width();
	int imageHeight = finalImage.get_height();

	double epsilon = std::sqrt(getMachineEpsilon<double>());
	double aspectratio = (double)imageWidth/(double)imageHeight;
	double worldH = std::tan(scene.camera.getFOV());
	double worldW = aspectratio*worldH;

	int xStart, xEnd, yStart, yEnd;

	if (splitDirection)
	{
		xStart = offset;
		xEnd = offset + size;
		yStart = 0;
		yEnd = imageHeight;
	}
	else
	{
		xStart = 0;
		xEnd = imageWidth;
		yStart = offset;
		yEnd = offset + size;
	}

	for (int x = xStart; x < xEnd; x++)
	{
		for (int y = yStart; y < yEnd; y++)
		{

			report->operator++();

			double cumulativeRed = 0;
			double cumulativeGreen = 0;
			double cumulativeBlue = 0;

			for (int aax = 0; aax < aadepth; aax++)
			{
				for (int aay = 0; aay < aadepth; aay++)
				{
					double aadeltaX = (aadepth > 1) ? (double)aax/(double)aadepth : 1;
					double aadeltaY = (aadepth > 1) ? (double)aay/(double)aadepth : 1;

					double deltaX = 2.0*((double)x + aadeltaX)/(double)imageWidth - 1;
					double deltaY = 2.0*((double)y + aadeltaY)/(double)imageHeight - 1;

					double alpha = worldH*deltaY;
					double beta = worldW*deltaX;

					/*Casting rays like this  >---> ---> ---> ---> v
																   v
											  >---> ---> ---> ---> v
																   v
											  >---> ---> ---> ---> v
					  Since png++ (c++ wrapper for libpng) expects "mirrored againts X axis" coordinates (origin at bottom left)*/

					arma::colvec3 castingDirection = arma::normalise(scene.camera.front() - alpha*scene.camera.up() + beta*scene.camera.right());
					Ray castedRay (scene.camera.position, castingDirection);

					std::vector<double> rayIntersectionDistance;
					std::vector<IntersectInfo> hitInfo;

					for (Object* o : scene.objects)
					{
						IntersectInfo auxHitInfo;
						rayIntersectionDistance.push_back(o->findIntersection(castedRay, auxHitInfo, true));
						hitInfo.push_back(auxHitInfo);
					}

					IntersectInfo closestIntersection;
					int nowClosest = closestObjectIndex(rayIntersectionDistance);
					closestIntersection = hitInfo[nowClosest];
					closestIntersection.indexId = nowClosest;
					//std::cout<<nowClosest;

					if (closestIntersection.indexId == -1)
					{
						int u = std::floor((0.5 + std::atan2(-castedRay.direction[0], -castedRay.direction[2])/(2*M_PI))*scene.skybox.getWidth());
						int v = std::floor((0.5 - std::asin(-castedRay.direction[1])/M_PI) * scene.skybox.getHeight());

						Color skyColor = scene.skybox.getColor(u,v);

						cumulativeRed += skyColor.r;
						cumulativeGreen += skyColor.g;
						cumulativeBlue += skyColor.b;
					}
					else
					{
						Color pixelColor = getPixelPerModel(castingDirection, closestIntersection, scene, epsilon, 100);

						cumulativeRed += pixelColor.r;
						cumulativeGreen += pixelColor.g;
						cumulativeBlue += pixelColor.b;
					}

				}
			}

			Color aaColor;
			aaColor.r = cumulativeRed/(double)(aadepth*aadepth);
			aaColor.g = cumulativeGreen/(double)(aadepth*aadepth);
			aaColor.b = cumulativeBlue/(double)(aadepth*aadepth);

			aaColor.clip();

			finalImage.set_pixel(x,y,png::rgb_pixel(aaColor.getPixelLevel('r'), aaColor.getPixelLevel('g'), aaColor.getPixelLevel('b')));
		}
	}
}
