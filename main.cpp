/**
* Project name: Random generated fractal trees
* Author: teaprog
* File created: 02.03.12
*/

#include <SDL.h>
#include <SDL_opengl.h>
#include <cmath>
#include <vector>
using std::vector;

const GLfloat PI = 3.14159265;

int screenWidth = 800;
int screenHeight = 600;
bool quit = false;
SDL_Event event;

void init();
void reshape(int, int);
void display();
void input();

class Tree {
	private:
		vector<GLfloat> xPositions;
		vector<GLfloat> yPositions;
		vector<GLfloat> sizes;
		vector<GLfloat> angles;
		GLfloat treeRed;
		GLfloat treeGreen;
	public:
		Tree() {
			generateFractalTree();
		}

		void drawTree(int lines) {
			if (lines > xPositions.size())
				return;

			for (size_t i = 0; i < lines; i++) {
				glColor3f(1.0f / (sizes.at(i) * treeRed),  1.0f / (sizes.at(i) * treeGreen), 0.0f); 
				glLineWidth(sizes.at(i)/40.0f + 1.0f);
				glPushMatrix();
					glTranslatef(xPositions.at(i), yPositions.at(i), 0.0f);
					glRotatef(angles.at(i), 0.0f, 0.0f, 1.0f);
					glBegin(GL_LINES);
						glVertex2f(0.0f, 0.0f);
						glVertex2f(0.0f, -sizes.at(i));
					glEnd();
				glPopMatrix();
			}
		}

		void generateFractalTree() {
			xPositions.clear();
			yPositions.clear();
			sizes.clear();
			angles.clear();

			int randVarRed = rand() % 3;
			if (randVarRed == 0)
				treeRed = 0.35f;
			else if (randVarRed == 1)
				treeRed = 0.25f;
			else if (randVarRed == 2)
				treeRed = 0.15f;
			
			int randVarGreen = rand() % 2;
			if (randVarGreen == 0)
				treeGreen = 0.15f;
			else if (randVarGreen == 1)
				treeGreen = 0.30f;

			generateFractalTree(screenWidth / 2.0f, screenHeight - 20.0f, (rand() % screenHeight / 4) + 40.0f, 0.0f);
		}

		void generateFractalTree(GLfloat xPos, GLfloat yPos, GLfloat size, GLfloat degrees) {
			if (size < 5.0f || yPos - size < 10.0f)
				return;
	
			static const int maxShrink = 900; // per thousand
			static const int minShrink = 1.0f;
			static const int maxAngle = 45;
			static const int maxBranches = 8;
			static const int minBranches = 0;
			GLfloat newPosX = xPos + size * sin(degrees / 180.0f * PI);
			GLfloat newPosY = yPos - size * cos(degrees / 180.0f * PI);

			xPositions.push_back(xPos);
			yPositions.push_back(yPos);
			sizes.push_back(size);
			angles.push_back(degrees);

			int branches = rand() % (maxBranches-minBranches) + 1 + minBranches;
			for (int i = 0; i < branches; i++) {
				int x = rand() % 2 == 1 ? -1 : 1;
				generateFractalTree(newPosX, newPosY, size * (rand() % maxShrink + minShrink) / 1000.0f, degrees + x * rand() % maxAngle);
			}
		}

		int getTotalLines() {
			return xPositions.size();
		}
} tree;

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WM_SetCaption("02.03.12 Fractal Tree", 0);
	SDL_putenv("SDL_VIDEO_CENTERED=center");
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	reshape(screenWidth, screenHeight);
	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
	glLineWidth(2.0f);
	srand(SDL_GetTicks());
}

void reshape(int screenW, int screenH) {
	screenWidth = screenW;
	screenHeight = screenH;

	SDL_SetVideoMode(screenW, screenH, 32, SDL_OPENGL | SDL_RESIZABLE);
	glViewport(0, 0, screenW, screenH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, screenW, screenH, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void display() {
	static bool newTree = false;
	static int counter = 0;
	static Uint32 timer = SDL_GetTicks();
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	if (counter < tree.getTotalLines()) {
		tree.drawTree(counter);
		counter += 0.005*tree.getTotalLines();
	}
	else {
		if (!newTree)
			timer = SDL_GetTicks();
		tree.drawTree(tree.getTotalLines());
		newTree = true;
	}

	SDL_GL_SwapBuffers();

	if (newTree && SDL_GetTicks() - timer > 500) {
		do {
			tree.generateFractalTree();
		}
		while (0.0001*tree.getTotalLines() < 1);

		counter = 0;
		newTree = false;
	}
}

void input() {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;
				}
				break;
			case SDL_VIDEORESIZE:
				reshape(event.resize.w, event.resize.h);
				break;
		}
	}
}

int main(int argc, char** argv) {
	init();

	while (!quit) {
		input();
		display();
	}

	SDL_Quit();

	return 0;
}