#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
effect tex_eff;
texture tex;
texture alpha_map;
texture originalMap;
//target_camera cam;
directional_light light;
frame_buffer frame;
geometry screen_quad;
int screenMode = 0;
float zoomValue = 0;
free_camera freeCam;

bool load_content() {
	// *********************************
	// Create frame buffer - use screen width and height
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	// Create screen quad
	vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	// *********************************
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);
	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// Create scene
	meshes["box"] = mesh(geometry_builder::create_box());
	meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["disk"] = mesh(geometry_builder::create_disk(20));
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

	// Transform objects
	meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
	meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
	meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
	meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
	meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
	meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

	// Set materials
	// Red box
	meshes["box"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["box"].get_material().set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	meshes["box"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["box"].get_material().set_shininess(25.0f);
	// Green tetra
	meshes["tetra"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["tetra"].get_material().set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	meshes["tetra"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["tetra"].get_material().set_shininess(25.0f);
	// Blue pyramid
	meshes["pyramid"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["pyramid"].get_material().set_diffuse(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	meshes["pyramid"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["pyramid"].get_material().set_shininess(25.0f);
	// Yellow disk
	meshes["disk"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["disk"].get_material().set_diffuse(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	meshes["disk"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["disk"].get_material().set_shininess(25.0f);
	// Magenta cylinder
	meshes["cylinder"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["cylinder"].get_material().set_diffuse(vec4(1.0f, 0.0f, 1.0f, 1.0f));
	meshes["cylinder"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["cylinder"].get_material().set_shininess(25.0f);
	// Cyan sphere
	meshes["sphere"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["sphere"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	meshes["sphere"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["sphere"].get_material().set_shininess(25.0f);
	// White torus
	meshes["torus"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["torus"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["torus"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["torus"].get_material().set_shininess(25.0f);

	// Load texture
	tex = texture("textures/checked.gif", true, true);
	// *****************
	// Load in alpha map
	// *****************
	alpha_map = texture("textures/alpha_map.png");
	originalMap = texture("textures/snow.jpg");
	// Set lighting values
	light.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
	light.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light.set_direction(vec3(1.0f, 1.0f, -1.0f));

	// Load in shaders
	eff.add_shader("48_Phong_Shading/phong.vert", GL_VERTEX_SHADER);
	eff.add_shader("48_Phong_Shading/phong.frag", GL_FRAGMENT_SHADER);

	// **********************************
	// Change the fragment shader to mask
	// **********************************
	tex_eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
	tex_eff.add_shader("75_Masking/mask.frag", GL_FRAGMENT_SHADER);
	// Build effects
	eff.build();
	tex_eff.build();

	// Set camera properties
	/*cam.set_position(vec3(50.0f, 10.0f, 50.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);*/

	freeCam.set_position(vec3(0.0f, 5.0f, 10.0f));
	freeCam.set_target(vec3(0.0f, 0.0f, 0.0f));
	freeCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

bool update(float delta_time) {
	/* if (glfwGetKey(renderer::get_window(), '1')) {
	cam.set_position(vec3(50, 10, 50));
	}
	if (glfwGetKey(renderer::get_window(), '2')) {
	cam.set_position(vec3(-50, 10, 50));
	}
	if (glfwGetKey(renderer::get_window(), '3')) {
	cam.set_position(vec3(-50, 10, -50));
	}
	if (glfwGetKey(renderer::get_window(), '4')) {
	cam.set_position(vec3(50, 10, -50));
	}*/

	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() * renderer::get_screen_aspect()) / static_cast<float>(renderer::get_screen_height());
	static double cursor_x = 0.0;
	static double cursor_y = 0.0;
	double current_x;
	double current_y;
	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	// Calculate delta of cursor positions from last frame
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;
	// Multiply deltas by ratios - gets actual change in orientation
	delta_x *= ratio_width;
	delta_y *= ratio_height;
	// Rotate cameras by delta
	freeCam.rotate(delta_x, -delta_y);
	// Use keyboard to move the camera - WASD
	vec3 translation(0.0f, 0.0f, 0.0f);
	if (glfwGetKey(renderer::get_window(), 'W')) {
		translation.z += 15.0f * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'S')) {
		translation.z -= 15.0f * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'A')) {
		translation.x -= 15.0f * delta_time;
	}
	if (glfwGetKey(renderer::get_window(), 'D')) {
		translation.x += 15.0f * delta_time;
	}
	// Move camera
	freeCam.move(translation);
	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;

	if (glfwGetKey(renderer::get_window(), 'I'))
	{
		freeCam.set_projection(half_pi<float>() / 8.0, renderer::get_screen_aspect(), 0.1f, 1000.0f);
		//zoomValue = 1.5;
		screenMode = 1;
	}

	if (glfwGetKey(renderer::get_window(), 'O'))
	{
		freeCam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
		//zoomValue = 0;
		screenMode = 0;
	}

	// Rotate the sphere
	meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

	freeCam.update(delta_time);

	return true;
}

bool render() {
	// *********************************
	// Set render target to frame buffer
	renderer::set_render_target(frame);
	// Clear frame
	renderer::clear();
	// *********************************

	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = freeCam.get_view();
		//mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);
		auto P = freeCam.get_projection();
		mat4 MVP;
		//if (screenMode == 0)
		//{
		//	MVP = LightProjectionMat * V * M;
		//}
		//else
		//{
		MVP = P * V * M;
		//}
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Create MV matrix
		auto MV = V * M;
		// Set MV matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));
		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		// Bind material
		renderer::bind(m.get_material(), "mat");
		// Bind light
		renderer::bind(light, "light");
		// Bind texture
		renderer::bind(tex, 0);
		// Set tex uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);
		// Set eye position
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(freeCam.get_position()));

		// Render mesh
		renderer::render(m);
	}

	// *********************************
	// Set render target back to the screen
	renderer::set_render_target();
	// Bind Tex effect
	renderer::bind(tex_eff);
	// MVP is now the identity matrix
	auto MVP = mat4(1);
	// Set MVP matrix uniform
	glUniformMatrix4fv(tex_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Bind texture from frame buffer to TU 0
	renderer::bind(frame.get_frame(), 0);
	// Set the tex uniform, 0
	glUniform1i(tex_eff.get_uniform_location("tex"), 0);
	if (screenMode == 0)
	{
		// Bind alpha texture to TU, 1
		renderer::bind(originalMap, 1);
		// Set the tex uniform, 1
		glUniform1i(tex_eff.get_uniform_location("alpha_map"), 1);
	}
	else
	{
		// Bind alpha texture to TU, 1
		renderer::bind(alpha_map, 1);
		// Set the tex uniform, 1
		glUniform1i(tex_eff.get_uniform_location("alpha_map"), 1);
	}
	// Render the screen quad
	renderer::render(screen_quad);
	// *********************************
	return true;
}

void main() {
	// Create application
	app application("75_Masking");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}