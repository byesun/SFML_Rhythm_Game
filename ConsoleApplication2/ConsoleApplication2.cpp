﻿#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
using namespace std;

// 게임 상태 열거형
enum class GameState {
    MainMenu,
    InGame
};

enum class NoteColor {
    Red, Blue
};

class Note {
public:
    sf::Sprite sprite;
    float x, y;
    bool isActive;
    NoteColor color;

    Note(sf::Texture& texture, float startX, float startY, NoteColor noteColor)
        : x(startX), y(startY), isActive(true), color(noteColor) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    // 노트를 움직입니다.
    void update(float delta) {
        if (isActive) {
            x -= 200.0f * delta; // 노트 속도 조절
            sprite.setPosition(x, y);
        }
    }

    // 노트가 활성 상태인지 확인합니다.
    bool isActiveNote() const {
        return isActive;
    }

    // 노트가 클릭된 지점에 있는지 확인합니다.
    bool isAtPosition(float positionX, NoteColor keyPressedColor) {
        if (isActive && color == keyPressedColor && x < positionX + 50.0f && x > positionX - 50.0f) {
            isActive = false; // 노트 비활성화
            return true;
        }
        return false;
    }

    //효과음 재생
    bool isAtPosition(float positionX, NoteColor keyPressedColor, sf::Sound& hitSound) {
        if (isActive && color == keyPressedColor && x < positionX + 50.0f && x > positionX - 50.0f) {
            isActive = false; // 노트 비활성화
            hitSound.play();  // 효과음 재생
            return true;
        }
        return false;
    }

};

