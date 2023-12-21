#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
using namespace std;
using namespace sf;

// 게임 상태 열거형
enum class GameState {
    MainMenu,
    InGame,
    GameOver
};

enum class NoteColor {
    Red, Blue, Yello
};

class Note {
public:
    Sprite sprite;
    float x, y;
    bool isActive;
    NoteColor color;

    Note(Texture& texture, float startX, float startY, NoteColor noteColor)
        : x(startX), y(startY), isActive(true), color(noteColor) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    // 노트를 움직임 설정
    void update(float delta) {
        if (isActive) {
            x -= 300.0f * delta; // 노트 속도 조절
            sprite.setPosition(x, y);
        }
    }

    // 노트가 활성 상태 확인
    bool isActiveNote() const {
        return isActive;
    }

    // 노트가 클릭된 지점 위치
    bool isAtPosition(float positionX, NoteColor keyPressedColor) {
        if (isActive && color == keyPressedColor && x < positionX + 50.0f && x > positionX - 50.0f) {
            isActive = false; // 노트 비활성화
            return true;
        }
        return false;
    }

    //효과음 재생
    bool isAtPosition(float positionX, NoteColor keyPressedColor, Sound& hitSound) {
        if (isActive && color == keyPressedColor && x < positionX + 50.0f && x > positionX - 50.0f) {
            isActive = false;
            hitSound.play();  // 효과음 재생
            return true;
        }
        return false;
    }

    // 노트가 x축 위치 100을 확인 ( 판정 )
    bool passedJudgementLine() {
        return x < 100.0f && isActive;
    }
};

class HitEffect {
public:
    Sprite sprite;
    Texture texture;
    Texture comboBreakTexture;
    bool active;
    float duration;

    HitEffect() : active(false), duration(0.01f) {
        if (!texture.loadFromFile("images/great.png")) {
            cerr << "Unable to load hit effect texture!" << endl;
            exit(EXIT_FAILURE);
        }
        if (!comboBreakTexture.loadFromFile("images/combobreak.png")) {
            cerr << "Unable to load combo break texture!" << endl;
            exit(EXIT_FAILURE);
        }
        sprite.setTexture(texture);
    }

    void activate(Vector2f position, bool comboBreak = false) {
        sprite.setPosition(position);
        if (comboBreak) {
            sprite.setTexture(comboBreakTexture);
            duration = 0.1f;
        }
        else {
            sprite.setTexture(texture);
        }
        active = true;
    }

    void update(float deltaTime) {
        if (active) {
            duration -= deltaTime;
            if (duration <= 0) {
                active = false;
                duration = 0.1f;
            }
        }
    }

    void draw(RenderWindow& window) {
        if (active) {
            window.draw(sprite);
        }
    }
};

class ScoreGrade {
public:
    Sprite sprite;
    Texture sTexture, aTexture, bTexture, fTexture;

    ScoreGrade() {
        if (!sTexture.loadFromFile("images/S.png") ||
            !aTexture.loadFromFile("images/A.png") ||
            !bTexture.loadFromFile("images/B.png") ||
            !fTexture.loadFromFile("images/F.png")) {
            cerr << "Unable to load grade textures!" << endl;
            exit(EXIT_FAILURE);
        }
    }

    void update(int score) {
        if (score < 100) {
            sprite.setTexture(fTexture);
        }
        else if (score < 200) {
            sprite.setTexture(bTexture);
        }
        else if (score < 300) {
            sprite.setTexture(aTexture);
        }
        else {
            sprite.setTexture(sTexture);
        }
        sprite.setPosition(600, 280); // 랭크 위치 설정
    }

    void draw(RenderWindow& window) {
        window.draw(sprite);
    }
};

class GameOverScreen {
public:
    Sprite backgroundSprite;
    Texture backgroundTexture;
    Sprite exitButtonSprite;
    Texture exitButtonTexture;
    ScoreGrade finalScoreGrade;
    Text scoreText;
    Font font;

