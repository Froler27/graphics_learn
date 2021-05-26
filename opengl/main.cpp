#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

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
	//初始化GLFW
	glfwInit();
	//配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	//创建一个窗口对象,宽和高作为它的前两个参数,第三个参数表示这个窗口的名称（标题）
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//通知GLFW将我们窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);
	//需要函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数,当窗口被第一次显示的时候framebuffer_size_callback也会被调用
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//创建一个顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//把着色器源码附加到着色器对象上
	//把要编译的着色器对象作为第一个参数
	//第二参数指定了传递的源码字符串数量
	//第三个参数是顶点着色器真正的源码
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//编译顶点着色器
	glCompileShader(vertexShader);
	//编译错误处理
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//创建一个片元着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用
	unsigned int shaderProgram = glCreateProgram();
	//把之前编译的着色器附加到程序对象上，然后用glLinkProgram链接它们
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//检测链接着色器程序是否失败
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//在把着色器对象链接到程序对象以后，记得删除着色器对象，我们不再需要它们了
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	0.0f,  0.5f, 0.0f
	//};
	float vertices[] = {
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	unsigned int VBO;//缓冲ID
	glGenBuffers(1, &VBO);//生成一个缓冲对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//把新创建的缓冲绑定到GL_ARRAY_BUFFER目标,即设置缓冲类型为 顶点缓冲对象GL_ARRAY_BUFFER
	//把之前定义的顶点数据复制到缓冲的内存
	//glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数
	//它的第一个参数是目标缓冲的类型
	//第二个参数指定传输数据的大小(以字节为单位)
	//第三个参数是我们希望发送的实际数据。
	//第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
	//	GL_STATIC_DRAW ：数据不会或几乎不会改变。
	//	GL_DYNAMIC_DRAW：数据会被改变很多。
	//	GL_STREAM_DRAW ：数据每次绘制时都会改变。
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//第一个参数指定我们要配置的顶点属性
	//  还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性
	//  的位置值(Location)吗？它可以把顶点属性的位置值设置为0
	//  因为我们希望把数据传递到这一个顶点属性中,所以这里我们传入0。
	//第二个参数指定顶点属性的大小。
	//  顶点属性是一个vec3，它由3个值组成，所以大小是3。
	//第三个参数指定数据的类型
	//  这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
	//下个参数定义我们是否希望数据被标准化(Normalize)。
	//  如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是 - 1）
	//  到1之间。我们把它设置为GL_FALSE。
	//第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。
	//  由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。
	//  要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）
	//  我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）
	//最后一个参数的类型是void*，所以需要我们进行这个奇怪的强制类型转换
	//  它表示位置数据在缓冲中起始位置的偏移量(Offset)
	//  由于位置数据在数组的开头，所以这里是0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的
	glEnableVertexAttribArray(0);


	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);


	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	//简单的渲染循环
	while (!glfwWindowShouldClose(window))//检查GLFW是否被要求退出
	{
		//按esc键退出
		processInput(window);

		//渲染指令
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//设置清空屏幕所用的颜色
		glClear(GL_COLOR_BUFFER_BIT);//清除颜色缓冲

		//激活着色器程序程序
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//采用线框模式
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// glBindVertexArray(0); // no need to unbind it every time 
		

		//检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		//更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
		glfwPollEvents();
		//交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲）
		//它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
		glfwSwapBuffers(window);
	}
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	//std::cin.get();
	//释放/删除之前的分配的所有资源
	glfwTerminate();
	return 0;
}