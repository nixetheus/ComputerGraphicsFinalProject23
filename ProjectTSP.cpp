// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"
#include "TextMaker.hpp"

 std::vector<SingleText> demoText = {
	{1, {"The Stanley Parable - Demo", "", "", ""}, 0, 0},
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject1 {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct GlobalUniformBufferObject2 {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

struct GlobalUniformBufferObject3 {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

// FINAL PROJECT

// Global Light
struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

// Spot Light
struct SpotUniformBufferObject {
	alignas(16) glm::vec3 lightPos;
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};

// Mesh Data
struct MeshUniformBlock {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};


class ProjectTSP;
//void GameLogic(ProjectTSP *A, float Ar, glm::mat4 &ViewPrj, glm::mat4 &World); ?

// MAIN ! 
class ProjectTSP : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLSpotLight, DSLMesh, DSLProcedural;

	// Pipelines [Shader couples]
	Pipeline PMesh, PProcedural;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model MClock;  // TODO
	Texture TClock;  // TODO
	DescriptorSet DSGubo, DSSpotLight, DSClock;  // TODO

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	float Ar;
	glm::mat4 ViewPrj;
	glm::vec3 Pos = glm::vec3(0,0,15);
	glm::vec3 cameraPos;
	float Yaw = glm::radians(0.0f);
	float Pitch = glm::radians(0.0f);
	float Roll = glm::radians(0.0f);

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Project The Stanley Parable";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.6f, 0.8f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 20;
		texturesInPool = 20;
		setsInPool = 20;
		
		Ar = 4.0f / 3.0f;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]

		// FINAL PROJECT
		DSLGubo.init(this, {
			// this array contains the binding:
			// first  element : the binding number
			// second element : the type of element (buffer or texture)
			// third  element : the pipeline stage where it will be used
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT}            // Gubo
			});

		DSLSpotLight.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT}            // Spot Light
			});

		DSLMesh.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},          // Mesh Ubo
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // Mesh Texture
			{2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}   // Emission Texture
			});

		// TODO
		DSLProcedural.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},          // Mesh Ubo
			});
		

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		// FINAL PROJECT
		PMesh.init(this, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLMesh });
		//PProcedural.init(this, "shaders/ProceduralVert.spv", "shaders/ProceduralFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLProcedural });

		// Models, textures and Descriptors (values assigned to the uniforms)
		MClock.init(this, "models/Room/Objects/Clock.obj");

		TClock.init(this, "textures/TexturesCity.png");
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		// PProcedural.create();

		DSGubo.init(this, &DSLGubo, {
			{0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});

		DSSpotLight.init(this, &DSLSpotLight, {
			{0, UNIFORM, sizeof(SpotUniformBufferObject), nullptr}
			});

		DSClock.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
			{1, TEXTURE, 0, &TClock},
			{2, TEXTURE, 0, &TClock},
		});

		// TODO
	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {

		PMesh.cleanup();
		//PProcedural.cleanup();

		DSGubo.cleanup();
		DSSpotLight.cleanup();
		DSClock.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {

		TClock.cleanup();

		MClock.cleanup();

		DSLGubo.cleanup();
		DSLSpotLight.cleanup();
		DSLMesh.cleanup();
		DSLProcedural.cleanup();

		PMesh.destroy();
		//PProcedural.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSSpotLight.bind(commandBuffer, PMesh, 1, currentImage);

		//DSGubo.bind(commandBuffer, PProcedural, 0, currentImage);
		//DSSpotLight.bind(commandBuffer, PProcedural, 1, currentImage);

		PMesh.bind(commandBuffer);

		MClock.bind(commandBuffer);
		DSClock.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MClock.indices.size()), 1, 0, 0, 0);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;
//std::cout << xpos << " " << ypos << " " << m_dx << " " << m_dy << "\n";

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				std::cout << "Scene : " << currScene << "\n";
				//Reset position when pressing SPACE
				//Pos = glm::vec3(0,0,currScene == 0 ? 15 : 0);
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		
		GameLogic();
		
