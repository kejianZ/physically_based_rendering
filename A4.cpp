// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A4.hpp"
#include "Frame.hpp"
#include "Ray.hpp"

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
	float y_len = 10 * tan(radians(Fovy / 2));				// half height of view plane
	float unit_len = y_len * 2 / h_pix;						// side length of each pix
	vec4 bot_left = vec4(Eye, 1.0) - 10 * cam_frame.z 
					- cam_frame.x * unit_len * w_pix / 2 
					- cam_frame.y * y_len;					// the bottom left of view plane

	Ray v_ray;
	for (uint y = 0; y < h_pix; ++y) {						// for each pixel
		for (uint x = 0; x < w_pix; ++x) {
			// calculate view ray
			v_ray = Ray(vec4(Eye, 1.0), 
					bot_left 
					+ cam_frame.x * (unit_len * (0.5 + 1.0)) 
					+ cam_frame.y * (unit_len * (0.5 + 2.0)));

			// if(root->hit(v_ray, rec)) 
			// 		color = rec.cal();
			//		shade(color)
			// else 
			//		shade(background)
			
			Graph(x, y, 0) = (double)1.0;
			Graph(x, y, 1) = (double)0.0;
			Graph(x, y, 2) = (double)1.0;
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

// Ray Render::view_ray(uint x, uint y)
// {

// }