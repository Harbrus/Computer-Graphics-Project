#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main() {
	vec2 u(1.0f, 0.0f);
	vec2 v(vec3(1.0, 0.0, 1.0));
	vec3 w(1.0, 0.0, 0.0);
	vec3 a(1.4, 1.0, 0.0);
	vec4 b(u, 1.0, 2.0);
	vec4 c(w, 1.0);

	vec3 r = w + a;
	r = 5.0f * r;

	float l = length(r);
	float aw = length(c);
	vec3 n = normalize(w);
	float d = dot(u, v);
	vec3 k = proj(a, w);
	vec3 o = cross(k, n);

	mat4 m(1.0f);
	mat4 T;
	vec3 s;
	// Can 't multiply a 3D vector by a 4x4 matrix
	// Have to convert to a 4D vector
	vec4 v = T * vec4(s, 1.0f);
	// we can always get the 3d version.
	vec3 w = vec3(T*vec4(s, 1.0f));

	// TRANSFORMATION
	// Translation
	mat4 T = translate(mat4(1.0f), vec3(1.0, 0.0, 0.0));
	// We have to transform an initial matrix - use identity


	//Rotation matrix
	mat4 Rx = rotate(mat4(1.0f), (pi<float>()/2), vec3(1.0f, 0.0f, 0.0f));
	mat4 Ry = rotate(mat4(1.0f), (pi<float>()/2), vec3(0.0f, 1.0f, 0.0f));
	mat4 Rz = rotate(mat4(1.0f), (pi<float>()/2), vec3(0.0f, 0.0f, 1.0f));

	mat4 R = eulerAngleYXZ(1.0,1.0,1.0);
	mat4 S = scale(mat4(1.0f), vec3(10, 1, 5));

	mat4 trans = T * (R*S);
}