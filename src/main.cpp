#include<glad/glad.h>
#include<SDL.h>
#include<iostream>

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
		}
	}
	return 0;
}