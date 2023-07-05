// This has been adapted from the Vulkan tutorial

#include "Starter.hpp"

// The uniform buffer object used in this example
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

// The vertices data structures
// Mesh structure
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};


class ProjectTSP;
//void GameLogic(ProjectTSP *A, float Ar, glm::mat4 &ViewPrj, glm::mat4 &World); ?

// MAIN ! 
class ProjectTSP : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLSpotLight, DSLMesh, DSLProcedural;

	// Vertex formats
	VertexDescriptor VMesh;

	// Pipelines [Shader couples]
	Pipeline PMesh, PProcedural;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model<VertexMesh> MTSP, MClock, MPainting, MPaperTray, MSharpener, MComputer, MLamp;

	Texture TTSP, TClock, TPainting, TPaperTray, TSharpener, TComputer, TLamp;
	Texture TMeshEmit, TComputerEmit;

	DescriptorSet DSGubo, DSSpotLight, DSTSP, DSClock, DSPainting, DSPaperTray, DSSharpener, DSComputer, DSLamp;

	// C++ storage for uniform variables
	GlobalUniformBufferObject gubo;
	SpotUniformBufferObject uboSpot;
	MeshUniformBlock uboTSP, uboClock, uboPainting, uboPaperTray, uboSharpener, uboComputer, uboLamp;
	
	// TODO CHANGE POSITION OF THIS CODE, MIMIC A16
	// Other application parameters
	float Ar;
	glm::mat4 World;
	glm::mat4 ViewPrj;
	glm::vec3 cameraPos;
	glm::vec3 Pos = glm::vec3(0,0,15);
	float Yaw = glm::radians(0.0f);
	float Pitch = glm::radians(0.0f);
	float Roll = glm::radians(0.0f);
	// TODO
	int gameState;
	float CamH, CamRadius, CamPitch, CamYaw;

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

		// Vertex descriptors
		VMesh.init(this, {
			// this array contains the bindings
			// first  element : the binding number
			// second element : the stride of this binging
			// third  element : whether this parameter change per vertex or per instance
			//                  using the corresponding Vulkan constant
			{0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				// this array contains the location
				// first  element : the binding number
				// second element : the location number
				// third  element : the offset of this element in the memory record
				// fourth element : the data type of the element
				//                  using the corresponding Vulkan constant
				// fifth  elmenet : the size in byte of the element
				// sixth  element : a constant defining the element usage
				//                   POSITION - a vec3 with the position
				//                   NORMAL   - a vec3 with the normal vector
				//                   UV       - a vec2 with a UV coordinate
				//                   COLOR    - a vec4 with a RGBA color
				//                   TANGENT  - a vec4 with the tangent vector
				//                   OTHER    - anything else
				//
				// ***************** DOUBLE CHECK ********************
				//    That the Vertex data structure you use in the "offsetoff" and
				//	in the "sizeof" in the previous array, refers to the correct one,
				//	if you have more than one vertex format!
				// ***************************************************
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
					   sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
					   sizeof(glm::vec3), NORMAL},
				{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, UV),
					   sizeof(glm::vec2), UV}
			});
		

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on...
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLMesh });
		//PProcedural.init(this, &VTODO, "shaders/ProceduralVert.spv", "shaders/ProceduralFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLProcedural });

		// Models, textures and Descriptors (values assigned to the uniforms)
		MTSP.init(this, &VMesh, "models/Room/TheStanleyParable.obj", OBJ);
		MClock.init(this, &VMesh, "models/Room/Objects/Clock.obj", OBJ);
		MPainting.init(this, &VMesh, "models/Room/Objects/Painting.obj", OBJ);
		MPaperTray.init(this, &VMesh, "models/Room/Objects/PaperTray.obj", OBJ);
		MSharpener.init(this, &VMesh, "models/Room/Objects/Sharpener.obj", OBJ);
		MComputer.init(this, &VMesh, "models/Room/Objects/Computer.obj", OBJ);
		MLamp.init(this, &VMesh, "models/Room/Objects/Lamp.obj", OBJ);

		TClock.init(this, "textures/TexturesCity.png");
		TTSP.init(this, "textures/TexturesCity.png");
		TClock.init(this, "textures/TexturesCity.png");
		TPainting.init(this, "textures/TexturesCity.png");
		TPaperTray.init(this, "textures/TexturesCity.png");
		TSharpener.init(this, "textures/TexturesCity.png");
		TComputer.init(this, "textures/TexturesCity.png");
		TLamp.init(this, "textures/TexturesCity.png");

		// Emitting Textures
		TMeshEmit.init(this, "textures/TexturesCity.png");
		TComputerEmit.init(this, "textures/TexturesCity.png");

		// TODO
		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
		gameState = 0;
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

		DSTSP.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TTSP},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSClock.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TClock},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPainting.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPainting},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPaperTray.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPaperTray},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSSharpener.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TSharpener},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSComputer.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TComputer},
			{2, TEXTURE, 0, &TComputerEmit},
			});

		DSLamp.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TLamp},
			{2, TEXTURE, 0, &TMeshEmit},
			});


	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {

		PMesh.cleanup();
		//PProcedural.cleanup();

		DSGubo.cleanup();
		DSSpotLight.cleanup();
		DSTSP.cleanup();
		DSClock.cleanup();
		DSPainting.cleanup();
		DSPaperTray.cleanup();
		DSSharpener.cleanup();
		DSComputer.cleanup();
		DSLamp.cleanup();

	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {

		MTSP.cleanup();
		MClock.cleanup();
		MPainting.cleanup();
		MPaperTray.cleanup();
		MSharpener.cleanup();
		MComputer.cleanup();
		MLamp.cleanup();

		TTSP.cleanup();
		TClock.cleanup();
		TPainting.cleanup();
		TPaperTray.cleanup();
		TSharpener.cleanup();
		TComputer.cleanup();
		TLamp.cleanup();

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

		MTSP.bind(commandBuffer);
		DSTSP.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MTSP.indices.size()), 1, 0, 0, 0);

		MClock.bind(commandBuffer);
		DSClock.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MClock.indices.size()), 1, 0, 0, 0);

		MPainting.bind(commandBuffer);
		DSPainting.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPainting.indices.size()), 1, 0, 0, 0);

		MPaperTray.bind(commandBuffer);
		DSPaperTray.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPaperTray.indices.size()), 1, 0, 0, 0);

		MSharpener.bind(commandBuffer);
		DSSharpener.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSharpener.indices.size()), 1, 0, 0, 0);

		MComputer.bind(commandBuffer);
		DSComputer.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MComputer.indices.size()), 1, 0, 0, 0);

		MLamp.bind(commandBuffer);
		DSLamp.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MLamp.indices.size()), 1, 0, 0, 0);

		//PProcedural.bind(commandBuffer);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;
