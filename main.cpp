#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>


const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;

class LTexture {
public:
	//init variables
	LTexture();
	//Dealloc memory
	~LTexture();
	//loads image from spec path
	bool loadFromFile(std::string path);
	//dealloc textures
	void free();
	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	//Set blending
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
	//Renders textures at given point
	void render(int x, int y, SDL_Rect* clip = NULL);
	//Gets image dim
	int getWidth();
	int getHeight();
private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	//Image dimmensions
	int mWidth;
	int mHeight;
};

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

//Loads individual texture as image
SDL_Texture* loadTexture(std::string path);
SDL_Surface* loadSurface(std::string path);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Current displayed texture
SDL_Texture* gTexture = NULL;
SDL_Texture* gMapLeft = NULL;
//Keys anim pointer
SDL_Texture* gKeys = NULL;
SDL_Texture* gSeederIcon = NULL;
//the image that correspond to a keypress
SDL_Texture* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}
LTexture::~LTexture() {
	//Dealocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! Error: %s\n", path.c_str(), IMG_GetError());
	}
	else {
		//Create image from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		} SDL_FreeSurface(loadedSurface);
	}
	mTexture = newTexture;
	return mTexture != NULL;
}


void LTexture::free() {
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode( SDL_BlendMode blending ){
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::render(int x, int y, SDL_Rect* clip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x,y,mWidth, mHeight };
	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

bool init() {
	//init flag
	bool success = true;
	//init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled");
		}
		//create window
		gWindow = SDL_CreateWindow("StuurmanNav v.0.1a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//initialize png loading
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error %s\n", SDL_GetError());
				success = false;
			}
			else {
				//Init renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				//init PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_Image could not be initialize! SDL_Image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia() {
	//loading success flag
	bool success = true;
	//load PNG texture
	gTexture = loadTexture("NavMainTrans.png");
	if (gTexture == NULL) {
		printf("Failed to load texture image!\n");
		success = false;
	}
	//Load sprite animation texture
	 if (!gSpriteSheetTexture.loadFromFile("SeederIconTexture.png")) {
		printf("Failed to load move animation texture!\n");
			success = false;
	 }
	 else { gSeederIcon = loadTexture("SeederIconTexture.png");

		 gSpriteClips[0].x = 4; gSpriteClips[0].y = 0; gSpriteClips[0].w = 134; gSpriteClips[0].h = 97;

		 gSpriteClips[1].x = 4; gSpriteClips[1].y = 105; gSpriteClips[1].w = 134; gSpriteClips[1].h = 97;

		 gSpriteClips[2].x = 4; gSpriteClips[2].y = 210; gSpriteClips[2].w = 134; gSpriteClips[2].h = 97;

		 gSpriteClips[3].x = 4; gSpriteClips[3].y = 315; gSpriteClips[3].w = 134; gSpriteClips[3].h = 97;
	 }
		//map
		gMapLeft = loadTexture("mapLeft.png");
		if (gMapLeft == NULL) {
			printf("Failed to load map texture!\n");
			success = false;
		}
		//SeederIcon
		/*gSeederIcon = loadTexture("SeederIcon2.png");
		if (gSeederIcon == NULL) {
			printf("Failed to load SedIcon texture!\n");
			success = false;
		}*/

		//Load default surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadTexture("ButPress.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
		{
			printf("Failed to load default image!\n");
			success = false;
		}

		//Load up surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadTexture("Up.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
		{
			printf("Failed to load up image!\n");
			success = false;
		}

		//Load down surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadTexture("Down.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
		{
			printf("Failed to load down image!\n");
			success = false;
		}

		//Load left surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadTexture("Left.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
		{
			printf("Failed to load left image!\n");
			success = false;
		}

		//Load right surface
		gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadTexture("Right.bmp");
		if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
		{
			printf("Failed to load right image!\n");
			success = false;
		}
	
	return success;
}

void close() {
	//Free loaded image
	SDL_DestroyTexture(gTexture);
	//destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;


	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

SDL_Surface* loadSurface(std::string path) {
	//Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}

	return loadedSurface;
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
			int frame = 0;
			gKeys = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
			
			//while app still running
			while (!quit) {

				while (SDL_PollEvent(&e) != 0) {

					if (e.type == SDL_QUIT) {

						quit = true;

					}
					else if (e.type == SDL_KEYDOWN) {
						switch (e.key.keysym.sym) {
						case SDLK_UP:
							
							//Render current frame
							SDL_Rect* currentClip = &gSpriteClips[frame / 4];
							gSpriteSheetTexture.render((SCREEN_WIDTH - currentClip->w) / 2, 
									(SCREEN_HEIGHT - currentClip->h) / 2, currentClip);
							//Update screen
							SDL_RenderPresent(gRenderer);
							//go to next frame
							++frame;
							//cycle animation
							if (frame / 4 >= WALKING_ANIMATION_FRAMES) {
								frame = 0;
							}

							break;

						
			
						}
					}
				
				}
					//Clear screen
					SDL_RenderClear(gRenderer);
					//Render texture to screen
					SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
					//Update screen
				
					/////////////////////////////BACKGROUND//////////////////////////////////////////

					
					
				
					
					///////////////////////////////SEED MAP///////////////////////////////////////////
					SDL_Rect LeftViewer;
					LeftViewer.x = 7;
					LeftViewer.y = 5;
					LeftViewer.w = 517 ;
					LeftViewer.h = 387 ;
					SDL_RenderSetViewport(gRenderer, &LeftViewer);

					//Render texture to screen
					SDL_RenderCopy(gRenderer, gMapLeft, NULL, NULL);
					
					

					///////////////////////////RENDERED SHAPES//////////////////////////////////////
					//Red rect left
					/*SDL_Rect fillRectRedLeft = { 221, 135.48, 87.5, 126.04};
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
					SDL_RenderFillRect(gRenderer, &fillRectRedLeft);*/
					
					//Draw blue line
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
					SDL_RenderDrawLine(gRenderer, 0,199,524,199);
					SDL_RenderDrawLine(gRenderer, 265.5, 0, 265.5, 392);
					
					//vertical yellow dot line
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			
					SDL_Rect BackViewer;
					BackViewer.x = 0;
					BackViewer.y = 0;
					BackViewer.w = SCREEN_WIDTH;
					BackViewer.h = SCREEN_HEIGHT;
					SDL_RenderSetViewport(gRenderer, &BackViewer);
					//Render texture to screen
					SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
					SDL_Rect icon;
					icon.x = 220;
					icon.y = 117;
					icon.w = 100;
					icon.h = 100;
					SDL_RenderSetViewport(gRenderer, &icon);
					SDL_RenderCopy(gRenderer, gSeederIcon, NULL, NULL);
					
					SDL_RenderPresent(gRenderer);
				//Update the surface
				SDL_UpdateWindowSurface(gWindow);

			}
		}
	}
	close();
	return 0;
}