    GameOverScreen() {
        if (!backgroundTexture.loadFromFile("images/end_menu.jpg")) {
            cerr << "Unable to load game over background texture!" << endl;
            exit(EXIT_FAILURE);
        }
        backgroundSprite.setTexture(backgroundTexture);

        if (!exitButtonTexture.loadFromFile("images/end.png")) {
            cerr << "Unable to load exit button texture!" << endl;
            exit(EXIT_FAILURE);
        }
        exitButtonSprite.setTexture(exitButtonTexture);
        exitButtonSprite.setPosition(330, 300); // 최종 화면 end 버튼 위치 설정

        // 폰트 로드
        if (!font.loadFromFile("fonts/NanumSquareNeo-Variable.ttf")) {
            cerr << "Unable to load font!" << endl;
            exit(EXIT_FAILURE);
        }

        // 점수 텍스트 설정
        scoreText.setFont(font);
        scoreText.setCharacterSize(35);
        scoreText.setFillColor(Color::Black);
        scoreText.setPosition(300, 100); // 점수 텍스트의 위치 설정
    }

    void draw(RenderWindow& window, int score) {
        window.draw(backgroundSprite);
        finalScoreGrade.update(score);
        finalScoreGrade.draw(window);
        window.draw(exitButtonSprite);
        setScore(score); // 점수 업데이트
        window.draw(scoreText); // 점수 텍스트 렌더링
    }

    void setScore(int score) {
        scoreText.setString("score : " + to_string(score));
    }

};

