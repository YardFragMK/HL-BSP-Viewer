#include<glad/glad.h>
#include<SDL.h>
#include<iostream>


// ---------------------------------------------
//  SHADER KAYNAK KODLARI
// ---------------------------------------------

const char* vertexShaderSource = R"(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;

    out vec3 vColor;

    void main() {
        vColor = aColor;
        gl_Position = vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core

    in  vec3 vColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vColor, 1.0);
    }
)";



// ---------------------------------------------
//  SHADER DERLEME
// ---------------------------------------------

//Bir shader oluşturur
unsigned int compileShader(unsigned int type, const char* source) { 

	unsigned int shader = glCreateShader(type); //GPU tarafında bos shader olusturur
	glShaderSource(shader, 1, &source, nullptr); //GLSL kodunu yukler
	glCompileShader(shader); //Derler

	// Hata kontrolü
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Derleme basarili mi?
	if (!success) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log); //Hata mesajini al
		std::cout << "Shader hatasi:\n" << log << std::endl; //Hta mesajini yaz
		return 0;
	}

	return shader;
}

//Vertex + Fragment shaderı birleştirir
unsigned int createShaderProgram(const char* vertSrc, const char* fragSrc) { 

	unsigned int vert = compileShader(GL_VERTEX_SHADER, vertSrc);
	unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

	unsigned int program = glCreateProgram(); //Program olusturur
	//Shaderları programa ekle.
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	//Birbirlerine bagla
	glLinkProgram(program);

	// Hata kontrolu
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::cout << "Program link hatasi:\n" << log << std::endl;
	}

	// Shader'lar programa baglandi, artık ayrıca tutmaya gerek yok
	glDeleteShader(vert);
	glDeleteShader(frag);

	return program;
}

int main(int argc, char* argv[]) {
	//--SDL BASLATILIR---
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init hatasi: " << SDL_GetError() << std::endl;
		return 1;
	}

	atexit(SDL_Quit); //SDL kpanısta otomatik temizlik yapar

	//---OPENGL OZELLIKLERI---
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
	// Depth buffer: 3D'de önde olanın arkadakini kapatması için
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	// Double buffer: ekran titremesini önler
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//---PENCERE---
	int windowWidth=1280;
	int windowHeight=720;

	SDL_Window* window = SDL_CreateWindow(
		"BSP Viewer v0.1",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window) {
		std::cout << "Pencere olusturulamadi: " << SDL_GetError() << std::endl;
		return 1;
	}

	//---OPENGL CONTEXT---
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		std::cout << "OpenGL context olusturulamadi: " << SDL_GetError() << std::endl;
		return 1;
	}
	// Context ve pencere baglantisi
	SDL_GL_MakeCurrent(window, glContext);
	SDL_GL_SetSwapInterval(1); //VSync 1:acik 0:kapali

	//---GLAD OPENGL---
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout<<"GLAD yuklenemedi\n";
		return 1;
	}
	std::cout << "GPU    : " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL   : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

