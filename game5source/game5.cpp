#define OLC_PGE_APPLICATION
#include "../shared/olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Flappy Bird"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;

  struct Block {
    olc::vf2d position;
    olc::vf2d velocity;
    olc::vf2d size;
    olc::Pixel colour;
    Block(int givenX, int givenY, int givenWidth, int givenHeight,
          olc::Pixel givenColour, olc::vf2d givenVelocity) {
      position.x = givenX;
      position.y = givenY;
      size.x = givenWidth;
      size.y = givenHeight;
      colour = givenColour;
      velocity = givenVelocity;
    }
  };

  std::vector<Block> blocks;
  float wallDelay = 1.5;
  float wallTimer = wallDelay;
  int gapSize = 100;
  float gravity = 1000;

  olc::vf2d birdPosition{WINDOW_WIDTH / 4, 100};
  olc::vf2d birdVelocity{0, 0};
  int birdRadius = 15;
  olc::Pixel birdColour = olc::MAGENTA;
  bool birdAlive = true;

 public:
  bool OnUserCreate() override {  // Load resources here
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    timer += fElapsedTime;
    frames++;
    if (timer > 1.0) {
      fps = frames;
      frames = 0;
      timer -= 1;
    }

    inputs();
    processes();
    outputs();

    if (GetKey(olc::Key::ESCAPE).bPressed) {
      return false;
    } else {
      return true;
    }
  }

  void inputs() {  // Game controls goes here

    if (birdAlive && GetKey(olc::Key::SPACE).bPressed) birdVelocity.y = -500;
  }

  void processes() {  // Game logic goes here

    wallTimer -= GetElapsedTime();

    if (wallTimer < 0) {
      wallTimer += wallDelay;

      auto y = gapSize + (rand() % (WINDOW_HEIGHT - 2 * gapSize));

      blocks.push_back(
          Block(WINDOW_WIDTH, 0, 40, y - gapSize, olc::GREEN, {-300, 0}));
      blocks.push_back(Block(WINDOW_WIDTH, y + gapSize, 40,
                             WINDOW_HEIGHT - y - gapSize, olc::GREEN,
                             {-300, 0}));
    }

    birdVelocity.y += gravity * GetElapsedTime();
    birdPosition += birdVelocity * GetElapsedTime();

    if (birdVelocity.y > 0 && birdPosition.y > WINDOW_HEIGHT - birdRadius) {
      birdAlive = false;
    }

    for (auto i = 0; i < blocks.size();) {
      blocks[i].position += blocks[i].velocity * GetElapsedTime();

      if (blocks[i].position.x + blocks[i].size.x < 0) {
        blocks.erase(blocks.begin() + i);
      } else {
        i++;
      }
    }

    for (auto block : blocks) {
      auto gammaX = birdPosition.x - block.position.x;
      auto gammaY = birdPosition.y - block.position.y;

      if (gammaX > -birdRadius && gammaX < block.size.x + birdRadius) {
        if (gammaY > -birdRadius && gammaY < block.size.y + birdRadius) {
          birdAlive = false;
        }
      }
    }
  }

  void outputs() {  // Game graphics drawn here

    Clear(olc::BLACK);

    for (auto block : blocks) {
      FillRect(block.position, block.size, block.colour);
    }

    if (birdAlive) {
      FillCircle(birdPosition, birdRadius, birdColour);
    }

    if (fps > 0) {
      auto fpsPosition = olc::vi2d(WINDOW_WIDTH - 70, WINDOW_HEIGHT - 20);
      DrawStringDecal(fpsPosition, "FPS " + std::to_string(fps));
    }
  }

  bool OnUserDestroy() override {
    std::cout << "Closing game" << std::endl;
    return true;
  }
};

int main() {
  Game game;
  if (game.Construct(WINDOW_WIDTH, WINDOW_HEIGHT, 1, 1)) game.Start();
  return 0;
}
