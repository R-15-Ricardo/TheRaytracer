#include "Scene.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

Scene loadFromJSON(const std::string path)
{
	std::cout<<"Loading scene..."<<std::endl;
	Scene readScene;

	pt::ptree root;
	pt::read_json(path, root);

	//load camera setting
	std::cout<<"[SCENE] loading camera..."<<std::endl;
	pt::ptree &camera_settings = root.get_child("camera");

	arma::colvec3 cam_pos;
	arma::colvec3 cam_headding;
	int i = 0;
	for (pt::ptree::value_type &value : camera_settings.get_child("position"))
		cam_pos[i++] = value.second.get_value<double>();
	i = 0;
	for (pt::ptree::value_type &value : camera_settings.get_child("headding"))
		cam_headding[i++] = value.second.get_value<double>();

	double FOV = (camera_settings.get<double>("FOV"))/2.0;

	Camera scene_cam(cam_pos,cam_headding,FOV);
	readScene.camera = scene_cam;

	//load sky
	std::cout<<"[SCENE] loading skydome..."<<std::endl;
	pt::ptree &sky_settings = root.get_child("sky");
	std::string skybox_texture = sky_settings.get<std::string>("dome_path");
	double ambientLight = sky_settings.get<double>("ambient");

	Texture skyTexture (skybox_texture);
	readScene.skybox = skyTexture;
	readScene.ambientLight = ambientLight;

	//load light
	std::cout<<"[SCENE] loading light sources..."<<std::endl;
	for (pt::ptree::value_type &source : root.get_child("light_sources"))
	{
		std::cout<<"[SCENE :: LIGHT] loading light..."<<std::endl;
		int index;

		arma::colvec3 light_pos; index = 0;
		for (pt::ptree::value_type &value : source.second.get_child("position"))
			light_pos[index++] = value.second.get_value<double>();

		arma::colvec3 int_colors; index = 0;
		for (pt::ptree::value_type &value : source.second.get_child("color"))
			int_colors[index++] = value.second.get_value<int>();

		Color light_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);
		double light_intensity = source.second.get<double>("intensity");


		LightSource* light = new LightSource(light_pos, light_color, light_intensity);
		readScene.lightSources.push_back(light);

	}

	//load objects
	std::cout<<"[SCENE] loading objects..."<<std::endl;
	for (pt::ptree::value_type &thing : root.get_child("objects"))
	{
		int index;
		std::string oType = thing.second.get<std::string>("type");

		if (oType == "plane")
		{
			std::cout<<"[SCENE :: OBJECT] loading plane..."<<std::endl;
			arma::colvec3 normal; index = 0;
			for (pt::ptree::value_type &value : thing.second.get_child("normal"))
				normal[index++] = value.second.get_value<double>();

			double distance = thing.second.get<double>("distance");

			pt::ptree &material = thing.second.get_child("material");

			if (material.find("r_Index") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading dielectric..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);
				double refractiveIndex = material.get<double>("r_Index");

				Material mat(material_color, refractiveIndex);

				Plane* object = new Plane(normal,distance,mat);

				readScene.objects.push_back(object);
			}
			else if (material.find("texture") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading uv..."<<std::endl;
				std::string texturePath = material.get<std::string>("texture");
				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(texturePath, reflectivness, gloss);

				Plane* object = new Plane(normal,distance,mat);
				readScene.objects.push_back(object);
			}
			else
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading albedo..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);

				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(material_color, reflectivness, gloss);

				Plane* object = new Plane(normal,distance,mat);
				readScene.objects.push_back(object);
			}
			continue;
		}
		else if (oType == "sphere")
		{
			std::cout<<"[SCENE :: OBJECT] loading sphere..."<<std::endl;
			arma::colvec3 position; index = 0;
			for (pt::ptree::value_type &value : thing.second.get_child("position"))
				position[index++] = value.second.get_value<double>();

			double radius = thing.second.get<double>("radius");

			pt::ptree &material = thing.second.get_child("material");

			if (material.find("r_Index") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading dielectric..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);
				double refractiveIndex = material.get<double>("r_Index");

				Material mat(material_color, refractiveIndex);

				Sphere* object = new Sphere(position,radius,mat);
				readScene.objects.push_back(object);
			}
			else if (material.find("texture") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading uv..."<<std::endl;
				std::string texturePath = material.get<std::string>("texture");
				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(texturePath, reflectivness, gloss);

				Sphere* object = new Sphere(position,radius,mat);
				readScene.objects.push_back(object);
			}
			else
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading albedo..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);

				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(material_color, reflectivness, gloss);


				Sphere* object = new Sphere(position,radius,mat);
				readScene.objects.push_back(object);
			}
			continue;
		}
		else if (oType == "mesh")
		{
			std::cout<<"[SCENE :: OBJECT] loading 3D Mesh..."<<std::endl;
			std::string objPath = thing.second.get<std::string>("path");

			arma::colvec3 position; index = 0;
			for (pt::ptree::value_type &value : thing.second.get_child("position"))
				position[index++] = value.second.get_value<double>();

			arma::colvec3 orientation; index = 0;
			for (pt::ptree::value_type &value : thing.second.get_child("orientation"))
				orientation[index++] = value.second.get_value<double>();

			double scale = thing.second.get<double>("size");

			pt::ptree &material = thing.second.get_child("material");

			if (material.find("r_Index") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading dielectric..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);
				double refractiveIndex = material.get<double>("r_Index");

				Material mat(material_color, refractiveIndex);

				Mesh* object = new Mesh(objPath,position,orientation,scale,mat);
				readScene.objects.push_back(object);
			}
			else if (material.find("texture") != material.not_found())
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading uv..."<<std::endl;
				std::string texturePath = material.get<std::string>("texture");
				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(texturePath, reflectivness, gloss);

				Mesh* object = new Mesh(objPath,position,orientation,scale,mat);
				readScene.objects.push_back(object);
			}
			else
			{
				std::cout<<"[SCENE :: OBJECT :: MATERIAL] loading albedo..."<<std::endl;
				arma::colvec3 int_colors; index = 0;
				for (pt::ptree::value_type &value : material.get_child("color"))
					int_colors[index++] = value.second.get_value<int>();

				Color material_color (int_colors[0]/255.0, int_colors[1]/255.0, int_colors[2]/255.0);

				double reflectivness = material.get<double>("reflectivness");
				double gloss = material.get<double>("gloss");

				Material mat(material_color, reflectivness, gloss);

				Mesh* object = new Mesh(objPath,position,orientation,scale,mat);
				readScene.objects.push_back(object);
			}
			continue;
		}
	}

	std::cout<<"[SCENE] loaded!"<<std::endl;

	return readScene;
}