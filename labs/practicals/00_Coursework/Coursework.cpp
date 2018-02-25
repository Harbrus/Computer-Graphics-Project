#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
std::array<mesh, 3> hMeshes;
std::array<texture, 3> hTextures;
effect multiLightEff;
effect basicEff;
texture tex;
texture normal_map;

free_camera fCam;
target_camera tCam;
arc_ball_camera bCam;
int curr_cam = 0;

directional_light light;
vector<point_light> points(4);
vector<spot_light> spots(5);

effect main_eff;
effect shadow_eff;
spot_light spot;
shadow_map shadow;

bool shadows = false;
bool back = false;
bool multilights = false;

double cursor_x = 0.0;
double cursor_y = 0.0;

bool initialise() {

	// Create shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

	return true;
}

bool load_content() {
	material nMat;
	// Create scene
	meshes["plane"] = mesh(geometry_builder::create_plane());
	meshes["box"] = mesh(geometry_builder::create_box(vec3(1.0f, 2.0f, 1.0f)));
	meshes["teapot"] = mesh((geometry("models/teapot.obj")));
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["pyramid2"] = mesh(geometry_builder::create_pyramid());
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

	// Create Three Identical Pyramid Meshes
	hMeshes[0] = mesh(geometry_builder::create_pyramid());
	hMeshes[1] = mesh(geometry_builder::create_pyramid());
	hMeshes[2] = mesh(geometry_builder::create_pyramid());

	//// Move Hierarchy Elements
	hMeshes[0].get_transform().translate(vec3(0, 1, 0));
	hMeshes[1].get_transform().translate(vec3(0, 0.5, 1));
	hMeshes[2].get_transform().translate(vec3(0, 1, 0));

	// Transform objects
	meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["box"].get_transform().translate(vec3(-10.0f, 10.5f, -30.0f));
	meshes["teapot"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f);
	meshes["teapot"].get_transform().translate(vec3(-20.0f, 10.0f, -10.0f));
	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["pyramid"].get_transform().translate(vec3(-10.0f, 18.0f, -30.0f));
	meshes["pyramid2"].get_transform().scale = vec3(5.0f, 5.0f, 3.0f);
	meshes["pyramid2"].get_transform().translate(vec3(-15.0f, 11.5f, -30.0f));
	meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().translate(vec3(0.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().translate(vec3(0.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
	meshes["plane"].get_transform().scale = vec3(1.5f, 1.0f, 1.5f);
	// Create mesh to chase
	meshes["chaser"] = mesh(geometry_builder::create_box());
	meshes["chaser"].get_transform().translate(vec3(-10.0f, 10.5f, -30.0f));
	meshes["chaser"].get_transform().position = vec3(-5.0f, 0.5f, 0.0f);

	// Set material
	// - emissive black - diffuse (0.53, 0.45, 0.37)
	// - specular white - shininess 25
	nMat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	nMat.set_diffuse(vec4(1.0f));
	nMat.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	nMat.set_shininess(25.0f);

	meshes["box"].set_material(nMat);
	meshes["pyramid"].set_material(nMat);
	meshes["pyramid2"].set_material(nMat);
	meshes["cylinder"].set_material(nMat);
	meshes["sphere"].set_material(nMat);
	meshes["torus"].set_material(nMat);
	meshes["chaser"].set_material(nMat);
	meshes["teapot"].set_material(nMat);
	meshes["plane"].set_material(nMat);
	hMeshes[0].set_material(nMat);
	hMeshes[1].set_material(nMat);
	hMeshes[2].set_material(nMat);

	// Load in shaders

	multiLightEff.add_shader("shaders/multi-light.vert", GL_VERTEX_SHADER);
	multiLightEff.add_shader("shaders/multi-light.frag", GL_FRAGMENT_SHADER);

	basicEff.add_shader("shaders/shader.vert", GL_VERTEX_SHADER);
	basicEff.add_shader("shaders/shader.frag", GL_FRAGMENT_SHADER);
	basicEff.add_shader("shaders/part_direction.frag", GL_FRAGMENT_SHADER);
	basicEff.add_shader("shaders/part_normal_map.frag", GL_FRAGMENT_SHADER);

	main_eff.add_shader("54_Shadowing/shadow.vert", GL_VERTEX_SHADER);
	vector<string> frag_shaders{ "54_Shadowing/shadow.frag", "shaders/part_spot.frag", "shaders/part_shadow.frag" };
	main_eff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);

	shadow_eff.add_shader("shaders/multi-light.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("shaders/multi-light.frag", GL_FRAGMENT_SHADER);

	// Load Textures
	tex = texture("textures/brick.jpg");
	normal_map = texture("textures/brick_normalmap.jpg");
	hTextures[0] = texture("textures/barrel.jpg");
	hTextures[1] = texture("textures/stonygrass.jpg");
	hTextures[2] = texture("textures/grass.jpg");

	// Build effect
	basicEff.build();
	multiLightEff.build();
	main_eff.build();
	shadow_eff.build();

	// Set free camera properties
	fCam.set_position(vec3(0.0f, 10.0f, 0.0f));
	fCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	fCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// Set target camera properties
	tCam.set_position(vec3(50.0f, 10.0f, 50.0f));
	tCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	tCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// Set arc ball camera properties
	bCam.set_target(meshes["chaser"].get_transform().position);
	bCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	bCam.move(20.0f);

	return true;
}

bool setDirectLight()
{
	//Set light properties
	light.set_ambient_intensity(vec4(0.1f));
	light.set_light_colour(vec4(1.0f));
	light.set_direction(normalize(vec3(-1.0, 0.5, 0.4)));
	return true;
}

bool setShadowScene()
{
	material mat;
	light.set_ambient_intensity(vec4(0.0f));
	
	//set material
	mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	mat.set_diffuse(vec4(1.0f));
	mat.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat.set_shininess(25.0f);

	meshes["box"].set_material(mat);
	meshes["pyramid"].set_material(mat);
	meshes["pyramid2"].set_material(mat);
	meshes["cylinder"].set_material(mat);
	meshes["sphere"].set_material(mat);
	meshes["torus"].set_material(mat);
	meshes["chaser"].set_material(mat);
	meshes["teapot"].set_material(mat);
	meshes["plane"].set_material(mat);

	//Set spot properties
	spot.set_position(vec3(-10.0f, 25.0f, -45.0f));
	spot.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spot.set_direction(normalize(vec3(-spot.get_position())));
	spot.set_range(3000);
	spot.set_power(0.1f);

	return true;
}

bool setMultiLightsAndMaterials()
{
	// Set materials
	material mat;
	mat.set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	mat.set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mat.set_shininess(25.0);

	// Red box
	mat.set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	meshes["box"].set_material(mat);

	// Green teapot
	mat.set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	meshes["teapot"].set_material(mat);

	// Blue pyramid
	mat.set_diffuse(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	meshes["pyramid"].set_material(mat);

	// Yellow pyramid
	mat.set_diffuse(vec4(1.0, 1.0f, 0.0f, 1.0f));
	meshes["pyramid2"].set_material(mat);

	// Magenta cylinder
	mat.set_diffuse(vec4(1.0f, 0.0f, 1.0f, 1.0f));
	meshes["cylinder"].set_material(mat);

	// Cyan sphere
	mat.set_diffuse(vec4(0.0, 1.0f, 1.0f, 1.0f));
	meshes["sphere"].set_material(mat);

	// White torus
	mat.set_diffuse(vec4(1.0, 1.0f, 1.0f, 1.0f));
	meshes["torus"].set_material(mat);

	// Purple Chaser Box
	mat.set_diffuse(vec4(0.75f, 0.0f, 1.3f, 1.0f));
	meshes["chaser"].set_material(mat);

	// Set lighting values
	// Point 0
	points[0].set_position(vec3(0.0f, 5.0f, -15.0f));
	points[0].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[0].set_range(20.0f);

	// Point 1
	// Green

	points[1].set_position(vec3(-25.0f, 5.0f, -15.0f));
	points[1].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[1].set_range(20.0f);

	// Point 2
	// Yellow
	points[2].set_position(vec3(-10.0f, 5.0f, -15.0f));
	points[2].set_light_colour(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	points[2].set_range(20.0f);

	// Point 3
	// Blue

	points[3].set_position(vec3(10.0f, 10.0f, 10.0f));
	points[3].set_light_colour(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	points[3].set_range(20.0f);

	// Spot 0
	spots[0].set_position(vec3(0.0f, 0.0, 0.0));
	spots[0].set_light_colour(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	spots[0].set_direction(normalize(vec3(0.0f, 0.0f, -1.0f)));
	spots[0].set_range(10.0f);
	spots[0].set_power(1.0);

	// Spot 1
	spots[1].set_position(vec3(-25.0f, 10.0f, -35.0f));
	spots[1].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	spots[1].set_direction(normalize(vec3(1.0f, -1.0f, 1.0f)));
	spots[1].set_range(50.0f);
	spots[1].set_power(0.5);

	// Spot 2
	spots[2].set_position(vec3(-10.0f, 10.0f, -15.0f));
	spots[2].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spots[2].set_direction(normalize(vec3(-1.0f, -1.0f, -1.0f)));
	spots[2].set_range(20.0f);
	spots[2].set_power(0.5);

	// Spot 3
	spots[3].set_position(vec3(-10.0f, 10.0f, -35.0f));
	spots[3].set_light_colour(vec4(0.8f, 1.0f, 0.3f, 1.0f));
	spots[3].set_direction(normalize(vec3(-1.0f, -1.0f, 1.0f)));
	spots[3].set_range(20.0f);
	spots[3].set_power(0.5);

	// Spot 4
	spots[4].set_position(vec3(-17.5f, 15.0f, -25.0f));
	spots[4].set_light_colour(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	spots[4].set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
	spots[4].set_range(30.0f);
	spots[4].set_power(0.5);

	return true;
}

void userInput()
{
	//  Free Camera 
	if (glfwGetKey(renderer::get_window(), 'F'))
	{
		curr_cam = 1;
	}

	// Target Camera
	else if (glfwGetKey(renderer::get_window(), 'T'))
	{
		curr_cam = 0;

	}

	else if (glfwGetKey(renderer::get_window(), 'B'))
	{
		curr_cam = 2;

	}

	else if (glfwGetKey(renderer::get_window(), 'M'))
	{
		multilights = true;
	}
	else if (glfwGetKey(renderer::get_window(), 'N'))
	{
		shadows = false;
		multilights = false;
	}
	else if (glfwGetKey(renderer::get_window(), 'K'))
	{
		shadows = true;
	}
	else if (glfwGetKey(renderer::get_window(), 'L'))
	{
		shadows = false;
	}
}

bool update(float delta_time) {

	userInput();

	if (curr_cam == 1)
	{
		// The ratio of pixels to rotation - remember the fov
		static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
		static double ratio_height =
			(quarter_pi<float>() *
			(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
			static_cast<float>(renderer::get_screen_height());

		double current_x;
		double current_y;

		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
		// Calculate delta of cursor positions from last frame
		float delta_x = current_x - cursor_x;
		float delta_y = current_y - cursor_y;
		// Multiply deltas by ratios - gets actual change in orientation
		delta_x *= ratio_width;
		delta_y *= ratio_height;
		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation
		fCam.rotate(delta_x, -delta_y);
		// Use keyboard to move the camera - WSAD
		if (glfwGetKey(renderer::get_window(), 'W'))
		{
			fCam.move(vec3(0.0f, 0.0f, 2.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'S'))
		{
			fCam.move(vec3(0.0f, 0.0f, -2.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'A'))
		{
			fCam.move(vec3(-2.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'D'))
		{
			fCam.move(vec3(2.0f, 0.0f, 0.0f));
		}
		fCam.update(delta_time);
		// Update cursor pos
		cursor_x = current_x;
		cursor_y = current_y;
	}
	else if (curr_cam == 0)
	{
		if (glfwGetKey(renderer::get_window(), '1')) {
			tCam.set_position(vec3(50, 10, 50));
		}
		if (glfwGetKey(renderer::get_window(), '2')) {
			tCam.set_position(vec3(-50, 10, 50));
		}
		if (glfwGetKey(renderer::get_window(), '3')) {
			tCam.set_position(vec3(-50, 10, -50));
		}
		if (glfwGetKey(renderer::get_window(), '4')) {
			tCam.set_position(vec3(50, 10, -50));
		}
		tCam.update(delta_time);
	}

	else if (curr_cam == 2)
	{
		static mesh &target_mesh = meshes["chaser"];

		// The ratio of pixels to rotation - remember the fov
		static const float sh = static_cast<float>(renderer::get_screen_height());
		static const float sw = static_cast<float>(renderer::get_screen_height());
		static const double ratio_width = quarter_pi<float>() / sw;
		static const double ratio_height = (quarter_pi<float>() * (sh / sw)) / sh;

		double current_x;
		double current_y;
		// Get the current cursor position
		glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
		// Calculate delta of cursor positions from last frame
		float delta_x = current_x - cursor_x;
		float delta_y = current_y - cursor_y;
		// Multiply deltas by ratios and delta_time - gets actual change in orientation
		delta_x *= ratio_width;
		delta_y *= ratio_height;

		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation

		bCam.rotate(delta_y, delta_x);

		// Use keyboard to move the target_mesh- WSAD
		// Also remember to translate camera
		if (glfwGetKey(renderer::get_window(), 'W'))
		{
			target_mesh.get_transform().translate(vec3(0.0f, 0.0f, -2.0f));
			bCam.translate(vec3(0.0f, 0.0f, -2.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'S'))
		{
			target_mesh.get_transform().translate(vec3(0.0f, 0.0f, 2.0f));
			bCam.translate(vec3(0.0f, 0.0f, 2.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'V'))
		{
			target_mesh.get_transform().translate(vec3(0.0f, 2.0f, 0.0f));
			bCam.translate(vec3(0.0f, 2.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'C'))
		{
			target_mesh.get_transform().translate(vec3(0.0f, -2.0f, 0.0f));
			bCam.translate(vec3(0.0f, -2.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'A'))
		{
			target_mesh.get_transform().translate(vec3(-2.0f, 0.0f, 0.0f));
			bCam.translate(vec3(-2.0f, 0.0f, 0.0f));
		}
		if (glfwGetKey(renderer::get_window(), 'D'))
		{
			target_mesh.get_transform().translate(vec3(2.0f, 0.0f, 0.0f));
			bCam.translate(vec3(2.0f, 0.0f, 0.0f));
		}
		// Use UP and DOWN to change camera distance
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP))
		{
			bCam.move(2.0f);
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN))
		{
			bCam.move(-2.0f);
		}

		// Update the camera
		bCam.update(delta_time);
		// Update cursor pos
		cursor_x = current_x;
		cursor_y = current_y;
	}

	// Update the shadow map light_position from the spot light
	shadow.light_position = spot.get_position();
	// do the same for light_dir property
	shadow.light_dir = spot.get_direction();

	// rotate Hierarchy Element one on Y axis by delta_time
	hMeshes[0].get_transform().rotate(vec3(0.0f, pi<float>() * delta_time, 0.0f));
	// rotate Hierarchy Element on Z axis by delta_time
	hMeshes[1].get_transform().rotate(vec3(0.0f, 0.0f, pi<float>() * delta_time));
	// rotate Hierarchy Elementon Y axis by delta_time
	hMeshes[2].get_transform().rotate(vec3(0.0f, pi<float>() * delta_time, 0.0f));

	meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);
	meshes["teapot"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

	if (meshes["pyramid2"].get_transform().position.z > vec3(-15.0, 11.5f, -50.0).z && back == false)
	{
		meshes["pyramid2"].get_transform().translate(vec3(0.0f, 0.0f, -2.0f)*delta_time);
	}
	else
	{
		back = true;
		meshes["pyramid2"].get_transform().translate(vec3(0.0f, 0.0f, 2.0f)*delta_time);
		if (meshes["pyramid2"].get_transform().position.z > vec3(-15.0f, 11.5f, -30.0f).z)
		{
			back = false;
		}
	}


	return true;
}

bool renderMultiLightMeshes(mat4 VP, vec3 pos)
{
	setMultiLightsAndMaterials();
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(multiLightEff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto N = m.get_transform().get_normal_matrix();
		auto MVP = VP * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(multiLightEff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(multiLightEff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(multiLightEff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
		// Bind material
		renderer::bind(m.get_material(), "mat");
		// Bind point lights
		renderer::bind(points, "points");
		// Bind spot lights 
		renderer::bind(spots, "spots");

		// Bind texture
		renderer::bind(tex, 1);
		// Set tex uniform
		glUniform1i(multiLightEff.get_uniform_location("tex"), 1);

		// Set eye position- Get this from active camera
		glUniform3fv(multiLightEff.get_uniform_location("eye_pos"), 1, value_ptr(pos));
		// Render mesh
		renderer::render(m);
	}

	auto M2 = meshes["plane"].get_transform().get_transform_matrix();
	// Set the texture value for the shader here
	glUniform1i(multiLightEff.get_uniform_location("tex"), 1);
	// Find the lcoation for the MVP uniform
	const auto loc = multiLightEff.get_uniform_location("MVP");
	// Render meshes
	for (size_t i = 0; i < hMeshes.size(); i++) {

		for (size_t j = i; j > 0; j--) {
			M2 = hMeshes[j - 1].get_transform().get_transform_matrix() * M2;
		}

		// Set MVP matrix uniform
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(VP * M2));
		// Bind texture to renderer
		renderer::bind(hTextures[i], 1);
		// Render mesh
		renderer::render(hMeshes[i]);
	}


	return true;
}

bool renderBasicScene(mat4 VP, vec3 pos)
{
	setDirectLight();
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(basicEff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto N = m.get_transform().get_normal_matrix();
		auto MVP = VP * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(basicEff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Set M matrix uniform
		glUniformMatrix4fv(basicEff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(basicEff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));
		// Bind Material
		renderer::bind(m.get_material(), "nMat");
		// Bind light
		renderer::bind(light, "light");
		// Bind texture
		renderer::bind(tex, 0);
		// Set tex uniform
		glUniform1i(basicEff.get_uniform_location("tex"), 0);
		// Bind normal_map
		renderer::bind(normal_map, 2);
		// Set normal_map uniform
		glUniform1i(basicEff.get_uniform_location("normal_map"), 2);
		// Set eye position
		glUniform3fv(basicEff.get_uniform_location("eye_pos"), 1, value_ptr(pos));
		// Render mesh
		renderer::render(m);
	}

	auto M2 = meshes["plane"].get_transform().get_transform_matrix();
	// Set the texture value for the shader here
	glUniform1i(basicEff.get_uniform_location("tex"), 0);
	// Find the lcoation for the MVP uniform
	const auto loc = basicEff.get_uniform_location("MVP");
	// Render meshes
	for (size_t i = 0; i < hMeshes.size(); i++) {

		for (size_t j = i; j > 0; j--) {
			M2 = hMeshes[j - 1].get_transform().get_transform_matrix() * M2;
		}

		// Set MVP matrix uniform
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(VP * M2));
		// Bind Material
		renderer::bind(hMeshes[i].get_material(), "mat");
		// Bind texture to renderer
		renderer::bind(hTextures[i], 1);
		// Render mesh
		renderer::render(hMeshes[i]);
	}

	return true;
}

bool renderShadowScene (mat4 VP, vec3 pos)
{
	setShadowScene();
	// Set render target to shadow map
	renderer::set_render_target(shadow);
	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set face cull mode to front
	glCullFace(GL_FRONT);

	// Create a new projection matrix for the light
	mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

	// Bind shader
	renderer::bind(shadow_eff);
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		// *********************************
		// View matrix taken from shadow map
		auto V = shadow.get_view();
		// *********************************
		auto MVP = LightProjectionMat * V * M;

		// Set MVP matrix uniform
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
			1,                                      // Number of values - 1 mat4
			GL_FALSE,                               // Transpose the matrix?
			value_ptr(MVP));                        // Pointer to matrix data
													// Render mesh
		renderer::render(m);
	}

	// Set render target back to the screen
	renderer::set_render_target();
	// Set face cull mode to back
	glCullFace(GL_BACK);

	// Bind shader
	renderer::bind(main_eff);

	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto MVP = VP * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), // Location of uniform
			1,                                    // Number of values - 1 mat4
			GL_FALSE,                             // Transpose the matrix?
			value_ptr(MVP));                      // Pointer to matrix data
												  // Set M matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE,
			value_ptr(m.get_transform().get_normal_matrix()));

		// Set lightMVP uniform, using:
		//Model matrix from m
		auto lM = m.get_transform().get_transform_matrix();

		// viewmatrix from the shadow map
		auto lV = shadow.get_view();

		// Multiply together with LightProjectionMat
		auto lightMVP = LightProjectionMat * lV * lM;

		// Set uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("lightMVP"), // Location of uniform
			1,                           			  // Number of values - 1 mat4
			GL_FALSE,								 // Transpose the matrix?
			value_ptr(lightMVP));
		// Bind material
		renderer::bind(m.get_material(), "mat");
		// Bind spot light
		renderer::bind(spot, "spot");

		// Bind texture
		renderer::bind(tex, 0);
		// Set tex uniform
		glUniform1i(main_eff.get_uniform_location("tex"), 0);
		// Set eye position
		glUniform3fv(main_eff.get_uniform_location("eye_pos"), 1, value_ptr(pos));
		// Bind shadow map texture - use texture unit 1
		renderer::bind(shadow.buffer->get_depth(), 1);
		glUniform1i(main_eff.get_uniform_location("shadow_map"), 1);
		// Render mesh
		renderer::render(m);
	}

	return true;
}

bool render() {

	// Create MVP matrix
	mat4 VP;
	vec3 pos = vec3(0);
	switch (curr_cam) {
	case 1:
		VP = fCam.get_projection() * fCam.get_view();
		pos = fCam.get_position();
		break;
	case 2:
		VP = bCam.get_projection() * bCam.get_view();
		pos = bCam.get_position();
		break;
	default:
		VP = tCam.get_projection() * tCam.get_view();
		pos = tCam.get_position();
		break;
	}

	if (multilights)
	{
		renderMultiLightMeshes(VP, pos);
	}
	else if (shadows)
	{
		renderShadowScene(VP, pos);
	}
	else
	{
		renderBasicScene(VP, pos);
	}

	return true;
}

void main() {
	// Create application
	app application("Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}