int main() {
    // SFML 라이브러리 윈도우 설정
    RenderWindow window(VideoMode(720, 405), "taegoui darin mojak");
    Clock clock;

    int score = 0;

    HitEffect hitEffect; // 임펙트 객체

    ScoreGrade scoreGrade; // 점수 등급 객체

    GameOverScreen gameOverScreen; // 최종 화면

    Time gameTimeLimit = seconds(20.0f);
    Time elapsedTime = Time::Zero;


    // 게임 상태 초기화
    GameState gameState = GameState::MainMenu;

    // 메인 메뉴 텍스처 및 스프라이트 로드
    Texture mainMenuTexture;
    if (!mainMenuTexture.loadFromFile("images/main_menu.jpg")) {
        cerr << "Main menu loading failed!" << endl;
        return -1;
    }
    Sprite mainMenuSprite;
    mainMenuSprite.setTexture(mainMenuTexture);

    // 게임 시작 버튼 텍스처 및 스프라이트 로드
    Texture gameStartButtonTexture;
    if (!gameStartButtonTexture.loadFromFile("images/start.png")) {
        cerr << "Game start button loading failed!" << endl;
        return -1;
    }
    Sprite gameStartButtonSprite;
    gameStartButtonSprite.setTexture(gameStartButtonTexture);
    gameStartButtonSprite.setPosition(330, 300); // game start 버튼 위치 설정

    // 메인 화면 종료 버튼 설정
    Sprite endButtonSprite;
    Texture endButtonTexture;
    if (!endButtonTexture.loadFromFile("images/end.png")) {
        cerr << "End button loading failed!" << endl;
        return -1;
    }
    endButtonSprite.setTexture(endButtonTexture);
    endButtonSprite.setPosition(330, 360); // 메인 화면 end버튼 위치 설정

    // 배경 텍스처 로드
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/game_background.jpg")) {
        cerr << "Background loading failed!" << endl;
        return -1;
    }

    // 배경 스프라이트 
    Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // 판정선 이미지 로드
    Texture drumTexture;
    if (!drumTexture.loadFromFile("images/judgment_line.png")) {
        cerr << "Drum image loading failed!" << endl;
        return -1;
    }

    Sprite drumSprite;
    drumSprite.setTexture(drumTexture);
    drumSprite.setPosition(195, 165); // 북 이미지를 배치할 위치 설정

    //노트 효과음 설정
    SoundBuffer hitSoundBuffer;
    if (!hitSoundBuffer.loadFromFile("Audios/drumBig3.mp3")) {
        cerr << "Hit sound loading failed!" << endl;
        return -1;
    }

    Sound hitSound;
    hitSound.setBuffer(hitSoundBuffer);

    // 음악 로드 및 재생
    Music music;
    if (!music.openFromFile("Audios/GO MY WAY!!.mp3")) {
        cerr << "Music loading failed!" << endl;
        return -1;
    }
    music.play(); //인게임 시 재생

    // 노트 텍스처 로드
    Texture redNoteTexture, blueNoteTexture;
    if (!redNoteTexture.loadFromFile("images/red.png") || !blueNoteTexture.loadFromFile("images/blue.png")) {
        cerr << "Texture loading failed!" << endl;
        return -1;
    }

    // 노트 생성
    vector<Note> notes;


    // 게임 루프
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            // 메인 메뉴에서 게임 시작 버튼 클릭 처리
            if (event.type == Event::MouseButtonPressed) {
                if (gameState == GameState::MainMenu &&
                    gameStartButtonSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    gameState = GameState::InGame;
                    music.play();
                    elapsedTime = Time::Zero; // 시간 초기화

                    notes.clear();// 이전 노트들 초기화

                    //게임 시작 시 노트 생성 (색상, 나오는 시간, 높이 )
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
                else if (endButtonSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    window.close(); // 메인 화면 종료 버튼 클릭 시 프로그램 종료
                }
                else if (gameState == GameState::GameOver) {
                    if (event.type == Event::MouseButtonPressed) {
                        // 최종 화면에서 종료 버튼 클릭 처리
                        if (gameOverScreen.exitButtonSprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            window.close(); // 최종 화면 종료 버튼 클릭 시 프로그램 종료
                        }
                    }
                }
            }
        }

        Time elapsed = clock.restart();

        // 게임 상태에 따른 업데이트 및 렌더링
        window.clear();
        if (gameState == GameState::MainMenu) {
            window.draw(mainMenuSprite);
            window.draw(gameStartButtonSprite);
            window.draw(endButtonSprite); // 메인 화면 종료 버튼
        }
        else if (gameState == GameState::InGame) {
            Time elapsed = clock.restart();
            float deltaTime = elapsed.asSeconds();

            hitEffect.update(deltaTime); // 임펙트 업데이트
            elapsedTime += elapsed;

            // 노트 업데이트 및 판정선 넘기 확인
            for (auto& note : notes) {
                note.update(deltaTime);
                if (note.passedJudgementLine()) {
                    if (note.isActiveNote()) {
                        score -= 5;
                        cout << "Score: " << score << endl;
                        note.isActive = false;
                        hitEffect.activate(Vector2f(160, 80), true);
                    }
                }
            }

            // 키 입력 처리
            NoteColor keyPressedColor = NoteColor::Red;
            if (Keyboard::isKeyPressed(Keyboard::F) || Keyboard::isKeyPressed(Keyboard::D)) {
                keyPressedColor = NoteColor::Red; // 빨간색 노트
                for (auto& note : notes) {
                    if (note.isActiveNote() && note.color == keyPressedColor &&
                        note.isAtPosition(drumSprite.getPosition().x, keyPressedColor, hitSound)) {
                        score += 10;
                        cout << "Score: " << score << endl;
                        hitEffect.activate(note.sprite.getPosition() - Vector2f(note.sprite.getGlobalBounds().width / 1.2, note.sprite.getGlobalBounds().height / 1.2)); // 임팩트 활성화 위치를 노트 중앙으로 조정
                        break;
                    }
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::J) || Keyboard::isKeyPressed(Keyboard::K)) {
                keyPressedColor = NoteColor::Blue; // 파란색 노트
                for (auto& note : notes) {
                    if (note.isActiveNote() && note.color == keyPressedColor &&
                        note.isAtPosition(drumSprite.getPosition().x, keyPressedColor, hitSound)) {
                        score += 10;
                        cout << "Score: " << score << endl;
                        hitEffect.activate(note.sprite.getPosition() - Vector2f(note.sprite.getGlobalBounds().width / 1.2, note.sprite.getGlobalBounds().height / 1.2)); // 임팩트 활성화 위치를 노트 중앙으로 조정
                        break;
                    }
                }
            }

            if (elapsedTime >= gameTimeLimit) {
                gameState = GameState::GameOver;
                music.stop(); // 음악 종료
            }

            // 배경 렌더링
            window.draw(backgroundSprite);

            // 판정 이미지 렌더링
            window.draw(drumSprite);

            // 노트 렌더링
            for (auto& note : notes) {
                if (note.isActiveNote()) {
                    window.draw(note.sprite);
                }
            }

            scoreGrade.update(score); // 점수 등급 업데이트
            scoreGrade.draw(window);  // 점수 등급 렌더링

            hitEffect.draw(window); // 임펙트 렌더링
        }
        else if (gameState == GameState::GameOver) {
            gameOverScreen.draw(window, score);
        }
        window.display();
    }

    return 0;
}