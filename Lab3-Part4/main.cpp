#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cmath>

#include "SDL.h"

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

#define NUM_SPRITES     4
#define MAX_SPEED       1

SDL_Texture* sprite;
SDL_Texture* Sprite1; //alien
SDL_Texture* Sprite2; //cannon
SDL_Texture* Sprite3; //fireball
SDL_Texture* Sprite4; //cannonball

SDL_Rect positions[NUM_SPRITES];
SDL_Rect velocities[NUM_SPRITES];
int sprite_w, sprite_h;

//initialize joystick pointer
SDL_Joystick *joy1 = NULL;

//load sprites
int LoadSprite(char *file, SDL_Renderer *renderer, SDL_Texture* &someSprite)
{
	SDL_Surface *temp;

	// Load the sprite image
	temp = SDL_LoadBMP(file);
	if (temp == NULL) {
		fprintf(stderr, "Couldn't load %s: %s", file, SDL_GetError());
		return (-1);
	}
	sprite_w = temp->w;
	sprite_h = temp->h;

	// Set transparent pixel as the pixel at (0,0)
	if (temp->format->palette) {
		SDL_SetColorKey(temp, SDL_TRUE, *(Uint8 *)temp->pixels);
	}
	else {
		switch (temp->format->BitsPerPixel) {
		case 15:
			SDL_SetColorKey(temp, SDL_TRUE, (*(Uint16 *)temp->pixels) & 0x00007FFF);
			break;
		case 16:
			SDL_SetColorKey(temp, SDL_TRUE, *(Uint16 *)temp->pixels);
			break;
		case 24:
			SDL_SetColorKey(temp, SDL_TRUE, (*(Uint32 *)temp->pixels) & 0x00FFFFFF);
			break;
		case 32:
			SDL_SetColorKey(temp, SDL_TRUE, *(Uint32 *)temp->pixels);
			break;
		}
	}

	// Create textures from the image
	someSprite = SDL_CreateTextureFromSurface(renderer, temp);
	if (!someSprite) {
		fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
		SDL_FreeSurface(temp);
		return (-1);
	}
	SDL_FreeSurface(temp);

	return (0);
}

