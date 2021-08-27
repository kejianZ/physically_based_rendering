// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "Frame.hpp"
#include "Ray.hpp"
#include "PhongMaterial.hpp"
#include <algorithm>
#include <math.h>

Render::Render(SceneNode *root,
		   Image &image,
		   const glm::vec3 &eye,
		   const glm::vec3 &view,
		   const glm::vec3 &up,
		   double fovy,
		   const glm::vec3 &ambient,
		   const std::list<Light *> &lights):
		   Root(root), Graph(image), Eye(eye), View(view), Up(up), Fovy(fovy), Ambient(ambient), Lights(lights)
		   {}

Render::~Render()
{
}

void Render::run()
{
	print_info();
	size_t h_pix = Graph.height();
	size_t w_pix = Graph.width();

	
	Frame cam_frame = Frame(View, Up);
	std::cout << to_string(cam_frame.x) << std::endl
			  << to_string(cam_frame.y) << std::endl
			  << to_string(cam_frame.z) << std::endl;
	float y_len = 10 * tan(radians(Fovy / 2));				// half height of view plane
	float unit_len = y_len * 2 / h_pix;						// side length of each pix
	vec3 bot_left = Eye - 10 * cam_frame.z 
					- cam_frame.x * unit_len * w_pix / 2 
					- cam_frame.y * y_len;					// the bottom left of view plane

	for (uint y = 0; y < h_pix; ++y) {						// for each pixel
		for (uint x = 0; x < w_pix; ++x) {
			// calculate view ray
			Ray v_ray = Ray(Eye,
					bot_left 
					+ cam_frame.x * (unit_len * (0.5 + x)) 
					+ cam_frame.y * (unit_len * (0.5 + y)), false);

			Record record;
			Root->hit(v_ray, 0, UINT_MAX, record);
			if(record.hit)
			{
				vec3 pix_color = vec3();
				for(Light *l: Lights)
				{
					Record shadow_record;
					Ray shadow_ray = Ray(record.intersection, l->position, false, RayType::ShadowRay);
					Root->hit(shadow_ray, 0.1, UINT_MAX, shadow_record);
					pix_color += cal_color(record, l, - v_ray.Direction, shadow_record.hit);
				}
				shade_pixel(x, y, pix_color);
			}
			else
			{
				shade_pixel(x, y, vec3(0.8, 1.0, 0.8));
			}
		}
	}
}

void Render::print_info()
{
	std::cout << "Rendering(\n" <<
		  "\t" << *Root <<
          "\t" << "Image(width:" << Graph.width() << ", height:" << Graph.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(Eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(View) << std::endl <<
		  "\t" << "up:   " << glm::to_string(Up) << std::endl <<
		  "\t" << "fovy: " << Fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(Ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;
	for(const Light * light : Lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;
}

vec3 Render::cal_color(Record record, Light *l, vec3 view, bool shadow)
{
	PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
	vec3 color = pm->diffuse() * Ambient;
	if(shadow) return color;

	vec3 light = normalize(l->position - record.intersection);
	color += pm->diffuse() * l->colour * std::max(float(0), dot(record.normal, light));

	vec3 half = normalize(light + view);
	color += pm->spectular() * l->colour * std::pow(std::max(float(0), dot(record.normal, half)), pm->reflectness());
	return color;
}

void Render::shade_pixel(int x, int y, vec3 color)
{
	size_t h_pix = Graph.height();
	Graph(x, h_pix - 1 - y, 0) = (double)color[0];
	Graph(x, h_pix - 1 - y, 1) = (double)color[1];
	Graph(x, h_pix - 1 - y, 2) = (double)color[2];
}