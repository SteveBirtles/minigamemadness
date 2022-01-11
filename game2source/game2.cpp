#define OLC_PGE_APPLICATION
#include "../shared/olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Snake"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;

  struct Block {
    olc::vf2d position;
    olc::vf2d size;
    olc::Pixel colour;
    Block(int givenX, int givenY, int givenWidth, int givenHeight,
          olc::Pixel givenColour) {
      position.x = givenX;
      position.y = givenY;
      size.x = givenWidth;
      size.y = givenHeight;
      colour = givenColour;
    }
  };

  std::vector<Block> snake;
  const int SNAKE_SIZE = 20;
  olc::vi2d snakeVelocity{SNAKE_SIZE, 0};
  float snakeDelay = 0.1;
  float snakeTimer = snakeDelay;
  int snakeLength = 10;
  bool snakeAlive = true;

  olc::vi2d applePosition;
  olc::Pixel appleColour{255, 0, 0};

  void randomizeApple() {
    applePosition =
        olc::vi2d(SNAKE_SIZE * (rand() % int(WINDOW_WIDTH / SNAKE_SIZE)),
                  SNAKE_SIZE * (rand() % int(WINDOW_HEIGHT / SNAKE_SIZE)));
  }

 public:
  bool OnUserCreate() override {  // Load resources here

    snake.push_back(
        Block(100, 100, SNAKE_SIZE, SNAKE_SIZE, olc::Pixel(0, 255, 0)));
    randomizeApple();
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

    if (snakeAlive) {
      if (GetKey(olc::Key::UP).bHeld && snakeVelocity.y == 0)
        snakeVelocity = olc::vi2d(0, -SNAKE_SIZE);
      if (GetKey(olc::Key::DOWN).bHeld && snakeVelocity.y == 0)
        snakeVelocity = olc::vi2d(0, SNAKE_SIZE);
      if (GetKey(olc::Key::LEFT).bHeld && snakeVelocity.x == 0)
        snakeVelocity = olc::vi2d(-SNAKE_SIZE, 0);
      if (GetKey(olc::Key::RIGHT).bHeld && snakeVelocity.x == 0)
        snakeVelocity = olc::vi2d(SNAKE_SIZE, 0);
    }
  }

  void processes() {  // Game logic goes here
    

    snakeTimer -= GetElapsedTime();
    if (snakeTimer < 0) {
      snakeTimer += snakeDelay;
      auto head = snake.size() - 1;
      auto x = snake[head].position.x + snakeVelocity.x;
      auto y = snake[head].position.y + snakeVelocity.y;

      for (auto block : snake) {
        if (block.position.x == x && block.position.y == y) {
          snakeAlive = false;
          break;
        }
      }

      if (snakeAlive) {
        if (x == applePosition.x && y == applePosition.y) {
          snakeLength++;
          randomizeApple();
        }
        snake.push_back(
            Block(x, y, SNAKE_SIZE, SNAKE_SIZE, olc::Pixel(0, 255, 0)));
      }
    }

    if (snake.size() > snakeLength) {
      snake.erase(snake.begin());
    }
  }

  void outputs() {  // Game graphics drawn here

    Clear(olc::BLACK);

    for (auto block : snake) {
      FillRect(block.position, block.size, block.colour);
    }

    auto centreOffset = olc::vi2d{SNAKE_SIZE / 2, SNAKE_SIZE / 2};
    FillCircle(applePosition + centreOffset, SNAKE_SIZE / 2, appleColour);

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