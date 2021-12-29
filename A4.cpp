// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "Ray.hpp"
#include <algorithm>
#include <math.h>
#include <chrono>
#include "Pool.hpp"
#include <functional>

#define my_pi pi<float>()

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
}

Render::~Render()
{
}

void Render::set_parameters()
{
	max_hit = 5;			// how many extra rays generate including the first hit

	silouette = false;		// render feature lines on objects
	sil_ring_num = 1;		// how many rings of extra rays inside the view cone
	sil_ring_size = 4;		// how many rays in each ring
	
	shading_al = 0;			// which shade algorith to use: 0 - blinn phong, 1 - gooch
	multi_thread = true;	// whether using multi-threding
	quality = 16;			// how many extra rays generate when super sampling
}

void Render::run()
{
	print_info();
	size_t h_pix = Graph.height();
	size_t w_pix = Graph.width();
	
	float y_len = 10 * tan(radians(Fovy / 2));					// half height of view plane
	float unit_len = y_len * 2 / h_pix;							// side length of each pix
	vec4 bot_left = Eye - 10 * cam_frame.z 
					- cam_frame.x * unit_len * w_pix / 2 
					- cam_frame.y * y_len;						// the bottom left of view plane

	set_parameters();

	if(silouette)
	{
		sil_sample_size = sil_ring_num * sil_ring_size;			// number of all rays in each view cone
		sil_ring_rad = unit_len;								// the radius of view cone
		unit_r = sil_ring_rad / sil_ring_num;
		unit_ang = 2 * my_pi / sil_ring_size;
	}

	Pool thread_pool;
	if(multi_thread)
	{
		int thread_num = std::thread::hardware_concurrency();
		thread_pool.init(thread_num);							// creating thread pool
	}

	auto start = std::chrono::high_resolution_clock::now();
	std::cout << "Start rendering: " << std::endl;

	for (uint y = 0; y < h_pix; ++y) {							// for each pixel
		for (uint x = 0; x < w_pix; ++x) {
			vec4 center = bot_left 								// center of the pixel
						  + cam_frame.x * (unit_len * (0.5 + x)) 
						  + cam_frame.y * (unit_len * (0.5 + y));

			if(multi_thread) 
				thread_pool.Add_Task([=](){shade_pixel(x, y, pix_operation(unit_len / 5, center));});
			else
				shade_pixel(x, y, pix_operation(unit_len / 2, center));
		}
	}

	thread_pool.wait();
	std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start;
	std::cout << "Time take: " << diff.count() * 1000 << " ms\n";
}

vec3 Render::pix_operation(float radius, vec4 center)
{
	vec3 pix_color = vec3();

	// the color of the pixel center
	Ray v_ray = Ray(Eye, center, false);
	pix_color = recursive_ray_color(v_ray, 0);

	for(int i = 0; i < quality; i++)
	{
		double r = sqrt(abs(distribution(generator))) * radius;	// random distance from cone center - normal distribution
		double ang = 2 * my_pi * uniform(generator);			// random angle from x-axis - uniform distribution

		v_ray = Ray(Eye, center + r * sin(ang) * cam_frame.y + r * cos(ang) * cam_frame.x, false);
		pix_color += recursive_ray_color(v_ray, 0);
	}

	pix_color /= quality + 1;
	return pix_color;
}

bool refract(vec4 d, vec4 n, double n1, double n2, vec4& t)
{
	double coeff = 1 - (1 - std::pow(dot(d, n), 2)) * std::pow(n1, 2) / std::pow(n2, 2);
	if(coeff < 0) return false;

	vec4 b = (d - n * dot(d, n)) * n1 / n2;
	t = normalize(b - std::sqrt(coeff) * n);
	return true;
}

