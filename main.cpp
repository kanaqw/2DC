#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
#include <stdio.h>
#include <string>
#include <sstream>
//TODO create new renderer for each window
const int MAP_WIDTH = 5000;
const int MAP_HEIGHT = 5000;

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;

class Dot {
public:
	//dot dimensions
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;
	//Init the variables
	Dot();
	//Takes key presses and adjusts the dot velocity
	void handleEvent(SDL_Event& e);
	//Moves the dot
	void move(SDL_Rect& wall);
	//shows the dot on screen
	void render(int camX, int camY, bool dotRenderFlag);
	//position accessors
	int getPosX();
	int getPosY();
	//The X and Y offsets of the dot
	int mPosX, mPosY;
	//velocity of the dot
	int mVelX, mVelY;
private: 
	SDL_Rect mCollider;
};

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
//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);
/////////////////////////////////////////////GLOBAL VARIABLES///////////////////////////////////////////////////
//Loads individual texture as image
SDL_Texture* loadTexture(std::string path);
SDL_Surface* loadSurface(std::string path);
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRendererMain = NULL;
//Globally used font
TTF_Font* gFont = NULL;
//Current displayed texture
SDL_Texture* gTexture = NULL;
//Map texture
LTexture gMapLeft;
LTexture gMapRight;
//text block texture
LTexture gTextBlock;
//Keys anim pointer
SDL_Texture* gKeys = NULL;
SDL_Texture* gSeederIconLeft = NULL;
SDL_Texture* gSeederIconRight = NULL;
//the image that correspond to a keypress
SDL_Texture* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
//Walking animation
const int WALKING_ANIMATION_FRAMES = 4;
SDL_Rect gSpriteClipsLeft[WALKING_ANIMATION_FRAMES];
SDL_Rect gSpriteClipsRight[WALKING_ANIMATION_FRAMES];
//Textures
LTexture gSeederIconTexture;
LTexture gSeederMiniIconTexture;
LTexture gMapTexture;
LTexture gDotTexture;
LTexture gBGTexture;
LTexture gTimeTextTexture;
LTexture gPromptTextTexture;
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
		newTexture = SDL_CreateTextureFromSurface(gRendererMain, loadedSurface);
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

/*bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	//Get rid of preexisting texture
	free();
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error:%s\n", TTF_GetError());
	}
	else {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRendererMain, textSurface);
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
}*/ 
//text texture load, not used


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
	SDL_RenderCopyEx(gRendererMain, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

Dot::Dot() {
	//init offsets
	mPosX = 275;
	mPosY = 460;
	//Set collision box dimension
	mCollider.w = DOT_WIDTH;
	mCollider.h = DOT_HEIGHT;
	//init velocity
	mVelX = 0;
	mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e) {
	//if a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		//Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN:mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	//If a key was released 
	else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
		//Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN:mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move(SDL_Rect& wall) {
	//Move the dot left or right. Velocity 5x lesser
	mPosX += mVelX/5;
	mCollider.x = mPosX;
	//if the dot went too far to left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > MAP_WIDTH)|| checkCollision(mCollider, wall)) {
		//move back
		mPosX -= mVelX/5;
		mCollider.x = mPosX;
	}
	//Move the dot up or down
	mPosY += mVelY/5;
	mCollider.y = mPosY;
	//if the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > MAP_HEIGHT)|| checkCollision(mCollider, wall)) {
		//Move back
		mPosY -= mVelY/5;
		mCollider.y = mPosY;
	}
}

bool checkCollision(SDL_Rect a, SDL_Rect b) {
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;
	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//if any of the sides from A are outside of B
	if (bottomA <= topB) {
		return false;
	}
	if (topA >= bottomB) {
		return false;
	}
	if (rightA <= leftB) {
		return false;
	}
	if (leftA >= rightB) {
		return false;
	}
	//If none of the sides from A are outside B
	return true;

}

void Dot::render(int camX, int camY, bool dotRenderFlag) {
	//Show the dot relative to camera
	if (dotRenderFlag) {
		gDotTexture.render(mPosX-1, mPosY-3);
	}
	gDotTexture.render(mPosX - camX, mPosY - camY);
}

int Dot::getPosX() {
	return mPosX;
}

int Dot::getPosY() {
	return mPosY;
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
			gRendererMain = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
			if (gRendererMain == NULL) {
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
					SDL_SetRenderDrawColor(gRendererMain, 0xFF, 0xFF, 0xFF, 0xFF);
				
			}
		}
	}
	return success;
}

