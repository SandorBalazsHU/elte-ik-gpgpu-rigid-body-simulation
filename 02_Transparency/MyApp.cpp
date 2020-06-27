#include "MyApp.h"

#include <math.h>
#include <vector>

#include <array>
#include <list>
#include <tuple>

#include "imgui\imgui.h"

#include "ObjParser_OGL3.h"

#include <cstdlib>
#include <ctime>

CMyApp::CMyApp(void)
{
	/* initialize random seed: */
	srand((unsigned int)time(NULL));
	gravity = glm::vec3(0.0f, 0.006f, 0.0f);
	ballInit();
}


CMyApp::~CMyApp(void)
{
	
}

float CMyApp::random(float lower, float upper)
{
	/* generate secret number between a and b: */
	float random = lower + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upper- lower)));
	//std::cout << random << std::endl;
	return random;
}

void CMyApp::ballInit() {
	int r = 40;
	float s = 0.2f;
	for (size_t i = 0; i < numberOfBalls; i++)
	{
		//positions[i] = glm::vec3(random(-r,r), random(-r, r), random(-r, r));
		positions[i] = glm::vec3(0, 50, 0);
		velocities[i] = glm::vec3(random(-s, s), random(-s, s), random(-s, s));
		//velocities[i] = glm::vec3(random(-0.1, 0.1), random(-0.1, 0.1), random(-0.1, 0.1));
	}
}

bool CMyApp::Init()
{
	// törlési szín legyen fehér
	glClearColor(0.0f,0.47f,0.87f, 1.0f);

	glEnable(GL_CULL_FACE); // kapcsoljuk be a hatrafele nezo lapok eldobasat
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)
	//glDepthMask(GL_FALSE); // kikapcsolja a z-pufferbe történő írást - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glDepthMask.xml

	// átlátszóság engedélyezése
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // meghatározza, hogy az átlátszó objektum az adott pixelben hogyan módosítsa a korábbi fragmentekből oda lerakott színt: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlendFunc.xhtml

	//
	// shaderek betöltése
	//

	// így is létre lehetne hozni:

	// a shadereket tároló program létrehozása
	m_program.Init({
		{ GL_VERTEX_SHADER, "myVert.vert" },
		{ GL_FRAGMENT_SHADER, "myFrag.frag" }
	},
	{
		{ 0, "vs_in_pos" },					// VAO 0-as csatorna menjen a vs_in_pos-ba
		{ 1, "vs_in_normal" },				// VAO 1-es csatorna menjen a vs_in_normal-ba
		{ 2, "vs_out_tex0" },				// VAO 2-es csatorna menjen a vs_in_tex0-ba
	});

	//
	// geometria definiálása (std::vector<...>) és GPU pufferekbe (m_buffer*) való feltöltése BufferData-val
	//

	// vertexek pozíciói:
	/*
	Az m_gpuBufferPos konstruktora már létrehozott egy GPU puffer azonosítót és a most következő BufferData hívás ezt 
		1. bind-olni fogja GL_ARRAY_BUFFER target-re (hiszen m_gpuBufferPos típusa ArrayBuffer) és
		2. glBufferData segítségével áttölti a GPU-ra az argumentumban adott tároló értékeit

	*/
	float size = boxSize;
	m_gpuBufferPos.BufferData(
		std::vector<glm::vec3>{
			//		  X, Y, Z
			glm::vec3(-size,-size,-size),	// 0-ás csúcspont
			glm::vec3(size, -size, -size), // 1-es
			glm::vec3(-size,-size, size), // 2-es
			glm::vec3(size, -size, size),  // 3-as
			glm::vec3(-size, size, -size),	// 0-ás csúcspont
			glm::vec3(size, size, -size), // 1-es
			glm::vec3(size, size, size), // 2-es
			glm::vec3(-size, size, size)  // 3-as
		}
	);

	// normálisai
	m_gpuBufferNormal.BufferData(
		std::vector<glm::vec3>{
			// normal.X,.Y,.Z
			glm::vec3(0, 1, 0), // 0-ás csúcspont
			glm::vec3(0, 1, 0), // 1-es
			glm::vec3(0, 1, 0), // 2-es
			glm::vec3(0, 1, 0),  // 3-as
			glm::vec3(0, 1, 0), // 0-ás csúcspont
			glm::vec3(0, 1, 0), // 1-es
			glm::vec3(0, 1, 0), // 2-es
			glm::vec3(0, 1, 0)  // 3-as
		}
	);

	// textúrakoordinátái
	m_gpuBufferTex.BufferData(
		std::vector<glm::vec2>{
			//        u, v
			glm::vec2(0, 0), // 0-ás csúcspont
			glm::vec2(1, 0), // 1-es
			glm::vec2(0, 1), // 2-es
			glm::vec2(1, 1), // 3-as
			glm::vec2(0, 0), // 0-ás csúcspont
			glm::vec2(1, 0), // 1-es
			glm::vec2(0, 1), // 2-es
			glm::vec2(1, 1)	 // 3-as
		}
	);

	// és a primitíveket alkotó csúcspontok indexei (az előző tömbökből) - triangle list-el való kirajzolásra felkészülve
	m_gpuBufferIndices.BufferData(
		std::vector<int>{
			0,1,2,	// 1. háromszög: 0-1-2 csúcspontokból
			2,1,3,	// 2. háromszög: 2-1-3 csúcspontokból
			2,0,4,
			4,7,2,
			4,0,1,
			4,5,1,
			3,1,5,
			3,6,5,
			2,3,6,
			2,6,7
		}
	);

	// geometria VAO-ban való regisztrálása
	m_vao.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferPos },	// 0-ás attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferPos GPU pufferben vannak
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, m_gpuBufferNormal },	// 1-es attribútum "lényegében" glm::vec3-ak sorozata és az adatok az m_gpuBufferNormal GPU pufferben vannak
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, m_gpuBufferTex }		// 2-es attribútum "lényegében" glm::vec2-ők sorozata és az adatok az m_gpuBufferTex GPU pufferben vannak
		},
		m_gpuBufferIndices
	);

	// textúra betöltése
	m_textureMetal.FromFile("texture.png");

	// mesh betöltése
	m_mesh = ObjParser::parse("sphere2.obj");

	// kamera
	m_camera.SetProj(45.0f, 640.0f / 480.0f, 0.01f, 1000.0f);

	return true;
}

