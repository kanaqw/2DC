#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

enum KeyPressSurfaces {
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

//starts SDL and creates window
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface* loadSurface(std::string path);
//The window we'll be rendering to. g means Global variables
SDL_Window* gWindow = NULL;
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;
//The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//current displayed image
SDL_Surface* gCurrentSurface = NULL;
//Current displayed PNG image
SDL_Surface* gPNGSurface = NULL;
//The image we will load and show on screen
SDL_Surface* gNavImage = NULL;
SDL_Surface* gStretchedSurface = NULL;

bool init() {
	//init flag
	bool success = true;
	//init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//create window
		gWindow = SDL_CreateWindow("StuurmanNav v.0.1a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//initialize png loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags)) {
				printf("SDL_image could not initialize! SDL_image Error:%s\n", IMG_GetError());
			}
			else {
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
		}
	}
	return success;
}

bool loadMedia() {
	//loading success flag
	bool success = true;

	//load splash image
	gPNGSurface = loadSurface("NavMain.png");
	if (gPNGSurface == NULL) {
		printf("Failed to load PNG image\n");
		success = false;
	}

	//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("ButPress.bmp");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
		printf("Failed to load default image!\n");
		success = false;
	}
	//Load up surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("Up.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL) {
			printf("Failed to load up image!\n");
			success = false;
		}
	//Load down surface 
		gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("Down.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL) {
			printf("Failed to load down image!\n");
			success = false;
		}
	//Load left surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("Left.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL) {
			printf("Failed to load left image!\n");
			success = false;
		}
	//Load right surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("Right.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL) {
			printf("Failed to load right image!\n");
			success = false;
		}
	return success;
}

void close() {
	//Deallocate surfaces
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++) {
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}

	SDL_FreeSurface(gPNGSurface);
	gPNGSurface = NULL;

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Surface* loadSurface(std::string path) {
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), IMG_GetError());
	}
	else {
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return optimizedSurface;
}

int main(int argc, char* args[]) {
	//Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		//Load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {
			//Main loop flag
			bool quit = false;
			//Event handler
			SDL_Event e;
			//Set default current surface
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			while (!quit) {

				while (SDL_PollEvent(&e) != 0) {

					if (e.type == SDL_QUIT) {

						quit = true;

					}
				
					else if (e.type == SDL_KEYDOWN) {
						switch (e.key.keysym.sym) {
						case SDLK_UP:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
							break;
						case SDLK_DOWN:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
							break;
						case SDLK_LEFT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
							break;
						case SDLK_RIGHT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
							break;

						default:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							break;
						}
					}
					SDL_Rect stretchRect;
					stretchRect.x = 0;
					stretchRect.y = 0;
					stretchRect.w = SCREEN_WIDTH;
					stretchRect.h = SCREEN_HEIGHT;
					SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);
				}
				//apply the image
				SDL_BlitSurface(gPNGSurface, NULL, gScreenSurface, NULL);
				//Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}
	close();
	return 0;
}
