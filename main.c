#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_X (3840)
#define WINDOW_Y (2160)
#define SPRITE_ORIENTATIONS (72)

struct Vector2
{
	float x;
	float y;
};

struct Object
{
	struct Vector2 pos;
	struct Vector2 vel;
	float rot;
	int spriteSize;
	SDL_Surface* spriteSheet;
};

int rotToFrame(float rot) { return (int)(rot  / (360.0 / (float)SPRITE_ORIENTATIONS) + 0.5) % SPRITE_ORIENTATIONS; }

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("My SDL2 Window",
		                                  SDL_WINDOWPOS_UNDEFINED,
		                                  SDL_WINDOWPOS_UNDEFINED,
		                                  WINDOW_X, WINDOW_Y,
		                                  SDL_WINDOW_SHOWN);
		                                  
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		                                
	if (!window) {
		printf("Failed to create SDL window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("Failed to create SDL renderer: %s\n", SDL_GetError());
		return 1;
	}

	int FrameRate = 60;
	int ClocksPerFrame = CLOCKS_PER_SEC / FrameRate;
	int frame = 0;
	
	//Test monkey heads preparation
    SDL_Surface *images[SPRITE_ORIENTATIONS];
    for(int i = 0; i < SPRITE_ORIENTATIONS; i++)
    {
    	char spritelocation[] = "sprites/monkey/0001.png";
    	spritelocation[17] = 48 + (i+ 1)/10; //to ascii number
    	spritelocation[18] = 48 + ((i+ 1)%10); //to ascii number
    	images[i]= IMG_Load(spritelocation);
    }

	//TEST OBJECT
	struct Object testObject;
	testObject.pos.x = WINDOW_X/2;
	testObject.pos.y = WINDOW_Y/2;
	testObject.vel.x = 5;
	testObject.vel.y = 0;
	testObject.rot = 0;
	testObject.spriteSize = 128;
	testObject.spriteSheet = IMG_Load("sprites/test_object/spritesheet.png");

	
	while(1)
	{
		clock_t FrameStartClock = clock();
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer); //erase

		//Rotating monkey heads
		for(int x = 0; x < 2; x++)
		{
			for(int y = 0; y < 2; y++)
			{
				int animFrame = ((frame%(SPRITE_ORIENTATIONS*4)/4) + (SPRITE_ORIENTATIONS/4*(x+y*2))) % SPRITE_ORIENTATIONS;
				SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, images[animFrame]);
				SDL_Rect dstrect = { (WINDOW_X-256)*x, (WINDOW_Y-256)*y, 256, 256 };
				SDL_RenderCopy(renderer, texture, NULL, &dstrect);
			}
		}

		//TEST OBJECT
		testObject.rot = ((int)testObject.rot + 1) % 360;
		int rotFrame = rotToFrame(testObject.rot);
		printf("rotf %i, rot %f ", rotFrame, testObject.rot);
		int animFrame = frame%119-59;
		if(animFrame < 0)
		{
			animFrame *= -1;
		}
		testObject.pos.x += testObject.vel.x;
		testObject.pos.y += testObject.vel.y;
		SDL_Surface *img = SDL_CreateRGBSurface(0, testObject.spriteSize, testObject.spriteSize, 32, 0, 0, 0, 0);
		SDL_Rect srcrect = { rotFrame*testObject.spriteSize, animFrame*testObject.spriteSize, 128, 128 };
		SDL_Rect dstrect = { 0, 0, 0, 0 };
		SDL_BlitSurface(testObject.spriteSheet, &srcrect, img, &dstrect);
		SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
		dstrect.x = testObject.pos.x;
		dstrect.y = testObject.pos.y;
		dstrect.w = 256;
		dstrect.h = 256;
		SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		if(testObject.pos.x >= (WINDOW_X - 256) || testObject.pos.x <= 0)
		{
			testObject.vel.x *= -1;
		}

		SDL_RenderPresent(renderer);

		SDL_Event event;
		int quit = 0;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
				    quit = 1;
				    break;
				// handle other events...
			}
			
			if(quit)
			{
				break;
			}
		}
		
		if(quit)
		{
			break;
		}
		
		printf("frame: %i\n", frame);
		
		//Sleep until we have taken up enough time.
		int ClocksThisFrame = clock()-FrameStartClock;
		int ClocksToSleep = ClocksPerFrame - ClocksThisFrame;
		if(ClocksToSleep > 0)
		{
			usleep((int)(((double)ClocksToSleep) / ClocksPerFrame / FrameRate * 1000000));
		}
		else
		{
			printf("FRAME TOO LONG!");
		}
		
		frame++;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
