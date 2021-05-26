#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv)
{
	//��ʼ��GLFW
	glfwInit();
	//����GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	//����һ�����ڶ���,��͸���Ϊ����ǰ��������,������������ʾ������ڵ����ƣ����⣩
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//֪ͨGLFW�����Ǵ��ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);
	//��Ҫ����������GLFW����ϣ��ÿ�����ڵ�����С��ʱ������������,�����ڱ���һ����ʾ��ʱ��framebuffer_size_callbackҲ�ᱻ����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	//�򵥵���Ⱦѭ��
	while (!glfwWindowShouldClose(window))//���GLFW�Ƿ�Ҫ���˳�
	{
		//��esc���˳�
		processInput(window);

		//��Ⱦָ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//���������Ļ���õ���ɫ
		glClear(GL_COLOR_BUFFER_BIT);//�����ɫ����

		//�����û�д���ʲô�¼�������������롢����ƶ��ȣ�
		//���´���״̬�������ö�Ӧ�Ļص�����������ͨ���ص������ֶ����ã�
		glfwPollEvents();
		//������ɫ���壨����һ��������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壩
		//������һ�����б��������ƣ����ҽ�����Ϊ�����ʾ����Ļ��
		glfwSwapBuffers(window);
	}
	//std::cin.get();
	//�ͷ�/ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}