bool loadMedia() {
	//loading success flag
	bool success = true;
	//Load dot texture
	if (!gDotTexture.loadFromFile("SeederIconMini.png")) {
		printf("Failed to load dot texture\n");
		success = false;
	}
	//open the font - not used
	/*gFont = TTF_OpenFont("LTYPE.TTF", 18);
	if (gFont == NULL) {
		printf("Failed to load lazy font! SDL_ttf Error %s\n", TTF_GetError());
		success = false;
	}
	else {
		//Render text
		SDL_Color textColor{ 255,0,0 };
		std::string Text = "Press Enter to Reset Start time.";
		if (!gTextBlock.loadFromRenderedText(Text, textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}*/
	//load PNG texture
	gTexture = loadTexture("NavMainTrans.png");
	if (gTexture == NULL) {
		printf("Failed to load texture image!\n");
		success = false;
	}
	//Load sprite animation texture
	if (!gSeederIconTexture.loadFromFile("SeederIconTexture.png")) {
		printf("Failed to load left icon animation texture!\n");
		success = false;
	}
	else {
		gSpriteClipsLeft[0].x = 4; gSpriteClipsLeft[0].y = 0; gSpriteClipsLeft[0].w = 134; gSpriteClipsLeft[0].h = 99;
		gSpriteClipsLeft[1].x = 4; gSpriteClipsLeft[1].y = 105; gSpriteClipsLeft[1].w = 134; gSpriteClipsLeft[1].h = 99;
		gSpriteClipsLeft[2].x = 4; gSpriteClipsLeft[2].y = 210; gSpriteClipsLeft[2].w = 134; gSpriteClipsLeft[2].h = 99;
		gSpriteClipsLeft[3].x = 4; gSpriteClipsLeft[3].y = 315; gSpriteClipsLeft[3].w = 134; gSpriteClipsLeft[3].h = 99;
	}

	if (!gSeederMiniIconTexture.loadFromFile("SeederIconMiniTexture.png")) {
		printf("Failed to load right icon animation texture!\n ");
		success = false;
	}
	else {
		gSpriteClipsRight[0].x = 1; gSpriteClipsRight[0].y = 0; gSpriteClipsRight[0].w = 75; gSpriteClipsRight[0].h = 53;
		gSpriteClipsRight[1].x = 1; gSpriteClipsRight[1].y = 55; gSpriteClipsRight[1].w = 75; gSpriteClipsRight[1].h = 53;
		gSpriteClipsRight[2].x = 1; gSpriteClipsRight[2].y = 111; gSpriteClipsRight[2].w = 75; gSpriteClipsRight[2].h = 53;
		gSpriteClipsRight[3].x = 1; gSpriteClipsRight[3].y = 168; gSpriteClipsRight[3].w = 75; gSpriteClipsRight[3].h = 53;
	}

	
	//map
	
	if (!gMapLeft.loadFromFile("MapLeft.png")) {
		printf("Failed to load map texture!\n");
		success = false;
	}

	if (!gMapRight.loadFromFile("MapRight.png")) {
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
	//Free loaded textures
	gTimeTextTexture.free();
	gPromptTextTexture.free();
	//Free loaded image
	SDL_DestroyTexture(gTexture);
	gTextBlock.free();
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;
	//destroy window
	SDL_DestroyRenderer(gRendererMain);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRendererMain = NULL;


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
		newTexture = SDL_CreateTextureFromSurface(gRendererMain, loadedSurface);
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
			//Timer
			SDL_Color textColor = { 0,0,0,255 };
			Uint32 startTime = 0;
			//In memory text stream
			std::stringstream timeText;
			//The dot
			Dot dot;
			
			//set the wall
			SDL_Rect wall;
			wall.x = 527;
			wall.y = 5;
			wall.w = 389;
			wall.h = 560;
			SDL_Rect camera = { wall.x, wall.y, wall.w, wall.h};
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
					else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
						startTime = SDL_GetTicks();
					}
					//Clear screen
					SDL_RenderClear(gRendererMain);
					//Render texture to screen
					SDL_RenderCopy(gRendererMain, gTexture, NULL, NULL);
					
					//Update screen

					/////////////////////////////BACKGROUND//////////////////////////////////////////

					///////////////////////////////SEED MAP///////////////////////////////////////////
					SDL_Rect LeftViewer;
					LeftViewer.x = 7;
					LeftViewer.y = 5;
					LeftViewer.w = 517;
					LeftViewer.h = 387;
					SDL_RenderSetViewport(gRendererMain, &LeftViewer);

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
					//Draw blue line
					SDL_SetRenderDrawColor(gRendererMain, 0x00, 0x00, 0xFF, 0xFF);
					SDL_RenderDrawLine(gRendererMain, 0, 199, 524, 199);
					SDL_RenderDrawLine(gRendererMain, 265.5, 0, 265.5, 392);

					//////////////////////////////SEED RIGHT VIEW///////////////////////////////////
					SDL_Rect RightViewer;
					RightViewer.x = wall.x;
					RightViewer.y = wall.y;
					RightViewer.w = wall.w;
					RightViewer.h = wall.h;
				
					SDL_RenderSetViewport(gRendererMain, &RightViewer);
					gMapRight.render(0, 0, NULL, NULL, NULL, flipType);

					///////////////////////////RENDERED SHAPES//////////////////////////////////////
					//vertical yellow dot line
					SDL_SetRenderDrawColor(gRendererMain, 0xFF, 0xFF, 0xFF, 0xFF);

					SDL_Rect BackViewer;
					BackViewer.x = 0;
					BackViewer.y = 0;
					BackViewer.w = SCREEN_WIDTH;
					BackViewer.h = SCREEN_HEIGHT;
					SDL_RenderSetViewport(gRendererMain, &BackViewer);
					/////////////////////////Seeder Animation////////////////////////////////
					//Set texture based on current keystate
				
					const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
					if (currentKeyStates[SDL_SCANCODE_UP]||(currentKeyStates[SDL_SCANCODE_DOWN])) {
						
						//Render current frame
						gSeederIconLeft = NULL;
						SDL_Rect* iconLeft = &gSpriteClipsLeft[frame / 4];
						gSeederIconRight = NULL;
						SDL_Rect* iconRight = &gSpriteClipsRight[frame / 4];
						//dot.render(false);
						gSeederIconTexture.render(206, 120, iconLeft);
						gSeederMiniIconTexture.render(dot.mPosX+RightViewer.x,dot.mPosY,iconRight);
						SDL_Rect icon;
						//go to next frame
						++frame;
						//cycle animation
						if (frame / 4 >= WALKING_ANIMATION_FRAMES) {
							frame = 0;
						} 
					}
					else {
						gSeederIconLeft = loadTexture("SeederIcon2.png");
						SDL_RenderSetViewport(gRendererMain, &wall);
						gMapRight.render(wall.x, wall.y, &camera);
						dot.render(camera.x, camera.y, !quit);//The one thats need to be up top
						SDL_RenderSetViewport(gRendererMain, &BackViewer);
					}
					
					/////////////////////////////Seeder Icon left////////////////////////////////////
					SDL_RenderCopy(gRendererMain, gTexture, NULL, NULL);
					SDL_Rect iconleft;
					iconleft.x = 205;
					iconleft.y = 120;
					iconleft.w = 135;
					iconleft.h = 99;
					SDL_RenderSetViewport(gRendererMain, &iconleft);
					SDL_RenderCopy(gRendererMain, gSeederIconLeft, NULL, NULL);
					SDL_UpdateWindowSurface(gWindow);
					
					//////////////////////////////////Dot//////////////////////////////////////////////
					dot.handleEvent(e);
					dot.move(wall);
					//Center camera over the dot
					camera.x = (dot.getPosX() + Dot::DOT_WIDTH / 2) - RightViewer.x;
					camera.y = (dot.getPosY() + Dot::DOT_HEIGHT / 2) - RightViewer.y;

					//Keep the camera in bonds
					if (camera.x < 0) {
						camera.x = 0;
					}
					if (camera.y < 0) {
						camera.y = 0;
					}
					if (camera.x > MAP_WIDTH - camera.w) {
						camera.x = MAP_WIDTH - camera.w;
					}
					if (camera.y < MAP_HEIGHT - camera.h) {
						camera.y = MAP_HEIGHT - camera.h;
					}
					

					
				}
				//render wall
				SDL_RenderDrawRect(gRendererMain, &wall);
				//render dot
					//DOT
				
					SDL_RenderPresent(gRendererMain);
					//Update the surface
					//SDL_UpdateWindowSurface(gWindow);
				}
			}
			close();
			return 0;
		}
	
