#include "Rasterization_Kernel.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
using namespace std;
using namespace glm;
const int max_id = 16777215;

void errorCallback(
		int error,
		const char * description
) {
	stringstream msg;
	msg << "GLFW Error Code: " << error << "\n" <<
			"GLFW Error Description: " << description << "\n";
    cout << msg.str();
}

void window_init(bool visible = false)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(visible)
		glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	else
    	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
}

void init() {
	// Render only the front face of geometry.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Setup depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_CLAMP);

	glClearDepth(1.0f);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void Rasterization::add_patch(float *vertexs, bool *vert_mask, float *vert_colors, int *index, int vertex_len, int index_len, int mode)
{
    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

	// pass vertexs coordinates to GPU
    GLuint vertex_vbo;
	glGenBuffers(1, &vertex_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_len * 3, vertexs, GL_STATIC_DRAW);
	GLuint shader_pos_bind = glGetAttribLocation(shader_ID, (const GLchar *)"vertexPosition_modelspace");
	glVertexAttribPointer(shader_pos_bind, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(shader_pos_bind);

	float *vertex_colors;
	vertex_colors = new float[vertex_len  * 3];
	if(mode == 0)
	{
		for(int i = 0; i < vertex_len; i++)
		{
			int patchID = vert_mask[i]? accumulative_patchID++:max_id;
			vertex_colors[i * 3] = float(patchID&0xff) / 255.0f;
			vertex_colors[i * 3 + 1]= float((patchID>>8)&0xff) / 255.0f;
			vertex_colors[i * 3 + 2]= float((patchID>>16)&0xff) / 255.0f;
		}
	}
	// pass vertexs colors to GPU
	GLuint color_vbo;
    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);	
	if(mode == 0) glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_len  * 3, vertex_colors, GL_STATIC_DRAW);
	else glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_len  * 3, vert_colors, GL_STATIC_DRAW);
	GLuint shader_color_bind = glGetAttribLocation(shader_ID, (const GLchar *)"color");
    glVertexAttribPointer(shader_color_bind, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(shader_color_bind);

	//for(int i = 0; i < index_len; i++) cout << vertexs[index[i] * 3] << ' ' << vertexs[index[i] + 1] << ' ' << vertexs[index[i] + 2] << endl;
	// Specify the indexing to draw triangles
	GLuint index_vbo;
	glGenBuffers(1, &index_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * index_len, index, GL_STATIC_DRAW);

	// Clear up
    buffers.push_back(vaoID);
	counts.push_back(index_len);
    glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void Rasterization::rewrite_color(int vao_no, float *colors, int len)
{
	glBindVertexArray(buffers[vao_no]);

	GLuint color_vbo;
    glGenBuffers(1, &color_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);	
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * len  * 3, colors, GL_STATIC_DRAW);
	GLuint shader_color_bind = glGetAttribLocation(shader_ID, (const GLchar *)"color");
    glVertexAttribPointer(shader_color_bind, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(shader_color_bind);

    glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

Rasterization::Rasterization()
{
	glfwSetErrorCallback(errorCallback);

    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Call to glfwInit() failed.\n");
        std::abort();
    }
	m_window = NULL;
    // window_init();

    // m_window = glfwCreateWindow(img_size, img_size, "", NULL, NULL);
    // if (m_window == NULL) {
    //     glfwTerminate();
    //     fprintf(stderr, "Call to glfwCreateWindow failed.\n");
    //     std::abort();
    // }

    // glfwMakeContextCurrent(m_window);
	// gl3wInit();

    // glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // // Clear error buffer.
    // while(glGetError() != GL_NO_ERROR);

	// shader_ID = LoadShaders( "Radiosity/vertex_shader.glsl", "Radiosity/fragment_shader.glsl" );
	// glUseProgram(shader_ID);

	//init();
}

void Rasterization::generate_window(bool visible, const string vertex_shader, const string fragment_shader)
{
	if(m_window != NULL)
	{
		glfwDestroyWindow(m_window);
	}
	window_init(visible);

    m_window = glfwCreateWindow(img_size, img_size, "", NULL, NULL);
    if (m_window == NULL) {
        glfwTerminate();
        fprintf(stderr, "Call to glfwCreateWindow failed.\n");
        std::abort();
    }

    glfwMakeContextCurrent(m_window);
	gl3wInit();

    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);

    // Clear error buffer.
    while(glGetError() != GL_NO_ERROR);

	shader_ID = LoadShaders( vertex_shader.c_str(), fragment_shader.c_str() );
	glUseProgram(shader_ID);
}

u_char* Rasterization::patch_form_factor(vec3 patch_origin, vec3 view_dir, vec3 up)
{
	mat4 projection = perspective(radians(90.0f), 1.0f, 0.1f, 100.0f);
	mat4 view = lookAt(patch_origin, patch_origin + view_dir, up);
	mat4 mvp = projection * view;

	init();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_ID);
	GLuint MatrixID = glGetUniformLocation(shader_ID, "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	for(int i = 0; i < buffers.size(); i++)
	{
		glBindVertexArray(buffers[i]);
		glDrawElements(GL_TRIANGLES, counts[i], GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
	glfwSwapBuffers(m_window);

	u_char *image = new u_char[img_size * img_size * 3];
	glReadPixels(0, 0, img_size, img_size, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	return image;
}

void Rasterization::display()
{
	// Set pipeline transformation matrix
	mat4 projection = perspective(radians(100.0f), 1.0f, 0.1f, 100.0f);
	mat4 view = lookAt(vec3(0,5,4), vec3(0,5,3), vec3(0,1,0));
	mat4 mvp = projection * view; 
	// display window main loop, visualize for debugging purpose
	 try {
         init();
		 //glClearColor(0, 0, 1.0, 1.0);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         glUseProgram(shader_ID);

			GLuint MatrixID = glGetUniformLocation(shader_ID, "MVP");
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


         while (glfwGetKey(m_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(m_window)) {   
            for(int i = 0; i < buffers.size(); i++)
            {
                glBindVertexArray(buffers[i]);
				glDrawElements(GL_TRIANGLES, counts[i], GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
            }

			glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
        
    } catch (const  std::exception & e) {
        std::cerr << "Exception Thrown: ";
        std::cerr << e.what() << endl;
    } catch (...) {
        std::cerr << "Uncaught exception thrown!  Terminating Program." << endl;
    }

    glfwDestroyWindow(m_window);
}

Rasterization::~Rasterization()
{
}