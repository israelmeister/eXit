#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <optional>
#include <cmath>
#include <map>
#include <vector>

struct Choice {
    std::string command;
    std::string target;
};

struct Room {
    std::string description;
    std::vector<Choice> forks;
};

class Story {
private:
    std::map<std::string, Room> rooms;
    std::string currentRoomId;
    bool reachedGoodEnding = false;
public:
    Story() {
        rooms["dungeon"] = {
            "You're trapped in a dungeon with your friend.\nYou see a barrel. What do you do?",
            {
                {"move the barrel", "barrel moved"},
                {"move barrel", "barrel moved"},
                {"sit down next to my friend", "friend"},
                
            }
        };

        rooms["barrel moved"] = {
            "The barrel rolls aside and you find a secret tunnel.\nWhat do you do?",
            {
                {"enter tunnel", "inside tunnel"},
                {"enter", "inside tunnel"},
                {"enter the tunnel", "inside tunnel"}
            }
        };

        rooms["inside tunnel"] = {
            "You start to escape but your frined is too weak to go with you.\nThey hand you a note. What do you do?",
            {
                {"read note", "dark"},
                {"read", "dark"},
                {"read the note", "dark"}
            }
        };

        rooms["dark"] = {
            "It is too dark to read the note. What do you do?",
            {
                {"leave", "beach"},
                {"leave tunnel", "beach"},
                {"leave the tunnel", "beach"}
            }
        };

        rooms["beach"] = {
            "You crawl through the tunnel and the tunnel leads\nyou to to a beach. What do you do?",
            {
                {"look", "boat"},
                {"look around", "boat"}
            }
        };

        rooms["boat"] = {
            "In the water you see a boat. What do you do?",
            {
                {"get on the boat", "end"},
                {"get on boat", "end"},
                {"get into a boat", "end"},
                {"get into boat", "end"}
            }
        };

        
        ///second route
        rooms["friend"] = {
            "Your friend hands you a note.\nWhat do you do?",
            {
                {"light a match", "note"},
                {"light", "note"},
                {"light match", "note"}
            }
        };
        rooms["note"] = {
            "The note says, \"Don't leave me here\"\nDo you leave your friend or stay?",
            {
                {"stay", "hello friend"},
                {"stay with him", "hello friend"},
                {"leave", "end" }
            }

        };
        rooms["end"] = {
            "Congratulations, you're heading to a new world!\nDo you want to play again?",
            {
                {"yes", "dungeon"},
                {"restart", "dungeon"},
                {"no", "EXIT_GAME"},
                {"exit", "EXIT_GAME"},
                {"escape", "EXIT_GAME"}
            }
        };
        


        currentRoomId = "dungeon";
    }

    std::string getCurrentText() { return rooms[currentRoomId].description; }
    std::string handleInput(std::string input) {
        input.erase(input.find_last_not_of(" \n\r\t") + 1);
        for (auto& x : input) {
            x = std::tolower(x);
        }
        Room& currentRoom = rooms[currentRoomId];
        for (const auto& choice : currentRoom.forks) {
            if (input == choice.command) {
                if (choice.target == "hello friend") {
                    reachedGoodEnding = true;
                    currentRoomId = "EXIT_GAME";
                    return "";
                }
                else {
                    currentRoomId = choice.target;
                    return rooms[currentRoomId].description;
                }
            }
        }
        
        return currentRoom.description + "\n(Nothing happens...)";
    }

    bool isGameOver() {
        return currentRoomId == "EXIT_GAME";
    }
    bool isGoodEnding() {
        return reachedGoodEnding;
    }
    std::string getCurrentRoomId() const {
        return currentRoomId;
    }
};

class Typewriter {
private:
    sf::SoundBuffer beepBuffer;
    sf::Sound beepSound;
    float randomPitch = 0.95f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.05f - 0.95f)));
