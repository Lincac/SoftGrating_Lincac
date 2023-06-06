#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>

#include<iostream>

#include"GPU.h"
#include"LoadModel.h"
#include"Camera.h"

#define scr_width 800
#define scr_height 600

bool mouseClick = false;
float lastx = 0, lasty = 0;
float currentx = 0, currenty = 0;

Camera camera(vec3(0.0, 0.0, 3.0));

vec3 lightDir(1.0);
vec3 lightColor(5);
vec3 Ambient(0.03);

void show_viewer(std::shared_ptr<Texture> tex);
void processInput(GLFWwindow* window);
void mouse_call_back(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int main(int argc,char **argv) {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "SoftGrating_Lincac", NULL, NULL);
	if (window == NULL) {
		std::cerr << "error to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_call_back);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "error to load glad" << std::endl;
		glfwTerminate();
		return -1;
	}

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	std::shared_ptr<Model> model = std::make_shared<Model>("resource/model/diablo3_pose/diablo3_pose.obj");
	std::shared_ptr<CPU> _CPU = std::make_shared<CPU>();
	_CPU->vertex = model->vertex;
	_CPU->Vindex = model->Vindex;
	_CPU->normal = model->normal;
	_CPU->Nindex = model->Nindex;
	_CPU->texcoord = model->texcoord;
	_CPU->Tindex = model->Tindex;

	mat4 _model(1);
	mat4 _view = camera.get_viewMatrix();
	mat4 _projection = perspective(camera.Zoom, float(scr_width) / float(scr_height), 0.1f, 100.0f);

	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	shader->mvp = _projection * _view * _model;
	lightDir.normalize();
	shader->lightDir = lightDir;
	shader->lightCol = lightColor;
	shader->Ambient = Ambient;
	shader->camPos = camera.Position;
	shader->Gloss = 64.0;
	shader->albedo = model->Albedo;
	shader->normal = model->Normal;
	shader->specular = model->Specular;

	std::shared_ptr<GPU> _GPU = std::make_shared<GPU>();
	_GPU->set_CPU_Data(_CPU);
	_GPU->set_Shader(shader);

	std::shared_ptr<FrameBuffer> framebuffer = std::make_shared<FrameBuffer>();
	std::shared_ptr<Texture> colorBuffer = std::make_shared<Texture>();
	colorBuffer->generate_Texture(scr_width, scr_height, RGB);
	framebuffer->add_ColorBuffer(colorBuffer);
	std::shared_ptr<Texture> depthBuffer = std::make_shared<Texture>();
	depthBuffer->generate_Texture(scr_width, scr_height, RED,1);
	framebuffer->add_DepthBuffer(depthBuffer);
	_GPU->set_BindFramebuffer(framebuffer);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT);

		_view = camera.get_viewMatrix();
		_projection = perspective(camera.Zoom, float(scr_width) / float(scr_height), 0.1f, 100.0f);
		_GPU->get_Shader()->mvp = _projection * _view * _model;
		_GPU->get_Shader()->camPos = camera.Position;

		// 渲染场景
		_GPU->excute();

		show_viewer(_GPU->get_ColorBuffer());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_None);
		ImGui::SetNextWindowSize(ImVec2(300,400), ImGuiCond_None);
		{
			ImGui::Begin("Operate Object", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			ImGui::Text("Frame rate : %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

		// 数据清空
		_GPU->reset();
	}
	glfwTerminate();

	return 0;
}

unsigned int texture;
void show_viewer(std::shared_ptr<Texture> tex) {
	if (texture == 0) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->get_width(), tex->get_height(), 0, GL_RGB, GL_FLOAT, tex->get_data());
		glBindTexture(GL_TEXTURE_2D, 0);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, scr_width, 0, scr_height, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->get_width(), tex->get_height(), 0, GL_RGB, GL_FLOAT, tex->get_data());
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(float(scr_width), 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(float(scr_width), float(scr_height));
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, float(scr_height));
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void mouse_call_back(GLFWwindow* window, double xpos, double ypos) {

	currentx = scr_width - static_cast<float>(xpos);
	currenty = scr_height - static_cast<float>(ypos);

	if (mouseClick)
	{
		float deltax = currentx - lastx;
		float deltay = currenty - lasty;

		camera.ProcessMouseButton(deltax,deltay);
	}
	lastx = currentx;
	lasty = currenty;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
		camera.ProcessMouseScroll(float(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, scr_width, scr_height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouseClick = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouseClick = false;
}