//move sprites
void MoveSprites(SDL_Window * window, SDL_Renderer * renderer)
{
	int i;
	int window_w = WINDOW_WIDTH;
	int window_h = WINDOW_HEIGHT;
	SDL_Rect *position, *velocity;

	// Draw a gray background
	SDL_SetRenderDrawColor(renderer, 0xA0, 0xA0, 0xA0, 0xFF);
	SDL_RenderClear(renderer);

	// Move the sprite, bounce at the wall, and draw
	for (i = 0; i < NUM_SPRITES; ++i) {
		position = &positions[i];
		velocity = &velocities[i];
		position->x += velocity->x;
		if ((position->x < 0) || (position->x >= (window_w - sprite_w))) {
			velocity->x = -velocity->x;
			position->x += velocity->x;
		}
		position->y += velocity->y;
		if ((position->y < 0) || (position->y >= (window_h - sprite_h))) {
			velocity->y = -velocity->y;
			position->y += velocity->y;
		}	

		//check for cannonball collision with wall, if collision, destroy cannonball
		if (positions[3].x <= 0)
		{
			positions[3].x = 1250;
			velocities[3].x = 0;
		}
		//check for fireball collision with wall, if collision, destroy fireball
		if (positions[2].x >= (WINDOW_WIDTH - sprite_w - 30))
		{
			positions[2].x = -200;
			velocities[2].x = 0;
		}
		//check for fireball collision with cannon, if collision, destroy both
		if (positions[2].x >= positions[1].x - sprite_w - 30 &&															//if fireballs x is greater or equal to cannons x - sprite_h and
			(positions[2].y + sprite_h >= positions[1].y && positions[2].y + sprite_h <= positions[1].y + sprite_h		// if fireballs y + sprite h is more than cannons y and less than cannons y + sprite h 
			|| positions[2].y <= positions[1].y + sprite_h && positions[2].y >= positions[1].y))						// or if fireballs y is less than cannons y + sprite h and more than cannons y
		{
			positions[2].x = -200;																					//delete both cannon and fireball
			velocities[2].x = 0;																					//delete both cannon and fireball
			positions[1].x = -200;																					//delete both cannon and fireball
		}
		//check for cannonball collision with alien, if collision, destroy both
		if (positions[3].x <= positions[0].x + sprite_w &&															//if cannonballs x is lesser or equal to aliens x + sprite_h and
			(positions[3].y + sprite_h >= positions[0].y && positions[3].y + sprite_h <= positions[0].y + sprite_h	// if cannonballs y + sprite h is more than aliens y and less than aliens y + sprite h 
			|| positions[3].y <= positions[0].y + sprite_h && positions[3].y >= positions[0].y))					// or if cannonballs y is less than aliens y + sprite h and more than aliens y
		{
			positions[3].x = -200;																					//delete both alien and cannonball
			velocities[3].x = 0;																					//delete both alien and cannonball
			positions[0].x = -200;																					//delete both alien and cannonball
		}

		// Blit the sprite onto the screen
		if (i == 0) {
			SDL_RenderCopy(renderer, Sprite1, NULL, position);
		}
		else if (i == 1) {
			SDL_RenderCopy(renderer, Sprite2, NULL, position);
		}
		else if (i == 2) {
			SDL_RenderCopy(renderer, Sprite3, NULL, position);
		}
		else if (i == 3) {
			SDL_RenderCopy(renderer, Sprite4, NULL, position);
		}
			
	}

	// Update the screen!
	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	int i, done;
	SDL_Event event;

	window = SDL_CreateWindow("Basic SDL Sprites",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (!window) {
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		return false;
	}

	//load the sprites
	if (LoadSprite("alien.bmp", renderer, Sprite1) < 0) {	//loads the alien image to sprite 1
		return false;
	}

	positions[0].x = 0;										//sets sprite x position
	positions[0].y = WINDOW_HEIGHT / 2 - (sprite_h / 2);	//sets sprite y position
	positions[0].w = sprite_w;								//sets sprites width
	positions[0].h = sprite_h;								//sets sprites height
	velocities[0].x = 0;									//sets velocity in x direction
	velocities[0].y = 0;									//sets veloctity in y direction

	if (LoadSprite("cannon.bmp", renderer, Sprite2) < 0) {	//loads the cannon image to sprite 2
		return false;
	}

	positions[1].x = WINDOW_WIDTH - sprite_w;				//sets sprite x position
	positions[1].y = WINDOW_HEIGHT / 2 - (sprite_h / 2);	//sets sprite y position
	positions[1].w = sprite_w;								//sets sprites width
	positions[1].h = sprite_h;								//sets sprites height
	velocities[1].x = 0;									//sets velocity in x direction
	velocities[1].y = 0;									//sets veloctity in y direction

	if (LoadSprite("fireball.bmp", renderer, Sprite3) < 0) {	//loads the fireball image to sprite 3
		return false;
	}

	positions[2].x = -100;									//same as sprite 1 and 2
	positions[2].y = WINDOW_HEIGHT / 2 - (sprite_h / 2);
	positions[2].w = sprite_w;
	positions[2].h = sprite_h;
	velocities[2].x = 0;
	velocities[2].y = 0;

	if (LoadSprite("cannonball.bmp", renderer, Sprite4) < 0) {	//loads the cannonball image to sprite 4
		return false;
	}

	positions[3].x = -100;									//same as sprite 1 and 2
	positions[3].y = WINDOW_HEIGHT / 2 - (sprite_h / 2);
	positions[3].w = sprite_w;
	positions[3].h = sprite_h;
	velocities[3].x = 0;
	velocities[3].y = 0;
	
	//intialize SDL joystick
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	//open an SDL joystick if controller/joystick is connected
	if (SDL_NumJoysticks() > 0)
	{
		joy1 = SDL_JoystickOpen(0);
	}
	

	// Main render loop
	done = 0;
	while (!done) {
		// Check for events
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{

			case SDL_QUIT:
				done = 1;
				break;
			case SDL_KEYDOWN:
				done = 1;	//quits on x button press
				break;
			case SDL_MOUSEMOTION:
				positions[0].y = event.motion.y;	//sets aliens y coordinate to match the mouse's y coordinate
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("Mouse button %d pressed at (%d,%d)\n",	
					event.button.button, event.button.x, event.button.y);
				printf("Mouse button pressed \n");
				if (event.button.button == 1) // Button click on mouse
				{
					//shoot fireball				
					positions[2].x = positions[0].x; //set fireball x coordinate to aliens position
					positions[2].y = positions[0].y; //set fireball y coordinate to aliens position
					velocities[2].y = 0;			//set fireball y direction speed
					velocities[2].x = 1;			//set fireball x direction speed

				}

				break;
			case SDL_JOYDEVICEREMOVED:
				printf("BYE BYE JOYSTICK");	//print to console when joystick is disconnected
				printf("Joy number %d\n", event.jbutton.which);	//print which joystick is disconnected to the console
			case SDL_JOYBUTTONDOWN:
				printf("Joy number %d\n", event.jbutton.which);	//print to console which joystick pressed a button
				printf("Button %d\n", event.jbutton.button);	//print to console what button is pressed
				if (event.jbutton.which == 0 && event.jbutton.button == 0) // Button 1 on 1st Joystick
				{
					//shoot cannonall				
					positions[3].x = positions[1].x; //set cannonball x position to cannons position
					positions[3].y = positions[1].y; //set cannonball y position to cannons position
					velocities[3].y = 0;			//set cannonball y direction speed
					velocities[3].x = -1;			//set cannonball x direction speed
				}
				else if (event.jbutton.which == 0 && event.jbutton.button == 1)  // Button 2 on 1st Joystick
				{
					done = 1; //quit
				}
				break;
			case SDL_JOYAXISMOTION:
			{
				if (event.jaxis.axis == 0) { //print to console x coordinate value
					printf("x = %d\n", event.jaxis.value);// positions[0].x);
				}
				if (event.jaxis.axis == 1) { //print to console y coordinate value
					printf("y = %d\n", event.jaxis.value);//positions[0].y);
				}
			}
			}
		}
		if (joy1)
		{
			positions[1].y += SDL_JoystickGetAxis(joy1, 1) / 6000; //joystick y coordinate move sprite 2's y coordinate

			if (positions[0].y > WINDOW_HEIGHT - sprite_h)	//keeps sprite inside the window
				positions[0].y = WINDOW_HEIGHT - sprite_h;
			if (positions[0].y < 0)
				positions[0].y = 0;

		}
		MoveSprites(window, renderer);
		SDL_Delay(3);
	}

	SDL_Quit();

	return true;
}
