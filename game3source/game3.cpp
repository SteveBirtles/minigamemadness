#define OLC_PGE_APPLICATION
#include "../shared/olcPixelGameEngine.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Game : public olc::PixelGameEngine {
 public:
  Game() { sAppName = "Tron"; }

 private:
  float timer = 0;
  int frames = 0;
  int fps;

  struct Block {
    olc::vf2d position;
    olc::vf2d size;
    olc::Pixel colour;
    Block(int givenX, int givenY, int givenWidth, int givenHeight,
          olc::Pixel givenColour, olc::vf2d givenVelocity = olc::vf2d{0,0}) {
      position.x = givenX;
      position.y = givenY;
      size.x = givenWidth;
      size.y = givenHeight;
      colour = givenColour;
      velocity = givenVelocity;
    }
  };

  const int PLAYER_SIZE = 20;

  std::vector<Block> player[2];
  olc::vi2d playerVelocity[2]{{PLAYER_SIZE, 0}, {-PLAYER_SIZE, 0}};
  float playerDelay[2]{0.1, 0.1};
  float playerTimer[2]{playerDelay[0], playerDelay[1]};
  bool playerAlive[2]{true, true};

 public:
  bool OnUserCreate() override {  // Load resources here

    player[0].push_back(
        Block(100, 360, PLAYER_SIZE, PLAYER_SIZE, olc::Pixel(0, 255, 0)));
    player[1].push_back(
        Block(1100, 360, PLAYER_SIZE, PLAYER_SIZE, olc::Pixel(255, 0, 0)));
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

    if (playerAlive[0]) {
      if (GetKey(olc::Key::W).bHeld && playerVelocity[0].y == 0)
        playerVelocity[0] = olc::vi2d(0, -PLAYER_SIZE);
      if (GetKey(olc::Key::S).bHeld && playerVelocity[0].y == 0)
        playerVelocity[0] = olc::vi2d(0, PLAYER_SIZE);
      if (GetKey(olc::Key::A).bHeld && playerVelocity[0].x == 0)
        playerVelocity[0] = olc::vi2d(-PLAYER_SIZE, 0);
      if (GetKey(olc::Key::D).bHeld && playerVelocity[0].x == 0)
        playerVelocity[0] = olc::vi2d(PLAYER_SIZE, 0);
    }

    if (playerAlive[1]) {
      if (GetKey(olc::Key::UP).bHeld && playerVelocity[1].y == 0)
        playerVelocity[1] = olc::vi2d(0, -PLAYER_SIZE);
      if (GetKey(olc::Key::DOWN).bHeld && playerVelocity[1].y == 0)
        playerVelocity[1] = olc::vi2d(0, PLAYER_SIZE);
      if (GetKey(olc::Key::LEFT).bHeld && playerVelocity[1].x == 0)
        playerVelocity[1] = olc::vi2d(-PLAYER_SIZE, 0);
      if (GetKey(olc::Key::RIGHT).bHeld && playerVelocity[1].x == 0)
        playerVelocity[1] = olc::vi2d(PLAYER_SIZE, 0);
    }
  }

  void processes() {  // Game logic goes here

    for (auto i = 0; i < 2; i++) {
      if (playerAlive[i]) {
        playerTimer[i] -= GetElapsedTime();
        if (playerTimer[i] < 0) {
          playerTimer[i] += playerDelay[i];
          auto head = player[i].size() - 1;
          auto x = player[i][head].position.x + playerVelocity[i].x;
          auto y = player[i][head].position.y + playerVelocity[i].y;

          for (auto j = 0; j < 2; j++) {
            for (auto block : player[j]) {
              if (block.position.x == x && block.position.y == y) {
                playerAlive[i] = false;
                break;
              }
            }
          }

          if (playerAlive[i]) {
            player[i].push_back(
                Block(x, y, PLAYER_SIZE, PLAYER_SIZE, player[i][head].colour));
          }
        }
      }
    }
  }

  void outputs() {  // Game graphics drawn here

    Clear(olc::BLACK);

    for (auto i = 0; i < 2; i++) {
      for (auto block : player[i]) {
        FillRect(block.position, block.size, block.colour);
      }
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