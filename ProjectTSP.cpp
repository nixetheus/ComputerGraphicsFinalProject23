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

// Overtlay Data
struct OverlayUniformBlock {
	alignas(4) float visible;
};

// The vertices data structures
// Mesh structure
struct VertexMesh {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 UV;
};

struct VertexOverlay {
	glm::vec2 pos;
	glm::vec2 UV;
};


class ProjectTSP;

// MAIN ! 
class ProjectTSP : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLGubo, DSLSpotLight, DSLMesh, DSLProcedural, DSLOverlay;

	// Vertex formats
	VertexDescriptor VMesh;
	VertexDescriptor VOverlay;

	// Pipelines [Shader couples]
	Pipeline PMesh, PProcedural;
	Pipeline POverlay;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model<VertexMesh> MTSP, MClock, MArm, MChair, MPainting, MPaperTray1, MPaperTray2, MSharpener, MLamp, MPencil, MProcedural;
	Model<VertexMesh> MComputer1, MComputer2;
	Model<VertexOverlay> MTitle;

	Texture TTSP, TClock, TArm, TChair, TPainting, TPaperTray1, TPaperTray2, TSharpener, TLamp, TPencil, TProcedural, TTitle;
	Texture TComputer1, TComputer2;
	Texture TMeshEmit, TComputerEmit1, TComputerEmit2;

	DescriptorSet DSGubo, DSSpotLight, DSTSP, DSClock, DSArm, DSChair, DSPainting, DSPaperTray1, DSPaperTray2, DSSharpener, DSLamp, DSPencil, DSProcedural, DSTitle;
	DescriptorSet DSComputer1, DSComputer2;

	// C++ storage for uniform variables
	GlobalUniformBufferObject gubo;
	SpotUniformBufferObject uboSpot;
	MeshUniformBlock uboTSP, uboClock, uboArm, uboChair, uboPainting, uboPaperTray1, uboPaperTray2, uboSharpener, uboLamp, uboPencil, uboProcedural;
	MeshUniformBlock uboComputer;
	OverlayUniformBlock uboTitle;

	/////////////////////////// PARAMETERS ///////////////////////////
		// Camera FOV-y, Near Plane and Far Plane
	const float FOVy = glm::radians(60.0f);
	const float nearPlane = 0.1f;
	const float farPlane = 100.f;
	// Height limits
	const float highPos = 5.0f;
	const float lowPos = 1.5f;
	// Camera Pitch limits
	const float minPitch = glm::radians(-60.0f);
	const float maxPitch = glm::radians(40.0f);
	// Rotation and motion speed
	const float ROT_SPEED = glm::radians(50.0f);
	const float MOVE_SPEED = 3.0f;

	// TODO CHANGE POSITION OF THIS CODE, MIMIC A16
	// Other application parameters
	float Ar;
	glm::mat4 World;
	glm::mat4 ViewPrj;
	//glm::vec3 cameraPos;
	glm::vec3 Pos = glm::vec3(-3,0,0);
	float Yaw = glm::radians(40.0f);
	float Pitch = glm::radians(-40.0f);
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
		uniformBlocksInPool = 30;
		texturesInPool = 30;
		setsInPool = 30;
		
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

		DSLProcedural.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},          // Mesh Ubo
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // Mesh Texture
			});

		DSLOverlay.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
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

		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});
		

		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on...
		PMesh.init(this, &VMesh, "shaders/MeshVert.spv", "shaders/MeshFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLMesh });
		PProcedural.init(this, &VMesh, "shaders/ProceduralVert.spv", "shaders/ProceduralFrag.spv", { &DSLGubo, &DSLSpotLight, &DSLProcedural });
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)
		MTSP.init(this, &VMesh, "models/Room/TheStanleyParablev10.obj", OBJ);
		MClock.init(this, &VMesh, "models/Room/Objects/Clock.obj", OBJ);
		MArm.init(this, &VMesh, "models/Room/Objects/ClockArm.obj", OBJ);
		MChair.init(this, &VMesh, "models/Room/Objects/Chair.obj", OBJ);
		MPencil.init(this, &VMesh, "models/Room/Objects/Pencil.obj", OBJ);
		MPainting.init(this, &VMesh, "models/Room/Objects/Painting.obj", OBJ);
		MPaperTray1.init(this, &VMesh, "models/Room/Objects/PaperTray1.obj", OBJ);
		MPaperTray2.init(this, &VMesh, "models/Room/Objects/PaperTray2.obj", OBJ);
		MSharpener.init(this, &VMesh, "models/Room/Objects/Sharpener.obj", OBJ);
		MLamp.init(this, &VMesh, "models/Room/Objects/Lamp.obj", OBJ);

		// Title Overlay
		MTitle.vertices = { {{-1.0f, -1.0f}, {0.0f, 0.0f}}, {{1.0f, -1.0f}, {1.0f, 0.0f}},
						 {{ -1.0f, 1.0f}, {0.0f, 1.0f}}, {{ 1.0f, 1.0f}, {1.0f, 1.0f}} };
		MTitle.indices = { 0, 2, 1, 1, 3, 2};
		MTitle.initMesh(this, &VOverlay);

		// Computers
		MComputer1.init(this, &VMesh, "models/Room/Objects/Computer.obj", OBJ);
		MComputer2.init(this, &VMesh, "models/Room/Objects/Computer.obj", OBJ);

		// Procedural
		createProcedural(MProcedural.vertices, MProcedural.indices);
		MProcedural.initMesh(this, &VMesh);

		TClock.init(this, "textures/clock.png");
		TArm.init(this, "textures/PaperTray1.png");
		TChair.init(this, "textures/ChairTexture.png");
		TTSP.init(this, "textures/RoomTexture2.png");
		TPainting.init(this, "textures/Painting.png");
		TPaperTray1.init(this, "textures/PaperTray1.png");
		TPaperTray2.init(this, "textures/PaperTray2.png");
		TSharpener.init(this, "textures/Sharpener.png");
		TLamp.init(this, "textures/steel.jpg");
		TPencil.init(this, "textures/TexturesCity.png");
		TProcedural.init(this, "textures/Mug.png");

		TTitle.init(this, "textures/Title.png");

		// Computers
		TComputer1.init(this, "textures/Computer1.png");
		TComputer2.init(this, "textures/Computer2.png");

		// Emitting Textures
		TMeshEmit.init(this, "textures/TexturesCity.png");
		TComputerEmit1.init(this, "textures/ComputerEmit1.png");
		TComputerEmit2.init(this, "textures/ComputerEmit2.png");

	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PMesh.create();
		POverlay.create();
		PProcedural.create();

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

		DSArm.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TArm},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSChair.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TChair},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPencil.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPencil},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPainting.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPainting},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPaperTray1.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPaperTray1},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSPaperTray2.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TPaperTray2},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSSharpener.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TSharpener},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSLamp.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TLamp},
			{2, TEXTURE, 0, &TMeshEmit},
			});

		DSComputer1.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TComputer1},
			{2, TEXTURE, 0, &TComputerEmit1},
			});

		DSComputer2.init(this, &DSLMesh, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TComputer2},
			{2, TEXTURE, 0, &TComputerEmit2},
			});

		DSProcedural.init(this, &DSLProcedural, {
			{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
			{1, TEXTURE, 0, &TProcedural},
			});

		DSTitle.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TTitle}
			});

	}

	// Here you destroy your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsCleanup() {

		PMesh.cleanup();
		POverlay.cleanup();
		PProcedural.cleanup();

		DSGubo.cleanup();
		DSSpotLight.cleanup();
		DSTSP.cleanup();
		DSClock.cleanup();
		DSArm.cleanup();
		DSChair.cleanup();
		DSPencil.cleanup();
		DSPainting.cleanup();
		DSPaperTray1.cleanup();
		DSPaperTray2.cleanup();
		DSSharpener.cleanup();
		DSComputer1.cleanup();
		DSComputer2.cleanup();
		DSLamp.cleanup();
		DSProcedural.cleanup();
		DSTitle.cleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// You also have to destroy the pipelines
	void localCleanup() {

		MTSP.cleanup();
		MClock.cleanup();
		MArm.cleanup();
		MChair.cleanup();
		MPencil.cleanup();
		MPainting.cleanup();
		MPaperTray1.cleanup();
		MPaperTray2.cleanup();
		MSharpener.cleanup();
		MComputer1.cleanup();
		MComputer2.cleanup();
		MLamp.cleanup();
		MProcedural.cleanup();
		MTitle.cleanup();

		TTSP.cleanup();
		TClock.cleanup();
		TArm.cleanup();
		TChair.cleanup();
		TPainting.cleanup();
		TPaperTray1.cleanup();
		TPaperTray2.cleanup();
		TSharpener.cleanup();
		TComputer1.cleanup();
		TComputer2.cleanup();
		TLamp.cleanup();
		TPencil.cleanup();
		TProcedural.cleanup();
		TMeshEmit.cleanup();
		TComputerEmit1.cleanup();
		TComputerEmit2.cleanup();
		TTitle.cleanup();

		DSLGubo.cleanup();
		DSLSpotLight.cleanup();
		DSLMesh.cleanup();
		DSLProcedural.cleanup();
		DSLOverlay.cleanup();

		PMesh.destroy();
		PProcedural.destroy();
		POverlay.destroy();
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

		DSGubo.bind(commandBuffer, PMesh, 0, currentImage);
		DSSpotLight.bind(commandBuffer, PMesh, 1, currentImage);

		DSGubo.bind(commandBuffer, PProcedural, 0, currentImage);
		DSSpotLight.bind(commandBuffer, PProcedural, 1, currentImage);

		PMesh.bind(commandBuffer);

		MTSP.bind(commandBuffer);
		DSTSP.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MTSP.indices.size()), 1, 0, 0, 0);

		MClock.bind(commandBuffer);
		DSClock.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MClock.indices.size()), 1, 0, 0, 0);

		MArm.bind(commandBuffer);
		DSArm.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MArm.indices.size()), 1, 0, 0, 0);

		MChair.bind(commandBuffer);
		DSChair.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MChair.indices.size()), 1, 0, 0, 0);

		MPencil.bind(commandBuffer);
		DSPencil.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPencil.indices.size()), 1, 0, 0, 0);

		MPainting.bind(commandBuffer);
		DSPainting.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPainting.indices.size()), 1, 0, 0, 0);

		MPaperTray1.bind(commandBuffer);
		DSPaperTray1.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPaperTray1.indices.size()), 1, 0, 0, 0);

		MPaperTray2.bind(commandBuffer);
		DSPaperTray2.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MPaperTray2.indices.size()), 1, 0, 0, 0);

		MSharpener.bind(commandBuffer);
		DSSharpener.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MSharpener.indices.size()), 1, 0, 0, 0);

		MComputer1.bind(commandBuffer);
		DSComputer1.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MComputer1.indices.size()), 1, 0, 0, 0);

		MComputer2.bind(commandBuffer);
		DSComputer2.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MComputer2.indices.size()), 1, 0, 0, 0);

		MLamp.bind(commandBuffer);
		DSLamp.bind(commandBuffer, PMesh, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MLamp.indices.size()), 1, 0, 0, 0);

		PProcedural.bind(commandBuffer);

		MProcedural.bind(commandBuffer);
		DSProcedural.bind(commandBuffer, PProcedural, 2, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MProcedural.indices.size()), 1, 0, 0, 0);

		POverlay.bind(commandBuffer);
		MTitle.bind(commandBuffer);
		DSTitle.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MTitle.indices.size()), 1, 0, 0, 0);
	}

	// Total Time Passed for Clock Arm
	int computerModel = 0;
	float totalSeconds = 0;

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		
		GameLogic();


		// FILL AND SET GLOBAL UNIFORMS
		// GUBO
		gubo.DlightDir = glm::normalize(glm::vec3(1, 1, 1));
		gubo.DlightColor = glm::vec4(0.2f, 0.2f, 0.2f, 1);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = Pos;
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		// SPOT UBO
		glm::vec3 lampPos = glm::vec3(4.5f, 4.1f, -2.5f); // Position of the lamp object
		uboSpot.lightDir = glm::mat3(World) * glm::normalize(glm::vec3(-3, -1, 0.0f));
		uboSpot.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		uboSpot.lightPos = World * glm::vec4(lampPos, 1.);
		uboSpot.eyePos = Pos;
		DSSpotLight.map(currentImage, &uboSpot, sizeof(uboSpot), 0);

		// FILL AND SET OBJECTS UNIFORMS
		uboTSP.amb = 1.0f; uboTSP.gamma = 180.0f; uboTSP.sColor = glm::vec3(1.0f);
		uboTSP.mvpMat = ViewPrj * World;
		uboTSP.mMat = World;
		uboTSP.nMat = glm::inverse(glm::transpose(World));
		DSTSP.map(currentImage, &uboTSP, sizeof(uboTSP), 0);

		uboClock.amb = 1.0f; uboClock.gamma = 180.0f; uboClock.sColor = glm::vec3(1.0f);
		uboClock.mvpMat = ViewPrj * World *
							(glm::translate(glm::mat4(1.0), glm::vec3(-6.2f, 6.1f, 2.3f)) * glm::rotate(glm::mat4(1.0), glm::radians(40.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0), glm::vec3(2, 2, 2)));
		uboClock.mMat = World;
		uboClock.nMat = glm::inverse(glm::transpose(World));
		DSClock.map(currentImage, &uboClock, sizeof(uboClock), 0);

		float armRotation = (((int)totalSeconds % 60) / 60.0f) * 360;
		uboArm.amb = 1.0f; uboArm.gamma = 180.0f; uboArm.sColor = glm::vec3(1.0f);
		uboArm.mvpMat = ViewPrj * World *
			(glm::translate(glm::mat4(1.0), glm::vec3(-6.15f, 6.1f, 2.3f)) * glm::rotate(glm::mat4(1.0), glm::radians(-armRotation), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0), glm::vec3(7, 7, 5)));
		uboArm.mMat = World;
		uboArm.nMat = glm::inverse(glm::transpose(World));
		DSArm.map(currentImage, &uboArm, sizeof(uboArm), 0);

		uboChair.amb = 1.0f; uboChair.gamma = 10000.0f; uboChair.sColor = glm::vec3(1.0f);
		uboChair.mvpMat = ViewPrj * World *
			(glm::translate(glm::mat4(1.0), glm::vec3(-3.75f, 0.6f, -0.6f)) * glm::rotate(glm::mat4(1.0), glm::radians(-75.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(2.7, 2.7, 2.7)));
		uboChair.mMat = World;
		uboChair.nMat = glm::inverse(glm::transpose(World));
		DSChair.map(currentImage, &uboChair, sizeof(uboChair), 0);

		uboPainting.amb = 1.0f; uboPainting.gamma = 180.0f; uboPainting.sColor = glm::vec3(1.0f);
		uboPainting.mvpMat = ViewPrj * World *
							(glm::translate(glm::mat4(1.0), glm::vec3(-3.2f, 5.6f, -3.45f)) * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5, 1.5, 1.5)));
		uboPainting.mMat = World;
		uboPainting.nMat = glm::inverse(glm::transpose(World));
		DSPainting.map(currentImage, &uboPainting, sizeof(uboPainting), 0);

		uboPaperTray1.amb = 1.0f; uboPaperTray1.gamma = 180.0f; uboPaperTray1.sColor = glm::vec3(1.0f);
		uboPaperTray1.mvpMat = ViewPrj * World *
							(glm::translate(glm::mat4(1.0), glm::vec3(-1.5f, 2.2f, -2.5f)) * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5, 1.5, 1.5)));
		uboPaperTray1.mMat = World;
		uboPaperTray1.nMat = glm::inverse(glm::transpose(World));
		DSPaperTray1.map(currentImage, &uboPaperTray1, sizeof(uboPaperTray1), 0);

		uboPaperTray2.amb = 1.0f; uboPaperTray2.gamma = 180.0f; uboPaperTray2.sColor = glm::vec3(1.0f);
		uboPaperTray2.mvpMat = ViewPrj * World *
							(glm::translate(glm::mat4(1.0), glm::vec3(-0.7f, 2.2f, -2.5f)) * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5, 1.5, 1.5)));
		uboPaperTray2.mMat = World;
		uboPaperTray2.nMat = glm::inverse(glm::transpose(World));
		DSPaperTray2.map(currentImage, &uboPaperTray2, sizeof(uboPaperTray2), 0);

		uboSharpener.amb = 1.0f; uboSharpener.gamma = 180.0f; uboSharpener.sColor = glm::vec3(1.0f);
		uboSharpener.mvpMat = ViewPrj * World *
							(glm::translate(glm::mat4(1.0), glm::vec3(1.5f, 2.1f, -2.1f)) * glm::rotate(glm::mat4(1.0), glm::radians(-115.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(1.5, 1.5, 1.5)));
		uboSharpener.mMat = World;
		uboSharpener.nMat = glm::inverse(glm::transpose(World));
		DSSharpener.map(currentImage, &uboSharpener, sizeof(uboSharpener), 0);

		uboLamp.amb = 1.0f; uboLamp.gamma = 180.0f; uboLamp.sColor = glm::vec3(1.0f);
		uboLamp.mvpMat = ViewPrj * World * 
						( glm::translate(glm::mat4(1.0), lampPos) * glm::rotate(glm::mat4(1.0), glm::radians(-90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(2.5, 2.5, 2.5)));
		uboLamp.mMat = World;
		uboLamp.nMat = glm::inverse(glm::transpose(World));
		DSLamp.map(currentImage, &uboLamp, sizeof(uboLamp), 0);

		uboPencil.amb = 1.0f; uboPencil.gamma = 180.0f; uboPencil.sColor = glm::vec3(1.0f);
		uboPencil.mvpMat = ViewPrj * World *
			(glm::translate(glm::mat4(1.0), glm::vec3(0.7f, 2.06f, -1.8f)) * glm::rotate(glm::mat4(1.0), glm::radians(40.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(4.5, 4.5, 4.5)));
		uboPencil.mMat = World;
		uboPencil.nMat = glm::inverse(glm::transpose(World));
		DSPencil.map(currentImage, &uboPencil, sizeof(uboPencil), 0);

		// Computer Models
		uboComputer.amb = 1.0f; uboComputer.gamma = 180.0f; uboComputer.sColor = glm::vec3(1.0f);
		uboComputer.mMat = World;
		uboComputer.nMat = glm::inverse(glm::transpose(World));

		float computerFlesh = ((int)(totalSeconds * 2) % 2);
		glm::vec3 scaleComputer1, scaleComputer2;
		if (computerFlesh == 0) scaleComputer1 = glm::vec3(2); else scaleComputer1 = glm::vec3(0);
		if (computerFlesh == 1) scaleComputer2 = glm::vec3(2); else scaleComputer2 = glm::vec3(0);

		// Computer 1
		uboComputer.mvpMat = ViewPrj * World *
			(glm::translate(glm::mat4(1.0), glm::vec3(-5.0f, 2.3f, -2.4f)) * glm::rotate(glm::mat4(1.0), glm::radians(-55.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), scaleComputer1));
		DSComputer1.map(currentImage, &uboComputer, sizeof(uboComputer), 0);

		// Computer 2
		uboComputer.mvpMat = ViewPrj * World *
			(glm::translate(glm::mat4(1.0), glm::vec3(-5.0f, 2.3f, -2.4f)) * glm::rotate(glm::mat4(1.0), glm::radians(-55.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), scaleComputer2));
		DSComputer2.map(currentImage, &uboComputer, sizeof(uboComputer), 0);

		// Procedrual
		uboProcedural.amb = 1.0f; uboProcedural.gamma = 180.0f; uboProcedural.sColor = glm::vec3(1.0f);
		uboProcedural.mvpMat = ViewPrj * World * (glm::translate(glm::mat4(1.0), glm::vec3(-3.35f, 2.23f, -2.25f)));
		uboProcedural.mMat = World;
		uboProcedural.nMat = glm::inverse(glm::transpose(World));
		DSProcedural.map(currentImage, &uboProcedural, sizeof(uboProcedural), 0);

		/* Map the uniform data block to the GPU */
		uboTitle.visible = 1.0f;
		DSTitle.map(currentImage, &uboTitle, sizeof(uboTitle), 0);
		
	}
	


	void GameLogic() {

		

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

		// Update time for clock
		totalSeconds += deltaT;

		// Change PC Model to simulate changing screen
		static int curDebounce = 0;
		static float debounce = false;
		if (glfwGetKey(window, GLFW_KEY_L)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_L;
				computerModel += 1;
				computerModel %= 2;
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_L) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
				
		
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

		// Final world matrix computaiton
		World = glm::rotate(glm::mat4(1.0f), -Roll, glm::vec3(0, 0, 1)) *
				glm::rotate(glm::mat4(1.0f), -Pitch, glm::vec3(1, 0, 0)) *
				glm::rotate(glm::mat4(1.0f), -Yaw, glm::vec3(0, 1, 0)) *
				glm::translate(glm::mat4(1), -Pos);
		
		// TO DO -> Posizione oggetti sbagliata

		// Projection
		glm::mat4 Prj = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Prj[1][1] *= -1;	// Given by the different Vulkan coordinate convention compared to GLM

		ViewPrj = Prj;// * World;
	}

	void createProcedural(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);
};


#include "Procedural.hpp"


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