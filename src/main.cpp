#include <iostream>
#include <thread>

#include <vector>
#include <png++/png.hpp>
#include <armadillo>

#include <chrono>

#include "Renderer/Renderer.hpp"
#include "Util/RayCaster.hpp"
#include "Math/Ray.hpp"
#include "Math/AffineTransform.hpp"
#include "Entity/Camera.hpp"
#include "Entity/LightSource.hpp"
#include "Entity/Object.hpp"


int main (int argc, char *argv[])
{
	if (argc < 4 || argc > 5)
	{
		std::cout<<"Invalid arguments!"<<std::endl;
		std::cout<<"Expected args:"<<std::endl;
		std::cout<<"->	[scene JSON path] [width] [height] [aadepth](Optional. Default: no anti-aliasing)"<<std::endl;
		return 0;
	}

	std::string JSONPath (argv[1]);
	int width = std::stoi(argv[2]);
	int height = std::stoi(argv[3]);
	int aadepth = (argc == 5) ? std::stoi(argv[4]) : 1;

	png::image<png::rgb_pixel> finalImage(width, height);

	Scene test = loadFromJSON(JSONPath);

	int totalChucks = std::thread::hardware_concurrency();

	int pDirectionSize = (width > height) ? width : height;
	bool splitDirection = (width > height) ? 1 : 0;
	int p_offset = pDirectionSize/totalChucks;

	std::thread t[totalChucks];
	std::atomic<int> report[totalChucks];
	int pixelCt;
	double progress;
	int barWidth = 50;

	std::cout<<"rendering..."<<std::endl;

	//Progress bar thingy
	auto tp1 = std::chrono::high_resolution_clock::now();

	for (int chunk = 0; chunk < totalChucks; chunk++)
	{
		int offset = chunk*p_offset;
		int chunkSize = p_offset;
		if (chunk == totalChucks - 1)
			chunkSize = (offset + p_offset == pDirectionSize) ? p_offset : pDirectionSize - offset;

		report[chunk].store(0);
		t[chunk] = std::thread(RenderChunk,std::ref(finalImage),splitDirection,offset,chunkSize,aadepth,test,&report[chunk]);
	}

	progress = 0;
	while (progress < 1)
	{
		pixelCt = 0;
		for (int chunk = 0; chunk < totalChucks; chunk++)
			pixelCt += report[chunk].load();

		progress = (double)pixelCt/(double)(width*height);
		std::cout<<"[";
		int pos = barWidth*progress;
		for (int i = 0; i<barWidth; i++)
		{
			if (i<pos) std::cout<<"=";
			else if (i == pos) std::cout<<">";
			else std::cout<<" ";
		}
		std::cout<<"]"<<progress*100.0<<" %        \r";
		std::cout.flush();
	}
	std::cout<<std::endl;
	for (int chunk = 0; chunk < totalChucks; chunk++)
		t[chunk].join();


	auto tp2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = tp2 - tp1;

	std::cout<<"Finished in "<<elapsedTime.count()<<" secs"<<std::endl;
	std::string outputfile = JSONPath.substr(0,JSONPath.size()-5);
	outputfile += "_out.png";

	std::cout<<"Image in "<<outputfile<<std::endl;

	finalImage.write(outputfile);

	return 0;
}