//printMat4("ViewPrj", ViewPrj);
//printMat4("WM", WM);
		
		UniformBufferObject ubo{};								
		// Here is where you actually update your uniforms

		// updates global uniforms
		const float FOVy = glm::radians(90.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		float CamH, CamRadius, CamPitch, CamYaw;
		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
		CamH += m.z * movSpeed * deltaT;
		CamRadius -= m.x * movSpeed * deltaT;
		CamPitch -= r.x * rotSpeed * deltaT;
		CamYaw += r.y * rotSpeed * deltaT;

		GlobalUniformBufferObject gubo{};
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;
		glm::vec3 camTarget = glm::vec3(0, CamH, 0);
		glm::vec3 camPos = camTarget +
			CamRadius * glm::vec3(cos(CamPitch) * sin(CamYaw),
				sin(CamPitch),
				cos(CamPitch) * cos(CamYaw));
		glm::mat4 View = glm::lookAt(camPos, camTarget, glm::vec3(0, 1, 0));

		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;

		SpotUniformBufferObject spot{};
		spot.lightDir = glm::normalize(glm::vec3(1, 2, 3));
		spot.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		spot.lightPos = glm::vec3(0.1f);
		spot.eyePos = camPos;

		MeshUniformBlock uboClock{};
		glm::mat4 World = glm::mat4(1);
		uboClock.amb = 1.0f; uboClock.gamma = 180.0f; uboClock.sColor = glm::vec3(1.0f);
		uboClock.mvpMat = Prj * View * World;
		uboClock.mMat = World;
		uboClock.nMat = glm::inverse(glm::transpose(World));

		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);
		DSSpotLight.map(currentImage, &spot, sizeof(spot), 0);
		DSClock.map(currentImage, &uboClock, sizeof(uboClock), 0);
		
	}
	
	void GameLogic() {

		/////////////////////////// PARAMETERS ///////////////////////////
		// Camera FOV-y, Near Plane and Far Plane
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;
		// Camera target height and distance
		const float camHeight = 1.25;
		const float camDist = 1.5;
		// Height limits
		const float highPos = 3.0f;
		const float lowPos = 1.0f;
		// Camera Pitch limits
		const float minPitch = glm::radians(-60.0f);
		const float maxPitch = glm::radians(40.0f);
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(100.0f);
		const float MOVE_SPEED = 3.0f;

		////////////////// Game Logic implementation //////////////////

		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m, r;
		r = glm::vec3(0.0f);
		m = glm::vec3(0.0f);
		//m = glm::vec3(0.0f);
		// Initialize Camera Height to highPos
		Pos.y = Pos.y == 0.0f ? highPos : Pos.y;

		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
				
		
		/////////////////////////// WORLD ////////////////////////////

		glm::mat4 World = glm::mat4(1);



		/////////////////////////// CAMERA ///////////////////////////

		ViewPrj = glm::mat4(1);

		// Rotate camera (no roll permitted, useful in this project)
		Yaw = Yaw - ROT_SPEED * deltaT * r.y;
		Pitch = Pitch + ROT_SPEED * deltaT * r.x;
		//CamPitch -= r.x * rotSpeed * deltaT;
		Pitch = Pitch < minPitch ? minPitch :
			(Pitch > maxPitch ? maxPitch : Pitch);

		// Set moving direction, changing them with the camera (yaw) pointing direction
		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), Yaw, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);

		// Move position 
		Pos = Pos + MOVE_SPEED * m.x * ux * deltaT;
		Pos = Pos + MOVE_SPEED * m.z * uz * deltaT;
		// Set height position, low after F pressed, high after R pressed (F and R set m.y value to -1/+1, maybe change to SHIFT)
		Pos.y = m.y == -1.0f ? lowPos : 
					m.y == 1.0f ? highPos : Pos.y;


		std::cout << "X=" << Pos.x << "    Y="<< Pos.y << "    Z="<< Pos.z << std::endl;

		//std::cout << Pos.x << ", " << Pos.y << ", " << Pos.z << ", " << Yaw << ", " << Pitch << ", " << Roll << "\n";

		// Final world matrix computaiton
		World = glm::rotate(glm::mat4(1.0f), -Roll, glm::vec3(0, 0, 1)) *
				glm::rotate(glm::mat4(1.0f), -Pitch, glm::vec3(1, 0, 0)) *
				glm::rotate(glm::mat4(1.0f), -Yaw, glm::vec3(0, 1, 0)) *
				glm::translate(glm::mat4(1), -Pos);
		
		// TO DO -> Posizione oggetti sbagliata

		// Projection
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;	// Given by the different Vulkan coordinate convention compared to GLM

		ViewPrj = Prj * World;
	}
};


// This is the main: probably you do not need to touch this!
int main() {
    ProjectTSP app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}