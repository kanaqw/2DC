#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
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
	//Creates image from string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
	//dealloc textures
	void free();
	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	//Set blending
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
	//Renders textures at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0 ,SDL_Point*center = NULL,
				SDL_RendererFlip flip = SDL_FLIP_NONE);
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
/////////////////////////////////////////////GLOBAL VARIABLES///////////////////////////////////////////////////
//Loads individual texture as image
SDL_Texture* loadTexture(std::string path);
SDL_Surface* loadSurface(std::string path);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Globally used font
TTF_Font* gFont = NULL;
//Current displayed texture
SDL_Texture* gTexture = NULL;
//Map texture
LTexture gMapLeft;
//text block texture
LTexture gTextBlock;
//Keys anim pointer
SDL_Texture* gKeys = NULL;
SDL_Texture* gSeederIcon = NULL;
//the image that correspond to a keypress
SDL_Texture* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;
LTexture gMapTexture;
///////////////////////////////////////////////END OF GV//////////////////////////////////////////////////////////
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

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	//Get rid of preexisting texture
	free();
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error:%s\n", TTF_GetError());
	}
	else {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL_Error%s\n", SDL_GetError());
		}
		else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		SDL_FreeSurface(textSurface);
	}
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

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	SDL_SetTextureAlphaMod(mTexture, alpha);
}
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x,y,mWidth, mHeight };
	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
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
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error %s\n", SDL_GetError());
				success = false;
			}
			else {
				//init PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_Image could not be initialize! SDL_Image Error: %s\n", IMG_GetError());
					success = false;
				}
					if (TTF_Init() == -1) {
						printf("SDL_ttf could not be initialize! SDL_ttf Error: %s\n", TTF_GetError());
						success = false;
					}
					//Init renderer color
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				
			}
		}
	}
	return success;
}

bool loadMedia() {
	//loading success flag
	bool success = true;
	//open the font
	gFont = TTF_OpenFont("LTYPE.TTF", 18);
	if (gFont == NULL) {
		printf("Failed to load lazy font! SDL_ttf Error %s\n", TTF_GetError());
		success = false;
	}
	else {
		//Render text
		SDL_Color textColor{ 255,0,0 };
		std::string Text = "Text check";
		if (!gTextBlock.loadFromRenderedText(Text,textColor ))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}
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
	else {
		gSpriteClips[0].x = 4; gSpriteClips[0].y = 0; gSpriteClips[0].w = 134; gSpriteClips[0].h = 99;

		gSpriteClips[1].x = 4; gSpriteClips[1].y = 105; gSpriteClips[1].w = 134; gSpriteClips[1].h = 99;

		gSpriteClips[2].x = 4; gSpriteClips[2].y = 210; gSpriteClips[2].w = 134; gSpriteClips[2].h = 99;

		gSpriteClips[3].x = 4; gSpriteClips[3].y = 315; gSpriteClips[3].w = 134; gSpriteClips[3].h = 99;
	}
	//map
	
	if (!gMapLeft.loadFromFile("MapLeft.png")) {
		printf("Failed to load map texture!\n");
		success = false;
	}

	//Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadTexture("Un.png");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}

	//Load up surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadTexture("UpDownButton.png");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("Failed to load up image!\n");
		success = false;
	}

	//Load down surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadTexture("UpDownButton.png");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("Failed to load down image!\n");
		success = false;
	}

	//Load left surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadTexture("leftRight.png");
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("Failed to load left image!\n");
		success = false;
	}

	//Load right surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadTexture("leftRight.png");
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
	gTextBlock.free();
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
	//destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;


	//Quit SDL subsystems
	IMG_Quit();
	TTF_Quit();
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
	else if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {
			//Main loop flag
			bool quit = false;
			//Event handler
			SDL_Event e;
			//Current rendered texture
			LTexture* currentTexture = NULL;
			int frame = 0;
			double degrees = 0;
			SDL_RendererFlip flipType = SDL_FLIP_NONE;
			gKeys = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			//while app still running
			while (!quit) {
				//handle events on queue
				while (SDL_PollEvent(&e) != 0) {

					if (e.type == SDL_QUIT) {

						quit = true;

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
					LeftViewer.w = 517;
					LeftViewer.h = 387;
					SDL_RenderSetViewport(gRenderer, &LeftViewer);

					//Render texture to screen
					gMapLeft.render(-200, -200, NULL, degrees, NULL, flipType);

					////////////////////////////////Rotating map//////////////////////////////
					if (e.type == SDL_KEYDOWN) {
						switch (e.key.keysym.sym) {
						case SDLK_LEFT:
							degrees -= 5;
							break;
						case SDLK_RIGHT:
							degrees += 5;
							break;

						}
					}
					gMapTexture.render((SCREEN_WIDTH - gMapTexture.getWidth() / 2),
						(SCREEN_HEIGHT - gMapTexture.getHeight()) / 2, NULL, degrees, NULL, flipType);

					///////////////////////////RENDERED SHAPES//////////////////////////////////////


					//Draw blue line
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
					SDL_RenderDrawLine(gRenderer, 0, 199, 524, 199);
					SDL_RenderDrawLine(gRenderer, 265.5, 0, 265.5, 392);

					//vertical yellow dot line
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

					SDL_Rect BackViewer;
					BackViewer.x = 0;
					BackViewer.y = 0;
					BackViewer.w = SCREEN_WIDTH;
					BackViewer.h = SCREEN_HEIGHT;
					SDL_RenderSetViewport(gRenderer, &BackViewer);
					/////////////////////////Seeder Animation////////////////////////////////
					//Set texture based on current keystate
					const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
					if (currentKeyStates[SDL_SCANCODE_UP]) {
						//Render current frame
						gSeederIcon = NULL;
						SDL_Rect* currentClip = &gSpriteClips[frame / 4];
						gSpriteSheetTexture.render(206, 120, currentClip);
						SDL_Rect icon;
						//go to next frame
						++frame;

						//cycle animation
						if (frame / 4 >= WALKING_ANIMATION_FRAMES) {
							frame = 0;
						}
					} else {
						gSeederIcon = loadTexture("SeederIcon2.png");
					}
					/////////////////////////////Seeder Icon left////////////////////////////////////
					SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
					SDL_Rect iconleft;
					iconleft.x = 205;
					iconleft.y = 120;
					iconleft.w = 135;
					iconleft.h = 99;
					SDL_RenderSetViewport(gRenderer, &iconleft);
					SDL_RenderCopy(gRenderer, gSeederIcon, NULL, NULL);
					SDL_UpdateWindowSurface(gWindow);
				}
					SDL_RenderPresent(gRenderer);
					//Update the surface
					SDL_UpdateWindowSurface(gWindow);
				}
			}
			close();
			return 0;
		}
	