// ---------------------------------------------
//  SHADER PROGRAMI
// ---------------------------------------------

	unsigned int shaderProgram = createShaderProgram(
		vertexShaderSource,
		fragmentShaderSource
	);

	// ---------------------------------------------
	//  ÜÇGEN VERİSİ
	//
	//  Ekran koordinatları:
	//  Merkez = (0, 0)   Sağ üst = (1, 1)
	//  Sol alt = (-1,-1) Sağ alt = (1,-1)
	//
	//  Her satır: x, y, z,  r, g, b
	// ---------------------------------------------

	float vertices[] = {
		//  pozisyon              renk
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  // sol alt  → mavi
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // sağ alt  → yeşil
		 0.0f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // üst orta → hirmizi
	};

	// ---------------------------------------------
	//  VAO ve VBO
	// ---------------------------------------------

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO); //VAO olusturur
	glGenBuffers(1, &VBO); //VBO olusturur

	// VAO'yu bağla — bundan sonraki ayarları hatırlayacak
	glBindVertexArray(VAO);

	// VBO'ya veriyi yükle
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Attribute 0: pozisyon (ilk 3 float)
	glVertexAttribPointer(
		0,                    // location = 0
		3,                    // 3 bileşen: x, y, z
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),    // stride: bir vertex 6 float yer kaplar
		(void*)0              // offset: 0. byte'tan başlar
	);
	glEnableVertexAttribArray(0);

	// Attribute 1: renk (sonraki 3 float)
	glVertexAttribPointer(
		1,                          // location = 1
		3,                          // 3 bileşen: r, g, b
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),          // stride aynı
		(void*)(3 * sizeof(float))  // offset: 3. float'tan başlar (12. byte)
	);
	glEnableVertexAttribArray(1);

	// Bağlamayı temizle
	glBindVertexArray(0);

	bool running = true;

	// Zaman takibi
	Uint64 prevTime = SDL_GetPerformanceCounter(); //Zaman sayaci
	Uint64 frequency = SDL_GetPerformanceFrequency(); //Sayac saniyede kac kez artti
	float  deltaTime = 0.0f;  // son frame kaç saniye sürdü
	int    frameCount = 0;
	float  fpsTimer = 0.0f;

	//---OYUN DONGUSU---
	while (running) {

		// ── Delta time hesapla ────────────────
		// Her frame ne kadar sürdüğünü ölç
		Uint64 currentTime = SDL_GetPerformanceCounter(); // Su anki zaman
		deltaTime = (float)(currentTime - prevTime) / (float)frequency;
		prevTime = currentTime; //Eski zamani guncelle

		// Çok büyük delta time olmasın (debug'da pause gibi)
		if (deltaTime > 0.1f) deltaTime = 0.1f; //Guvenlik

		// FPS hesapla ve başlığa yaz
		frameCount++;
		fpsTimer += deltaTime;
		if (fpsTimer >= 1.0f) {
			float fps = (float)frameCount / fpsTimer;

			char title[64];
			sprintf_s(title, "BSP Viewer | FPS: %.0f | dt: %.2fms",
				fps, deltaTime * 1000.0f);
			SDL_SetWindowTitle(window, title); //Basliga yaz
			//Sayaci sifirla
			frameCount = 0;
			fpsTimer = 0.0f;
		}


		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;

				case SDL_KEYDOWN:
					// Tuşa basıldı
					// event.key.repeat > 0 ise basılı tutmadan gelen tekrar
					if (event.key.repeat == 0) {
						std::cout<<"Tus basildi: scancode= "<< event.key.keysym.scancode<<std::endl;
					}

					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						running = false;
					}
					// V tuşu: vsync aç/kapat
					if (event.key.keysym.scancode == SDL_SCANCODE_V) {
						static bool vsync = true;
						vsync = !vsync;
						SDL_GL_SetSwapInterval(vsync ? 1 : 0);
						std::cout << "VSync: " << (vsync ? "ACIK" : "KAPALI") << std::endl;
					}
					break;
				case SDL_KEYUP:
					// Tuş bırakıldı
					std::cout<<"Tus birakildi: scancode= "<< event.key.keysym.scancode<<std::endl;
					break;
				case SDL_MOUSEMOTION:
					// Fare hareketi
					// xrel ve yrel frame basina mouse ne kadar hareket etti
					std::cout << "Fare: x= " << event.motion.x << " y= " << event.motion.y << " xrel= "<< event.motion.xrel << " yrel= " << event.motion.yrel << std::endl;
					break;
				case SDL_WINDOWEVENT:
					//Pencere
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						windowWidth = event.window.data1;
						windowHeight = event.window.data2;
						std::cout << "Pencere boyutu: " << windowWidth << " " << windowHeight << std::endl;
						glViewport(0, 0, windowWidth, windowHeight);
					}
					break;
			}
			//Ekrani temizle
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Shader'ı aktif et
			glUseProgram(shaderProgram);

			// VAO'yu bağla ve çiz
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			SDL_GL_SwapWindow(window);
		}
	}
	//Temiz cikis yap
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}