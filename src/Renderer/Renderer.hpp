#pragma once

#include <png++/image.hpp>
#include "Scene.hpp"

void RenderImage(png::image<png::rgb_pixel>& finalImage, int aadepth, Scene& scene, std::atomic<int>* progress);
void RenderChunk(png::image<png::rgb_pixel>& finalImage, bool splitDirection, int offset, int size, int aadepth, Scene scene, std::atomic<int>* progress);