public:
    sf::Text displayText;
    std::string fullString;
    std::string currentString;
    sf::Clock clock;

    size_t index = 0;
    float delayMs = 60.0f;

    Typewriter(const sf::Font& font, unsigned int size, sf::Color color, sf::Vector2f pos)
        : displayText(font), beepSound(beepBuffer) {
        displayText.setCharacterSize(size);
        displayText.setFillColor(color);
        displayText.setPosition(pos);
       
        

        if (beepBuffer.loadFromFile("assets/click.ogg")) {
            beepSound.setBuffer(beepBuffer);
        }
        else {
            beepSound.setBuffer(beepBuffer);
            beepSound.setVolume( 50.f );
        }
            
    }

    void playClick() {
        beepSound.setPitch(randomPitch);
        beepSound.play();
   }

    void playerClick() {
        beepSound.setPitch(randomPitch);
        beepSound.play();
    }

    void start(std::string text) {
        fullString = text;
        currentString = "";
        index = 0;
        clock.restart();
    }
    
    void update() {
        if (index < fullString.size()) {
            if (clock.getElapsedTime().asMilliseconds() > delayMs) {
                
                for (int i = 0; i < 2 && index < fullString.size(); ++i) {
                    currentString += fullString[index++];
                    beepSound.play();
                }
                displayText.setString(currentString);
                clock.restart();
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(displayText);
    }
    
};

int main() {
    std::map<std::string, sf::Texture> roomTextures;
    if (!roomTextures["dungeon"].loadFromFile("assets/barrel.png")) return -1;
    if (!roomTextures["barrel moved"].loadFromFile("assets/roll.png")) return -1;
    sf::Sprite sceneSprite(roomTextures["dungeon"]);
    sf::Vector2u tSize = roomTextures["dungeon"].getSize();
    if (!roomTextures["inside tunnel"].loadFromFile("assets/inside_tunnel.png")) return -1;
    if (!roomTextures["dark"].loadFromFile("assets/dark.png")) return -1;
    if (!roomTextures["beach"].loadFromFile("assets/beach.png")) return -1;
    if (!roomTextures["boat"].loadFromFile("assets/boat.png")) return -1;
    if(!roomTextures["end"].loadFromFile("assets/end.png")) return -1;
    if (!roomTextures["friend"].loadFromFile("assets/dark.png")) return -1;
    if (!roomTextures["note"].loadFromFile("assets/note.png")) return -1;
    enum class GameState { INTRO, PLAYING, LOADING, EXITING_DARKARMY, EXITING_FSOCIETY };
    GameState current_state = GameState::INTRO;
    std::srand(std::time(nullptr));
    float textureRevealY = 0.f;
    float revealSpeed = 500.f;
    float progress = 1.0f;
    std::vector<std::uint8_t> pixels(800 * 600 * 4, 0);
    for (unsigned int y = 0; y < 600; y += 2) {
        for (unsigned int x = 0; x < 800; ++x) {
            unsigned int index = (y * 800 + x) * 4;
            pixels[index] = 0;
            pixels[index + 1] = 20;
            pixels[index + 2] = 10;
            pixels[index + 3] = 90;
        }
    }
    
    sf::SoundBuffer  loadBuffer;
    sf::Sound loadSound(loadBuffer);
    if (!loadBuffer.loadFromFile("assets/transition.wav")) {
        return -1;
    }
    loadSound.setBuffer(loadBuffer);
    sf::Image crtImage({ 800,600 }, pixels.data());
    sf::Texture crtTexture;
    if (!crtTexture.loadFromImage(crtImage)) {
        return -1;
    }
    sf::Sprite crtOverlay(crtTexture);
    crtOverlay.setPosition({ 0.f,0.f });


    sf::SoundBuffer warmUpBuffer;
    sf::Sound warmUpSound(warmUpBuffer);
    warmUpSound.setVolume(0);
    warmUpSound.play();

    sf::SoundBuffer darkArmyEndingBuffer;
    sf::Sound darkArmyEndingSound(darkArmyEndingBuffer);
    if (!darkArmyEndingBuffer.loadFromFile("assets/dark_army_ending.mp3"));
    darkArmyEndingSound.setBuffer(darkArmyEndingBuffer);

    sf::SoundBuffer endingBuffer;
    sf::Sound endingSound(endingBuffer);
    if (!endingBuffer.loadFromFile("assets/ending.mp3"));
    endingSound.setBuffer(endingBuffer);

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "eXit");
    window.setFramerateLimit(60);

    bool introSoundPlayed = false;
    sf::SoundBuffer introBuffer;
    if (!introBuffer.loadFromFile("assets/start.wav")) {
        return -1;
    }
    sf::Sound introSound(introBuffer);
    introSound.setVolume(70.f);

    sf::Texture testTexture;
    if (!testTexture.loadFromFile("assets/barrel.png")) {
        return -1;
    }
    sf::Sprite test(testTexture);
    sf::Vector2u texSize = testTexture.getSize();
    sf::Texture exitTexture;
    if (!exitTexture.loadFromFile("assets/exit.png")) {
        return -1;
    }
    sf::Texture doorTexture;
    if (!doorTexture.loadFromFile("assets/door.png")) {
        return -1;

    }
    sf::Texture inputTexture;
    if (!inputTexture.loadFromFile("assets/input.png")) {
        return -1;
    }
    sf::Texture fsocietyTexture;
    if (!fsocietyTexture.loadFromFile("assets/fsociety.png")) {
        return -1;
    }
    sf::Texture darkArmyTexture;
    if (!darkArmyTexture.loadFromFile("assets/dark_army.png")) {
        return -1;
    }
    sf::Clock exitClock;
    bool isExiting = false;

    sf::Sprite doorSprite(doorTexture);
    doorSprite.setPosition({ 0.f,0.f });

    sf::Sprite exitSprite(exitTexture);
    exitSprite.setPosition({ 315.f, 460.f });

    sf::Sprite inputSprite(inputTexture);

    sf::Sprite fsocietySprite(fsocietyTexture);
    fsocietySprite.setPosition({ 0.f, 0.f });
    sf::Sprite darkArmySprite(darkArmyTexture);
    darkArmySprite.setPosition({ 0.f, 0.f });
    
    sf::Font font;
    if (!font.openFromFile("assets/vga.ttf")) {
        return -1;
    }
    //

    Typewriter gameText(font, 20, sf::Color::Cyan, { 50.f, 445.f });
    sf::Text inputDisplay(font);

    inputDisplay.setCharacterSize(20);
    inputDisplay.setFillColor(sf::Color::White);
    inputDisplay.setPosition({ 30.f, 520.f });

    sf::Clock typingClock;
    sf::Clock loadingClock;
    std::string pendingText = "";

    Story story;
    std::string playerInput = "";

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (current_state == GameState::INTRO && !introSoundPlayed) {
                introSound.play();
                introSoundPlayed = true;
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scan::Enter) {
                    loadSound.play();
                    if (current_state == GameState::INTRO) {
                        current_state = GameState::PLAYING;

                        gameText.start("You're trapped in a dungeon with your friend.\nYou see a barrel. What do you do?");
                        continue;
                    }
                }
            }
            if (current_state == GameState::PLAYING) {

                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    typingClock.restart();
                    if (textEvent->unicode == 8) {
                        if (!playerInput.empty()) {

                            playerInput.pop_back();
                            gameText.playerClick();
                        }
                    }
                    else if (textEvent->unicode == 13 || textEvent->unicode == 10) {

                        if (!playerInput.empty()) {
                           std::string resultText = story.handleInput(playerInput);
                           loadSound.play();
                           textureRevealY = 0.f;

                            
                            loadingClock.restart();
                            if (story.isGameOver()) {
                                isExiting = true;
                                exitClock.restart();
                                loadSound.stop();
                                if (story.isGoodEnding()) {
                                    endingSound.setVolume(50);
                                    endingSound.play();
                                    current_state = GameState::EXITING_FSOCIETY;
                                }
                                else {
                                    darkArmyEndingSound.play();
                                    current_state = GameState::EXITING_DARKARMY;
                                }

                            }
                            else {
                                std::string currentId = story.getCurrentRoomId();
                                if (roomTextures.count(currentId)) {
                                    sceneSprite.setTexture(roomTextures[currentId]);
                                    texSize = roomTextures[currentId].getSize();
                                }
                                gameText.start(resultText);
                            }

                        }


                        playerInput.clear();
                    }
                    else if (textEvent->unicode < 128) {
                        playerInput += static_cast<char>(textEvent->unicode);
                        gameText.playerClick();
                    }
                }
            }
        }
        
        if (current_state == GameState::PLAYING) {
            gameText.update();
            float brightness = 200 + 55 * sin(gameText.clock.getElapsedTime().asSeconds() * 10.f);
            inputDisplay.setString("> " + playerInput);

        }

        window.clear(sf::Color::Black);
        if (current_state == GameState::INTRO) {
            float brightness = 200 + 55 * sin(gameText.clock.getElapsedTime().asSeconds() * 2.f);
            doorSprite.setColor(sf::Color(brightness, brightness, brightness));
            exitSprite.setColor(sf::Color(brightness, brightness, brightness));

            window.draw(doorSprite);
            window.draw(exitSprite);
        }
        else if (current_state == GameState::EXITING_DARKARMY) {
            
            window.draw(darkArmySprite);
            if (exitClock.getElapsedTime().asSeconds() > 1.f) {
                window.close();
            }
        }
        else if (current_state == GameState::EXITING_FSOCIETY) {
            window.draw(fsocietySprite);
            float exitClock_ = exitClock.getElapsedTime().asSeconds();
            if (exitClock_ > 1.5f) {
                window.close();
            }
        }
        else if (current_state == GameState::PLAYING) {
            float brightness = 200 + 55 * sin(gameText.clock.getElapsedTime().asSeconds() * 3.f);
            sf::Vector2f cursorSlot = inputDisplay.findCharacterPos(playerInput.size() + 2);
            inputSprite.setPosition({ cursorSlot.x + 2.f,527.f });
            inputSprite.setColor(sf::Color::Cyan);
            bool isCurrentTyping = typingClock.getElapsedTime().asSeconds() < 0.2f;
            bool isBlink = static_cast<int>(gameText.clock.getElapsedTime().asSeconds() * 4) % 2 == 0;
            if (isCurrentTyping || isBlink) {
                window.draw(inputSprite);
            }
            
            if (loadSound.getStatus() == sf::SoundSource::Status::Playing) {
                progress = loadSound.getPlayingOffset().asSeconds() / loadBuffer.getDuration().asSeconds();
            }

            sf::IntRect portion;
            if (progress < 0.2f) {
                
                portion = sf::IntRect({ 0, 0 }, { (int)texSize.x, (int)(texSize.y * 0.1f) });
            }
            else if (progress < 0.6f) {
                test.setPosition({ -10.f,0.f });
                portion = sf::IntRect({ 0, 0 }, { (int)texSize.x, (int)(texSize.y * 0.5f) });
                test.setPosition({ 0.f,0.f });
            }
            else {
                
                portion = sf::IntRect({ 0, 0 }, { (int)texSize.x, (int)texSize.y });
            }
            sceneSprite.setTextureRect(portion);
            window.draw(sceneSprite);
            gameText.draw(window);
            window.draw(inputDisplay);
        }
        uint8_t flicker = static_cast<uint8_t>(200 + rand() % 55);
        crtOverlay.setColor(sf::Color(255, 255, 255, flicker));

        window.draw(crtOverlay);
        window.display();
    }
    return 0;
}