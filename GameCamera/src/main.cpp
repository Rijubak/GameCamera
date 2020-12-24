#include<SDL.h>
#include<SDL_image.h>
#include<string>
#include<Windows.h>

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class CustomTexture
{
public:
	SDL_Texture* mTexture;
	int mWidth, mHeight;

	CustomTexture()
	{
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}

	void render(int x, int y)
	{
		SDL_Rect dstRect = { x, y, mWidth, mHeight };
		SDL_RenderCopy(gRenderer, mTexture, NULL, &dstRect);
	}
	
	void render(int x, int y, SDL_Rect& clip)
	{
		SDL_RenderCopy(gRenderer, mTexture, &clip, NULL);
	}

	void free()
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}
};


CustomTexture gDotTexture;
CustomTexture gBGTexture;

class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 5;

	//Initializes the variables
	Dot();

	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	//Moves the dot
	void move();

	//Shows the dot on the screen relative to the camera
	void render(int camX, int camY);
	void render();

	//Position accessors
	int getPosX();
	int getPosY();

private:
	//The X and Y offsets of the dot
	int mPosX, mPosY;

	//The velocity of the dot
	int mVelX, mVelY;
};

Dot::Dot()
{
	mPosX = 0;
	mPosY = 0;
	mVelX = 0, mVelY = 0;

}

void Dot::render()
{
	gDotTexture.render(mPosX, mPosY);
}

void Dot::render(int camX, int camY)
{
	gDotTexture.render(mPosX - camX, mPosY - camY);
}
void Dot::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{

		switch (e.key.keysym.sym)
		{
			case SDLK_UP:  mVelY -= DOT_VEL; break;
			case SDLK_DOWN: mVelY += DOT_VEL; break;
			case SDLK_LEFT: mVelX -= DOT_VEL; break;
			case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
	
	if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{

		switch (e.key.keysym.sym)
		{
		case SDLK_UP:  mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move()
{
	mPosX += mVelX;
	if (mPosX < 0 || (mPosX + DOT_WIDTH > LEVEL_WIDTH))
	{
		mPosX -= mVelX;
	}
	mPosY += mVelY;
	if (mPosY < 0 || (mPosY + DOT_HEIGHT > LEVEL_HEIGHT))
	{
		mPosY -= mVelY;
	}
}

int Dot::getPosX()
{
	return mPosX;
}

int Dot::getPosY()
{
	return mPosY;
}

void close()
{
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	gDotTexture.free();
	SDL_Quit();
	IMG_Quit();
}

int main(int argc, char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	gWindow = SDL_CreateWindow("game_cam", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	
	std::string basepath = std::string(SDL_GetBasePath()) + "..\\..\\GameCamera\\assets\\";
	std::string dot_path = basepath + "dot.bmp";
	SDL_Surface* loaded_surface = IMG_Load(dot_path.c_str());
	if (!loaded_surface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		return -1;
	}

	SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0, 255, 255));
	gDotTexture.mTexture = SDL_CreateTextureFromSurface(gRenderer, loaded_surface);
	gDotTexture.mWidth = loaded_surface->w;
	gDotTexture.mHeight = loaded_surface->h;
	std::string bgpath = basepath + "bg.png";
	loaded_surface = IMG_Load(bgpath.c_str());
	if (!loaded_surface) {
		printf("IMG_Load: %s\n", IMG_GetError());
		return -1;
	}
	gBGTexture.mTexture = SDL_CreateTextureFromSurface(gRenderer, loaded_surface);
	gBGTexture.mWidth = loaded_surface->w;
	gBGTexture.mHeight = loaded_surface->h;
	SDL_FreeSurface(loaded_surface);


	SDL_Event e;
	bool quit = false;
	Dot dot;
	//SDL_Rect wall = { 300, 40, 40, 400 };
	
	SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			dot.handleEvent(e);
		}
		dot.move();
		
		camera.x = (dot.getPosX() + Dot::DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
		if (camera.x < 0)
		{
			camera.x = 0;
		}
		else if (camera.x + camera.w > LEVEL_WIDTH)
		{
			camera.x = LEVEL_WIDTH - camera.w;
		}
		camera.y = (dot.getPosY() + Dot::DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;
		if (camera.y < 0)
		{
			camera.y = 0;
		}
		else if (camera.y + camera.h > LEVEL_HEIGHT)
		{
			camera.y = LEVEL_HEIGHT - camera.h;
		}

		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		SDL_RenderClear(gRenderer);
		
		//dot.render();
		gBGTexture.render(0, 0, camera);
		dot.render(camera.x, camera.y);
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		//SDL_RenderDrawRect(gRenderer, &wall);
		SDL_RenderPresent(gRenderer);
	}

	close();
	return 0;
}