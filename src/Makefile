all: raytracer

raytracer:main.o Entity/Camera.o Material/Material.o  Entity/Object.o Util/RayCaster.o  Renderer/Renderer.o Math/AffineTransform.o Renderer/Scene.o
	g++ main.o Camera.o Material.o  Object.o RayCaster.o  Renderer.o AffineTransform.o Scene.o -o raytracer -larmadillo -lpng -pthread

main.o: main.cpp
	g++ -c main.cpp

Entity/Camera.o: Entity/Camera.cpp
	g++ -c Entity/Camera.cpp

Material/Material.o: Material/Material.cpp
	g++ -c Material/Material.cpp

Entity/Object.o: Entity/Object.cpp
	g++ -c Entity/Object.cpp

Util/RayCaster.o: Util/RayCaster.cpp
	g++ -c Util/RayCaster.cpp

Renderer/Renderer.o: Renderer/Renderer.cpp
	g++ -c Renderer/Renderer.cpp

Math/AffineTransform.o: Math/AffineTransform.cpp
	g++ -c Math/AffineTransform.cpp

Renderer/Scene.o: Renderer/Scene.cpp
	g++ -c Renderer/Scene.cpp