int main() {
    // 윈도우 설정
    sf::RenderWindow window(sf::VideoMode(720, 405), "Rhythm Game");
    sf::Clock clock;

    int score = 0;

    // 게임 상태 초기화
    GameState gameState = GameState::MainMenu;

    // 메인 메뉴 텍스처 및 스프라이트 로드
    sf::Texture mainMenuTexture;
    if (!mainMenuTexture.loadFromFile("images/main_menu.jpg")) {
        cerr << "Main menu loading failed!" << endl;
        return -1;
    }
    sf::Sprite mainMenuSprite;
    mainMenuSprite.setTexture(mainMenuTexture);

    // 게임 시작 버튼 텍스처 및 스프라이트 로드
    sf::Texture gameStartButtonTexture;
    if (!gameStartButtonTexture.loadFromFile("images/game_start_button.jpg")) {
        cerr << "Game start button loading failed!" << endl;
        return -1;
    }
    sf::Sprite gameStartButtonSprite;
    gameStartButtonSprite.setTexture(gameStartButtonTexture);
    gameStartButtonSprite.setPosition(300, 300); // 버튼 위치 조정

    // 배경 텍스처 로드
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/배경.jpg")) {
        std::cerr << "Background loading failed!" << std::endl;
        return -1;
    }

    // 배경 스프라이트 설정
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // 북 이미지의 텍스처 로드
    sf::Texture drumTexture;
    if (!drumTexture.loadFromFile("images/red.png")) {
        std::cerr << "Drum image loading failed!" << std::endl;
        return -1;
    }

    // 북 스프라이트 설정
    sf::Sprite drumSprite;
    drumSprite.setTexture(drumTexture);
    drumSprite.setPosition(150, 165); // 북 이미지를 배치할 위치 설정

    //노트 효과음 
    sf::SoundBuffer hitSoundBuffer;
    if (!hitSoundBuffer.loadFromFile("Audios/drumBig3.mp3")) {
        std::cerr << "Hit sound loading failed!" << std::endl;
        return -1;
    }

    //노트 효과음 설정
    sf::Sound hitSound;
    hitSound.setBuffer(hitSoundBuffer);

    // 음악 로드 및 재생
    sf::Music music;
    if (!music.openFromFile("Audios/GO MY WAY!!.mp3")) {
        std::cerr << "Music loading failed!" << std::endl;
        return -1;
    }
    music.play();

    // 노트 텍스처 로드
    sf::Texture redNoteTexture, blueNoteTexture;
    if (!redNoteTexture.loadFromFile("images/red.png") || !blueNoteTexture.loadFromFile("images/blue.png")) {
        std::cerr << "Texture loading failed!" << std::endl;
        return -1;
    }

    // 노트 생성
    std::vector<Note> notes;


    // 게임 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // 메인 메뉴에서 게임 시작 버튼 클릭 처리
            if (event.type == sf::Event::MouseButtonPressed) {
                if (gameState == GameState::MainMenu &&
                    gameStartButtonSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    gameState = GameState::InGame;
                    music.play(); // 음악 재생

                    //게임 시작 시 노트 생성

                    notes.clear();// 이전 노트들 제거

                    // (색상, 나오는 시간, 높이 )
                    notes.emplace_back(redNoteTexture, 800.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 900.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 1000.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(redNoteTexture, 1100.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 1200.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 1300.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 1400.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 1500.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 1600.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 1700.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(redNoteTexture, 1800.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 1900.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 2000.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(redNoteTexture, 2100.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 2200.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 2300.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 2400.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 2500.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 2600.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 2700.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(redNoteTexture, 2800.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 2900.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 3000.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(redNoteTexture, 3100.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 3200.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 3300.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 3400.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 3500.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(blueNoteTexture, 3600.0f, 165.0f, NoteColor::Blue); 
                    notes.emplace_back(redNoteTexture, 3700.0f, 165.0f, NoteColor::Red);  
                    notes.emplace_back(redNoteTexture, 3800.0f, 165.0f, NoteColor::Red); 
                    notes.emplace_back(blueNoteTexture, 3900.0f, 165.0f, NoteColor::Blue);
                    notes.emplace_back(redNoteTexture, 4000.0f, 165.0f, NoteColor::Red);
                    notes.emplace_back(redNoteTexture, 4100.0f, 165.0f, NoteColor::Red);
                    notes.emplace_back(blueNoteTexture, 4200.0f, 165.0f, NoteColor::Blue);
                    notes.emplace_back(blueNoteTexture, 4300.0f, 165.0f, NoteColor::Blue);
                    notes.emplace_back(redNoteTexture, 4400.0f, 165.0f, NoteColor::Red);
                    notes.emplace_back(blueNoteTexture, 4500.0f, 165.0f, NoteColor::Blue);
                    notes.emplace_back(blueNoteTexture, 4600.0f, 165.0f, NoteColor::Blue);
                    notes.emplace_back(redNoteTexture, 4700.0f, 165.0f, NoteColor::Red);
                }
            }
        }

        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        for (auto& note : notes) {
            note.update(deltaTime);
        }

        // 게임 상태에 따른 업데이트 및 렌더링
        window.clear();
        if (gameState == GameState::MainMenu) {
            window.draw(mainMenuSprite);
            window.draw(gameStartButtonSprite);
        }
        else if (gameState == GameState::InGame) {
            sf::Time elapsed = clock.restart();
            float deltaTime = elapsed.asSeconds();

            //노트 업데이트
            for (auto& note : notes) {
                note.update(deltaTime);
            }

            //키 입력 처리 및 노트 렌더링
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                for (auto& note : notes) {
                    if (note.isActiveNote() && note.color == NoteColor::Red &&
                        note.isAtPosition(drumSprite.getPosition().x, NoteColor::Red, hitSound)) {
                        score += 10;
                        cout << "Score: " << score << endl;
                        break;
                    }
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) || sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
                for (auto& note : notes) {
                    if (note.isActiveNote() && note.color == NoteColor::Blue &&
                        note.isAtPosition(drumSprite.getPosition().x, NoteColor::Blue, hitSound)) {
                        score += 10;
                        cout << "Score: " << score << endl;
                        break;
                    }
                }
            }

            // 배경 렌더링
            window.draw(backgroundSprite);

            // 북 이미지 렌더링
            window.draw(drumSprite);

            // 노트 렌더링
            for (auto& note : notes) {
                if (note.isActiveNote()) {
                    window.draw(note.sprite);
                }
            }
        }
        window.display();
    }

    return 0;
}