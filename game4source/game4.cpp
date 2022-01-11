#define OLC_PGE_APPLICATION
#include "../shared/olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Breakout"; }

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
          olc::Pixel givenColour) {
      position.x = givenX;
      position.y = givenY;
      size.x = givenWidth;
      size.y = givenHeight;
      colour = givenColour;
    }
  };

  std::vector<Block> blocks;
  int player = 0;

  olc::vf2d ballPosition{WINDOW_WIDTH / 2, WINDOW_HEIGHT - 100};
  olc::vf2d ballVelocity{250, -250};
  int ballRadius = 10;
  olc::Pixel ballColour{0, 255, 255};

 public:
  bool OnUserCreate() override {  // Load resources here

    blocks.push_back(
        Block(WINDOW_WIDTH / 2 - 40, WINDOW_HEIGHT - 60, 80, 20, olc::WHITE));

    for (auto y = 120; y < WINDOW_HEIGHT / 2; y += 50) {
      for (auto x = 120; x <= WINDOW_WIDTH - 200; x += 80) {
        blocks.push_back(Block(x, y, 60, 30, olc::BLUE));
      }
    }

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

    if (GetKey(olc::Key::RIGHT).bHeld) {
      blocks[player].velocity.x += 2400 * GetElapsedTime();
      if (blocks[player].velocity.x > 600) blocks[player].velocity.x = 600;
    }

    if (GetKey(olc::Key::LEFT).bHeld) {
      blocks[player].velocity.x -= 2400 * GetElapsedTime();
      if (blocks[player].velocity.x < -600) blocks[player].velocity.x = -600;
    }
  }

  void processes() {  // Game logic goes here

    blocks[player].position += blocks[player].velocity * GetElapsedTime();

    if (blocks[player].position.x < 0) {
      blocks[player].position.x = 0;
      blocks[player].velocity.x = 0;
    }
    if (blocks[player].position.x > WINDOW_WIDTH - blocks[player].size.x) {
      blocks[player].position.x = WINDOW_WIDTH - blocks[player].size.x;
      blocks[player].velocity.x = 0;
    }

    ballPosition += ballVelocity * GetElapsedTime();

    if (ballVelocity.x > 0 && ballPosition.x > WINDOW_WIDTH - ballRadius)
      ballVelocity.x = -ballVelocity.x;
    if (ballVelocity.x < 0 && ballPosition.x < ballRadius)
      ballVelocity.x = -ballVelocity.x;
    if (ballVelocity.y > 0 && ballPosition.y > WINDOW_HEIGHT - ballRadius)
      ballVelocity.y = -ballVelocity.y;
    if (ballVelocity.y < 0 && ballPosition.y < ballRadius)
      ballVelocity.y = -ballVelocity.y;

    for (auto i = 0; i < blocks.size();) {
      auto collision = false;
      auto block = blocks[i];

      auto midX = block.position.x + block.size.x / 2;
      auto midY = block.position.y + block.size.y / 2;

      auto gammaX = ballPosition.x - block.position.x;
      auto gammaY = ballPosition.y - block.position.y;

      if (gammaX > -ballRadius && gammaX < block.size.x + ballRadius) {
        if (gammaY > -ballRadius && gammaY < block.size.y + ballRadius) {
          if (i != -1) collision = true;

          auto fracX = (ballPosition.x - midX) / block.size.x;
          auto fracY = (ballPosition.y - midY) / block.size.y;
          if (abs(fracX) > abs(fracY)) {
            if (ballPosition.x < midX && ballVelocity.x > 0 ||
                ballPosition.x > midX && ballVelocity.x < 0)
              ballVelocity.x = -ballVelocity.x;
            ballVelocity += block.velocity / 10;
          } else {
            if (ballPosition.y < midY && ballVelocity.y > 0 ||
                ballPosition.y > midY && ballVelocity.y < 0)
              ballVelocity.y = -ballVelocity.y;
            ballVelocity += block.velocity / 10;
          }
        }
      }

      if (i > 0 && collision) {
        blocks.erase(blocks.begin() + i);
      } else {
        i++;
      }
    }
  }

  void outputs() {  // Game graphics drawn here

    Clear(olc::BLACK);

    for (auto block : blocks) {
      FillRect(block.position, block.size, block.colour);
    }

    FillCircle(ballPosition, ballRadius, ballColour);

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