vec3 Render::recursive_ray_color(Ray ray, int hit_count)
{
	if(hit_count == max_hit) return vec3(0,0,0);

	Record record;
	ray.Type == RayType::ViewRay? Root->hit(ray, 0, UINT_MAX, record):Root->hit(ray, 0.1, UINT_MAX, record);

	vec3 color;
	if(silouette && cal_silouette(ray, record, color)) return color;

	if(record.hit)
	{
		if(record.material->type == 0)										// Specular material
		{
			PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
			vec4 r = ray.Direction - 2 * dot(ray.Direction, record.normal) * record.normal;
			return cal_color(record, - ray.Direction, pm) + 
				   pm->spectular() * recursive_ray_color(Ray(record.intersection, r, true, RayType::Reflect), hit_count + 1);
		}
		if(record.material->type == 2)										// Diffuse material
		{
			PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
			vec4 r = ray.Direction - 2 * dot(ray.Direction, record.normal) * record.normal;
			return cal_color(record, - ray.Direction, pm);
		}
		if(record.material->type == 1)										// Dielectric
		{
			Dielectric *dm = static_cast<Dielectric *>(record.material);
			color = cal_color(record, - ray.Direction, dm);
			vec3 k;
			vec4 refract_dir, reflect_dir = ray.Direction - 2 * dot(ray.Direction, record.normal) * record.normal;
			float c;														// cos

			if(dot(ray.Direction, record.normal) < 0)						// from air to the material
			{
				refract(ray.Direction, record.normal, 1.0, dm->refractive(), refract_dir);
				c = dot(-ray.Direction, record.normal);
				k = vec3(1.0, 1.0, 1.0);
			}
			else															// from the material to air
			{
				double dis = distance(record.intersection, ray.Origin);		// distance from last intersection
				k = exp(-dm->kr() * dis);									// intensity attenuation
				if(refract(ray.Direction, - record.normal, dm->refractive(), 1.0, refract_dir))
					c = dot(refract_dir, record.normal);
				else
					return k * recursive_ray_color(Ray(record.intersection, reflect_dir), hit_count + 1) + color;
			}

			float r1 = dm->reflectance() + (1 - dm->reflectance()) * pow(1 - c, 5);
			return k * (r1 * (recursive_ray_color(Ray(record.intersection, reflect_dir, true, RayType::Reflect), hit_count + 1) + color) + (1 - r1) * recursive_ray_color(Ray(record.intersection, refract_dir, true, RayType::Reflect), hit_count + 1));
		}
		if(record.material->type == 3)
		{
			PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
			vec4 r = ray.Direction - 2 * dot(ray.Direction, record.normal) * record.normal;
			return cal_color(record, - ray.Direction, pm) + 
				   pm->spectular() * recursive_ray_color(Ray(record.intersection, r, true, RayType::Reflect), hit_count + 1);
		}
	}
	else if(hit_count == 0)
		return vec3(0.9, 0.8, 0.8);

	return vec3(0,0,0);
}

bool Render::cal_silouette(Ray ray, Record record, vec3 &pix_color)		// need fix to adapt to recursive check
{
	return false;

	// int diff = 0;
	// float x_coeff, y_coeff;
	// Ray sil_ray;

	// for(int i = 0; i < sil_ring_size; i++)
	// {
	// 	x_coeff = cos(unit_ang * i);
	// 	y_coeff = sin(unit_ang * i);

	// 	for(int j = 0; j < sil_ring_num; j++)
	// 	{
	// 		vec4 sil_dir = center +
	// 					   (j + 1) * unit_r * x_coeff * cam_frame.x +
	// 					   (j + 1) * unit_r * y_coeff * cam_frame.y;
	// 		sil_ray = Ray(v_ray.Origin, sil_dir, false);
	// 		Record sil_record;
	// 		Root->hit(sil_ray, 0, UINT_MAX, sil_record);

	// 		if(sil_record.hit_node != center_hit || dot(center_normal, sil_record.normal) <= 0.7)
	// 		 	diff++;
	// 	}
	// }
	
	// float coeff = abs(2 * diff - sil_sample_size) / sil_sample_size;
	// if(coeff < 1) pix_color = vec3(0.5, 0.5, 0.5) * coeff;
	// return false;
}

vec3 Render::cal_color(Record record, vec4 view, PhongMaterial* pm)
{
	vec3 pix_color;
	for(Light *l: Lights)
	{
		Record shadow_record;
		vec4 l_pos = l->random_pos();
		Ray shadow_ray = Ray(record.intersection, l_pos, false, RayType::ShadowRay);
		Root->hit(shadow_ray, 0.1, distance(record.intersection, l_pos), shadow_record);

		vec3 diffuse = (pm->type == 3)? pm->diffuse(record.texture_x, record.texture_y)/255:pm->diffuse();

		pix_color += diffuse * Ambient;
		if(shadow_record.hit) continue;

		vec4 light = normalize(l_pos - record.intersection);
		pix_color += diffuse * l->colour * std::max(float(0), dot(record.normal, light));

		vec4 half = normalize(light + view);
		pix_color += pm->spectular() * l->colour * std::pow(std::max(float(0), dot(record.normal, half)), pm->shininess());	
	}
	return pix_color;
}

vec3 Render::gooch_color(Record record, vec4 view, vec3 &cumulative_km)
{
	vec3 pix_color;
	for(Light *l: Lights)
	{
		PhongMaterial *pm = static_cast<PhongMaterial *>(record.material);
		vec3 k_cool = vec3(0.1, 0.0, 0.6) + 0.3 * pm->diffuse();
		vec3 k_warm = vec3(0.3, 0.2, 0.0) + 0.6 * pm->diffuse();
		cumulative_km = cumulative_km * pm->spectular();

		vec4 light = normalize(l->position - record.intersection);
		float coeff = (1.0f + dot(record.normal, light)) / 2.0f;
		pix_color = coeff * k_warm + (1.0f - coeff) * k_cool;

		vec4 half = normalize(light + view);
		pix_color += pm->spectular() * l->colour * std::pow(std::max(float(0), dot(record.normal, half)), pm->shininess());	
	}
	return pix_color;
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

void Render::shade_pixel(int x, int y, vec3 color)
{
	size_t h_pix = Graph.height();
	Graph(x, h_pix - 1 - y, 0) = (double)color[0];
	Graph(x, h_pix - 1 - y, 1) = (double)color[1];
	Graph(x, h_pix - 1 - y, 2) = (double)color[2];
}