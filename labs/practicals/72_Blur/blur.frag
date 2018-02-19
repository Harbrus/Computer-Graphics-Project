#version 430 core

// Incoming frame data
uniform sampler2D tex;

// 1.0f / screen width
uniform float inverse_width;
// 1.0f / screen height
uniform float inverse_height;

// Surrounding pixels to sample and their scale
const vec4 samples[4] = vec4[4](vec4(-1.0, 0.0, 0.0, 0.25), vec4(1.0, 0.0, 0.0, 0.25), vec4(0.0, 1.0, 0.0, 0.25),
                                vec4(0.0, -1.0, 0.0, 0.25));
// Edge detection
const vec4 samplesEdge[6] = vec4[6](vec4(-1.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 2.0), vec4(1.0, 1.0, 0.0, 1.0),
                                vec4(-1.0, -1.0, 0.0, -1.0), vec4(0.0, -1.0, 0.0,-2.0), vec4(1.0, -1.0, 0.0, -1.0));

// SharpEdge detection
const vec4 samplesSharpEdge[5] = vec4[5](vec4(-1.0, 0.0, 0.0, -2.0/3.0), vec4(0.0, 1.0, 0.0, -2.0/3.0), vec4(1.0, 0.0, 0.0, -2.0/3.0),
                                vec4(0.0, -1.0, 0.0, -2.0/3.0), vec4(0.0, 0.0, 0.0,11.0/3.0));

// Gaussian One detection
const vec4 samplesGaussianOne[7] = vec4[7](vec4(-3.0, 0.0, 0.0, 1.0/64.0), vec4(-2.0, 0.0, 0.0, 6.0/64.0), vec4(-1.0, 0.0, 0.0, 15.0/64.0),
                                vec4(0.0, 0.0, 0.0, 20.0/64.0), vec4(1.0, 0.0, 0.0, 15.0/64.0), vec4(2.0, 0.0, 0.0, 6.0/64.0), vec4(3.0, 0.0, 0.0, 1.0/64.0));

// Gaussian Two detection
const vec4 samplesGaussianTwo[7] = vec4[7](vec4(0.0, -3.0, 0.0, 1.0/64.0), vec4(0.0, -2.0, 0.0, 6.0/64.0), vec4(0.0, -1.0, 0.0, 15.0/64.0),
                                vec4(0.0, 0.0, 0.0, 20.0/64.0), vec4(0.0, 1.0, 0.0, 15.0/64.0), vec4(0.0, 2.0, 0.0, 6.0/64.0), vec4(0.0, 3.0, 0.0, 1.0/64.0));
// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Start with colour as black
  colour = vec4(0.0f,0.0f,0.0f,1.0f);
  // Loop through each sample vector
	for (int i = 0; i < 6; i++)
	{
		// Calculate tex coord to sample
		vec2 uv = tex_coord + vec2(samplesSharpEdge[i].x * inverse_width, samplesSharpEdge[i].y * inverse_height);
		// Sample the texture and scale appropriately
		// - scale factor stored in w component
		vec4 tempColour = texture(tex,uv);
		tempColour *= samplesSharpEdge[i].w;
		colour += tempColour;
	}

	/*for (int i = 0; i < 7; i++)
	{
		// Calculate tex coord to sample
		vec2 uv = tex_coord + vec2(samplesGaussianOne[i].x * inverse_width, samplesGaussianOne[i].y * inverse_height);
		// Sample the texture and scale appropriately
		// - scale factor stored in w component
		vec4 tempColour = texture(tex,uv);
		tempColour *= samplesGaussianOne[i].w;
		colour += tempColour;
	}
	for (int i = 0; i < 7; i++)
	{
		// Calculate tex coord to sample
		vec2 uv = tex_coord + vec2(samplesGaussianTwo[i].y * inverse_height , samplesGaussianTwo[i].x * inverse_width);
		// Sample the texture and scale appropriately
		// - scale factor stored in w component
		vec4 tempColour = texture(tex,uv);
		tempColour *= samplesGaussianTwo[i].w;
		colour += tempColour;
	}*/

  // Ensure alpha is 1.0
  colour.a = 1.0;
  // *********************************
}
/*
float threshold(in float thr1, in float thr2 , in float val) {
 if (val < thr1) {return 0.0;}
 if (val > thr2) {return 1.0;}
 return val;
}
// averaged pixel intensity from 3 color channels
float avg_intensity(in vec4 pix) {
 return (pix.r + pix.g + pix.b)/3.;
}
vec4 get_pixel(in vec2 coords, in float dx, in float dy) {
 return texture2D(tex,coords + vec2(dx, dy));
}
// returns pixel color
float IsEdge(in vec2 coords){
  float dxtex = 1.0 / inverse_width image width;
  float dytex = 1.0 / inverse_height image height;
  float pix[9];
  int k = 0;
  float delta;
  // read neighboring pixel intensities
  for (int i=-1; i<2; i++) {
   for(int j=-1; j<2; j++) {
    
    pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex, float(j)*dytex));
	k++;
   }
  }
  // average color differences around neighboring pixels
  delta = (abs(pix[1]-pix[7]) + abs(pix[5]-pix[3]) + abs(pix[0]-pix[8]) + abs(pix[2]-pix[6])) / 4.;
  return threshold(0.1,0.1,clamp(1.8*delta,0.0,1.0));
}
void main()
{
  vec4 color = vec4(0.0,0.0,0.0,1.0);
  color.b = IsEdge(tex_coord);
  colour = color;
}*/