//std::cout << xpos << " " << ypos << " " << m_dx << " " << m_dy << "\n";

		/*/if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				std::cout << "Scene : " << currScene << "\n";
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}*/

		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		
		GameLogic();

		// updates global uniforms
		const float FOVy = glm::radians(90.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.0f;
		const float rotSpeed = glm::radians(90.0f);
		const float movSpeed = 1.0f;

		glm::vec3 camTarget = glm::vec3(0, CamH, 0);
		glm::vec3 camPos = camTarget +
			CamRadius * glm::vec3(cos(CamPitch) * sin(CamYaw),
				sin(CamPitch),
				cos(CamPitch) * cos(CamYaw));

		// FILL AND SET GLOBAL UNIFORMS
		// GUBO
		gubo.DlightDir = glm::normalize(glm::vec3(1, 1, 1));
		gubo.DlightColor = glm::vec4(0.2f, 0.2f, 0.2f, 1);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = camPos;
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		// SPOT UBO
		glm::vec3 lampPos = glm::vec3(4.5f, 5.0f, -2.5f); // Position of the lamp object
		uboSpot.lightDir = glm::normalize(glm::vec3(-3, -1, 1.0f));
		uboSpot.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		uboSpot.lightPos = World * glm::vec4(lampPos, 1.);
		uboSpot.eyePos = camPos;
		DSSpotLight.map(currentImage, &uboSpot, sizeof(uboSpot), 0);

		// FILL AND SET OBJECTS UNIFORMS
		uboTSP.amb = 1.0f; uboTSP.gamma = 180.0f; uboTSP.sColor = glm::vec3(1.0f);
		uboTSP.mvpMat = ViewPrj * World;
		uboTSP.mMat = World;
		uboTSP.nMat = glm::inverse(glm::transpose(World));
		DSTSP.map(currentImage, &uboTSP, sizeof(uboTSP), 0);

		uboClock.amb = 1.0f; uboClock.gamma = 180.0f; uboClock.sColor = glm::vec3(1.0f);
		uboClock.mvpMat = ViewPrj * World;
		uboClock.mMat = World;
		uboClock.nMat = glm::inverse(glm::transpose(World));
		DSClock.map(currentImage, &uboClock, sizeof(uboClock), 0);

		uboPainting.amb = 1.0f; uboPainting.gamma = 180.0f; uboPainting.sColor = glm::vec3(1.0f);
		uboPainting.mvpMat = ViewPrj * World;
		uboPainting.mMat = World;
		uboPainting.nMat = glm::inverse(glm::transpose(World));
		DSPainting.map(currentImage, &uboPainting, sizeof(uboPainting), 0);

		uboPaperTray.amb = 1.0f; uboPaperTray.gamma = 180.0f; uboPaperTray.sColor = glm::vec3(1.0f);
		uboPaperTray.mvpMat = ViewPrj * World;
		uboPaperTray.mMat = World;
		uboPaperTray.nMat = glm::inverse(glm::transpose(World));
		DSPaperTray.map(currentImage, &uboPaperTray, sizeof(uboPaperTray), 0);

		uboSharpener.amb = 1.0f; uboSharpener.gamma = 180.0f; uboSharpener.sColor = glm::vec3(1.0f);
		uboSharpener.mvpMat = ViewPrj * World;
		uboSharpener.mMat = World;
		uboSharpener.nMat = glm::inverse(glm::transpose(World));
		DSSharpener.map(currentImage, &uboSharpener, sizeof(uboSharpener), 0);

		uboComputer.amb = 1.0f; uboComputer.gamma = 180.0f; uboComputer.sColor = glm::vec3(1.0f);
		uboComputer.mvpMat = ViewPrj * World * glm::translate(glm::mat4(1.0), glm::vec3(-3.0f, 3.0f, 0.0f));
		uboComputer.mMat = World;
		uboComputer.nMat = glm::inverse(glm::transpose(World));
		DSComputer.map(currentImage, &uboComputer, sizeof(uboComputer), 0);

		uboLamp.amb = 1.0f; uboLamp.gamma = 180.0f; uboLamp.sColor = glm::vec3(1.0f);
		uboLamp.mvpMat = ViewPrj * World * glm::translate(glm::mat4(1.0), lampPos);
		uboLamp.mMat = World;
		uboLamp.nMat = glm::inverse(glm::transpose(World));
		DSLamp.map(currentImage, &uboLamp, sizeof(uboLamp), 0);
		
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

		World = glm::mat4(1);



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


		// std::cout << "X=" << Pos.x << "    Y="<< Pos.y << "    Z="<< Pos.z << std::endl;

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