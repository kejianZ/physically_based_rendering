// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "Ray.hpp"
#include "PhongMaterial.hpp"
#include <algorithm>
#include <math.h>
#include <random>

Render::Render(SceneNode *root,
		   Image &image,
		   glm::vec3 &eye,
		   glm::vec3 &view,
		   glm::vec3 &up,
		   double fovy,
		   const glm::vec3 &ambient,
		   const std::list<Light *> &lights):
		   Root(root), Graph(image), Fovy(fovy), Ambient(ambient), Lights(lights)
{
	Eye = vec4(eye, 1);
	View = vec4(view, 0);
	Up = vec4(up, 0);
	cam_frame = Frame(View, Up);
	max_hit = 2;
}

Render::~Render()
{
}

void Render::run()
{
	print_info();
	size_t h_pix = Graph.height();
	size_t w_pix = Graph.width();
	
	float y_len = 10 * tan(radians(Fovy / 2));				// half height of view plane
	float unit_len = y_len * 2 / h_pix;						// side length of each pix
	vec4 bot_left = Eye - 10 * cam_frame.z 
					- cam_frame.x * unit_len * w_pix / 2 
					- cam_frame.y * y_len;					// the bottom left of view plane

	for (uint y = 0; y < h_pix; ++y) {						// for each pixel
		for (uint x = 0; x < w_pix; ++x) {
			// center of the pixel
			vec4 center = bot_left 
						  + cam_frame.x * (unit_len * (0.5 + x)) 
						  + cam_frame.y * (unit_len * (0.5 + y));

			shade_pixel(x, y, pix_operation(unit_len / 2, center, 2));
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

vec3 Render::cal_color(Record record, vec4 view, vec3 &cumulative_km)
{
	vec3 pix_color;
	for(Light *l: Lights)
	{
		Record shadow_record;
		Ray shadow_ray = Ray(record.intersection, l->position, false, RayType::ShadowRay);
		Root->hit(shadow_ray, 0.1, UINT_MAX, shadow_record);

		PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
		pix_color += pm->diffuse() * Ambient;
		cumulative_km = cumulative_km * pm->spectular();
		if(shadow_record.hit) continue;

		vec4 light = normalize(l->position - record.intersection);
		pix_color += pm->diffuse() * l->colour * std::max(float(0), dot(record.normal, light));

		vec4 half = normalize(light + view);
		pix_color += pm->spectular() * l->colour * std::pow(std::max(float(0), dot(record.normal, half)), pm->reflectness());	
	}
	return pix_color;
}

void Render::shade_pixel(int x, int y, vec3 color)
{
	size_t h_pix = Graph.height();
	Graph(x, h_pix - 1 - y, 0) = (double)color[0];
	Graph(x, h_pix - 1 - y, 1) = (double)color[1];
	Graph(x, h_pix - 1 - y, 2) = (double)color[2];
}

vec3 Render::pix_operation(float radius, vec4 center, int quality)
{
	vec3 pix_color = vec3();

	// the color of the pixel center
	Ray v_ray = Ray(Eye, center, false);
	single_ray_color(v_ray, pix_color);

	std::default_random_engine generator(200);
  	std::normal_distribution<double> distribution(0.5,0.2);
	for(int i = 0; i < quality; i++)
	{
		double r = distribution(generator) * radius;
		double ang = 2 * pi<float>() * distribution(generator);

		v_ray = Ray(Eye, center + r * sin(ang) * cam_frame.y + r * cos(ang) * cam_frame.x, false);
		single_ray_color(v_ray, pix_color);
	}

	pix_color /= quality + 1;
	return pix_color;
}

void Render::single_ray_color(Ray v_ray, vec3& pix_color)
{
	Record record;
	Root->hit(v_ray, 0, UINT_MAX, record);

	if(record.hit)
	{
		vec3 cumulative_km = vec3(1.0, 1.0, 1.0);
		pix_color += cal_color(record, - v_ray.Direction, cumulative_km);
			
		for(int i = 1; i < max_hit; i++)
		{
			vec4 r = v_ray.Direction - 2 * dot(v_ray.Direction, record.normal) * record.normal;
			v_ray = Ray(record.intersection, r);
			record = Record();
			Root->hit(v_ray, 0.1, UINT_MAX, record);
			if(!record.hit) break;
			else pix_color += cumulative_km * cal_color(record, - v_ray.Direction, cumulative_km);					
		}
	}
	else
	{
		pix_color += Ambient;
	}
}