void CMyApp::Clean()
{
}

void CMyApp::Update()
{
	static Uint32 last_time = SDL_GetTicks();
	float delta_time = (SDL_GetTicks() - last_time) / 1000.0f;

	m_camera.Update(delta_time);

	last_time = SDL_GetTicks();
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT) és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// mindegyik geometria ugyanazt a shader programot hasznalja: ne kapcsolgassuk most ki-be
	m_program.Use();

	m_program.SetTexture("texImage", 0, m_textureMetal);
	
	for (size_t i = 0; i < numberOfBalls; i++)
	{
		if (positions[i].y > -boxSize+1 && run) {
			velocities[i] = velocities[i] - gravity;
			positions[i] = positions[i] + velocities[i];
		}
		glm::mat4 suzanne1World = glm::translate(positions[i]);
		m_program.SetUniform("world", suzanne1World);
		m_program.SetUniform("worldIT", glm::transpose(glm::inverse(suzanne1World)));
		m_program.SetUniform("MVP", m_camera.GetViewProj() * suzanne1World);
		m_program.SetUniform("Kd", glm::vec4(0.8, 0.8, 0, 1));
		m_mesh->draw();
	}


	// fal

	// most kikapcsoljuk a hátlapeldobást, hogy lássuk mindkétt oldalát!
	// Feladat: még mi kellene ahhoz, hogy a megvilágításból származó színek jók legyenek?
	glDisable(GL_CULL_FACE);

	glm::mat4 wallWorld = glm::translate(glm::vec3(0, 0, 0));
	m_program.SetUniform("world", wallWorld);
	m_program.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	m_program.SetUniform("MVP", m_camera.GetViewProj()*wallWorld);
	m_program.SetUniform("Kd", m_wallColor);

	m_vao.Bind();
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	m_vao.Unbind(); // nem feltétlen szükséges: a m_mesh->draw beállítja a saját VAO-ját

	// de kapcsoljuk most vissza, mert még jön egy Suzanne
	//glEnable(GL_CULL_FACE);

	// végetért a 3D színtér rajzolása
	m_program.Unuse();

	//
	// UI
	//
	// A következő parancs megnyit egy ImGui tesztablakot és így látszik mit tud az ImGui.
	//ImGui::ShowTestWindow();
	// A ImGui::ShowTestWindow implementációja az imgui_demo.cpp-ben található
	// Érdemes még az imgui.h-t böngészni, valamint az imgui.cpp elején a FAQ-ot elolvasni.
	// Rendes dokumentáció nincs, de a fentiek elegendőek kell legyenek.

	ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Tesztablak"))
	{
		ImGui::Text("Keszitette: Sandor Balazs");
		ImGui::SliderFloat3("Gravity", &(gravity[0]), 0, 0.1);
		ImGui::Checkbox("RUN", &run);
		static int clicked = 0;
		if (ImGui::Button("Button")) ballInit();
	}
	ImGui::End();
}

void CMyApp::KeyboardDown(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp(key);
}

void CMyApp::MouseMove(SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove(mouse);
}

void CMyApp::MouseDown(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseWheel(SDL_MouseWheelEvent& wheel)
{
	m_camera.MouseWheel(wheel);
}

// a két paraméterbe az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h );

	m_camera.Resize(_w, _h);
}