#include "constants.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace sf;

int isThereStillFence(int fence) {
    return fence ? 2 : 0;
}

int validPositions(int x, int y) {
    return x && y || x == 0 && y != 0 || y == 0 && x != 0;
}

int isFenceNotBlocked(int boardMatrix[7][9], int boardMatrixFences[7][9], int boardMatrixFencesMock[7][9], int positionX, int positionY, int orientation, int prevX, int prevY, int pPrevX, int pPrevY) {
    //1 - vert orientation
    //0 - oriz orientation

    int firstCondValid = (positionX - 2 >= 0 && positionY - 2 >= 0 && positionY + 2 <= 8 ? 1 : 0) && orientation && validPositions(pPrevX, pPrevY);
    int secondCondValid = (positionX - 2 >= 0 && positionY - 2 >= 0 && positionX + 2 <= 8 ? 1 : 0) && !orientation && validPositions(pPrevX, pPrevY);


    if(orientation && prevX == positionX - 2 && prevY == positionY && validPositions(prevX, prevY) && boardMatrixFencesMock[positionX - 2][positionY] == 4 && (!boardMatrixFencesMock[positionX - 2][positionY - 2] && !boardMatrixFencesMock[positionX - 2][positionY + 2]) && boardMatrixFencesMock[positionX][positionY] == 2 && (!boardMatrixFencesMock[positionX][positionY - 2] && !boardMatrixFencesMock[positionX][positionY + 2]) && positionX - 2 >= 0 && positionY - 2 >= 0 ||
       firstCondValid && positionX - 4 == pPrevX && boardMatrixFencesMock[positionX - 4][positionY] && boardMatrix[positionX - 2][positionY - 2] && boardMatrix[positionX - 2][positionY + 2] ||
       secondCondValid && positionY - 4 == pPrevY && boardMatrixFencesMock[positionX][positionY - 4] && boardMatrix[positionX - 2][positionY - 2] && boardMatrix[positionX + 2][positionY - 2] ||
       !orientation && prevX == positionX && prevY == positionY - 2 && validPositions(prevX, prevY) && boardMatrixFencesMock[positionX][positionY - 2] == 4 && (!boardMatrixFencesMock[positionX - 2][positionY - 2] && !boardMatrixFencesMock[positionX + 2][positionY - 2]) && boardMatrixFencesMock[positionX][positionY] == 2 && (!boardMatrixFencesMock[positionX - 2][positionY] && !boardMatrixFencesMock[positionX + 2][positionY]) &&  positionY - 2 >= 0 && positionX - 2 >= 0) {
        return 1;
    }

    boardMatrixFencesMock[positionX][positionY] += 2;

    return 0;
}

void generateBoardMatrix(int boardMatrix[7][9], int boardMatrixRes[7][9], int level, int boardMatrixDefault[7][9], int boardMatrixFences[7][9], float& timeLimit) {
    ifstream fin1("../nivele/planse/plansa_" + to_string(level) + ".txt");
    ifstream fin2("../nivele/rezolvari/rezolvare_" + to_string(level) + ".txt");
    int k1, k2;
    for(int i = 0; i < 7; i++)
        for(int j = 0; j < 9; j++) {
            fin1 >> k1;
            boardMatrix[i][j] = k1;
            boardMatrixDefault[i][j] = k1;
            boardMatrixFences[i][j] = 0;

            fin2 >> k2;
            boardMatrixRes[i][j] = k2;
        }


    fin1.close();
    fin2.close();

    if (level >= 1 && level <= 12) {
        timeLimit = 30;
    } else if (level >= 13 && level <= 24) {
        timeLimit = 45;
    } else if (level >= 25 && level <= 34) {
        timeLimit = 60;
    } else {
        timeLimit = 90;
    }
}

string animalAddress(int typeAnimal) {
    switch(typeAnimal) {
        case 4:
            return HORSE2;
        case -4:
            return HORSE1;
        case 5:
            return COW2;
        case -5:
            return COW1;
        case 6:
            return PIG;
        case 7:
            return SHEEP;
        case 8:
            return WATERPOT;
        default:
            return "";
    }
}

int isLevelCompleted(int boardMatrix[7][9], int boardMatrixRes[7][9]) {
    for(int i = 0; i < 7; i++)
        for(int j = 0; j < 9; j++)
            if(boardMatrix[i][j] != boardMatrixRes[i][j])
                return 0;

    return 1;
}

int main()
{
    int currentState = 0; //main menu

    RenderWindow window(VideoMode(SCREEN_X, SCREEN_Y), "Smart Farmer");

    Music music;
    if (!music.openFromFile("../music/MainMenuSong.mp3"))
        return EXIT_FAILURE;
    music.setVolume(40);
    music.setLoop(true);

    Music winMusic;
    if (!winMusic.openFromFile("../music/victory (1).wav"))
        return EXIT_FAILURE;
    winMusic.setVolume(50);
    winMusic.setLoop(false);

    Music loseMusic;
    if (!loseMusic.openFromFile("../music/lose.wav"))
        return EXIT_FAILURE;
    loseMusic.setVolume(50);
    loseMusic.setLoop(false);

    Music fenceMusic;
    if (!fenceMusic.openFromFile("../music/buttonSound.wav"))
        return EXIT_FAILURE;
    fenceMusic.setVolume(70);
    fenceMusic.setLoop(false);

    Music fenceUnplaceMusic;
    if (!fenceUnplaceMusic.openFromFile("../music/wood_unplace.wav"))
        return EXIT_FAILURE;
    fenceUnplaceMusic.setVolume(50);
    fenceUnplaceMusic.setLoop(false);

    Music buttonPressMusic;
    if (!buttonPressMusic.openFromFile("../music/press_button.wav"))
        return EXIT_FAILURE;
    buttonPressMusic.setVolume(70);
    buttonPressMusic.setLoop(false);

    Clock clock;
    float timeLimit = 30.f;
    int delayTime = 0;

    //Configurare Font
    Font fontConvergence;
    if (!fontConvergence.loadFromFile("../fonts/Convergence.ttf"))
        return EXIT_FAILURE;

    //Configurare Culori
    Color customButtonColor(178, 70, 76); //#aa464c
    Color customLevelColor(141, 182, 60); //#8DB63C

    //Configurare Cursori
    Cursor cursorHand;
    if (!cursorHand.loadFromSystem(Cursor::Hand))
        return EXIT_FAILURE;

    Cursor cursorDefault;
    if (!cursorDefault.loadFromSystem(Cursor::Arrow))
        return EXIT_FAILURE;

    //Background Main Menu
    Texture textureBackground;
    if (!textureBackground.loadFromFile("../images/main_screen/SF_background.jpg"))
        return EXIT_FAILURE;
    Sprite spriteBackground(textureBackground);

    //Background pentru ecranul de nivele
    Texture textureLevelsBackground;
    if (!textureLevelsBackground.loadFromFile("../images/Board/levelBackground.jpg"))
        return EXIT_FAILURE;
    Sprite spriteLevelsBackground(textureLevelsBackground);

    FloatRect bgBounds = spriteLevelsBackground.getLocalBounds();
    spriteLevelsBackground.setScale(
            SCREEN_X / bgBounds.width,
            SCREEN_Y / bgBounds.height
    );
    spriteLevelsBackground.setPosition(0, 0);

    //Titlu
    Text gameTitle("Smart Farmer", fontConvergence);
    gameTitle.setCharacterSize(70);
    gameTitle.setStyle(Text::Bold);
    gameTitle.setFillColor(Color::White);
    gameTitle.setPosition(MIDDLE_X - 250, 50);

    //Butoane meniu
    RectangleShape buttonStart(Vector2f(270.f, 70.f));
    buttonStart.setPosition(MIDDLE_X - 135, MIDDLE_Y - 100);
    buttonStart.setFillColor(customButtonColor);

    Text textStart("Start", fontConvergence);
    textStart.setCharacterSize(30);
    textStart.setStyle(Text::Bold);
    textStart.setFillColor(Color::White);
    textStart.setPosition(MIDDLE_X - 40, MIDDLE_Y - 85);

    RectangleShape buttonHTP(Vector2f(270.f, 70.f));
    buttonHTP.setPosition(MIDDLE_X - 135, MIDDLE_Y);
    buttonHTP.setFillColor(customButtonColor);

    Text textHTP("How to play", fontConvergence);
    textHTP.setCharacterSize(30);
    textHTP.setStyle(Text::Bold);
    textHTP.setFillColor(Color::White);
    textHTP.setPosition(MIDDLE_X - 90, MIDDLE_Y + 12.5);

    RectangleShape buttonExit(Vector2f(270.f, 70.f));
    buttonExit.setPosition(MIDDLE_X - 135, MIDDLE_Y + 100);
    buttonExit.setFillColor(customButtonColor);

    Text textExit("Exit", fontConvergence);
    textExit.setCharacterSize(30);
    textExit.setStyle(Text::Bold);
    textExit.setFillColor(Color::White);
    textExit.setPosition(MIDDLE_X - 30, MIDDLE_Y + 112.5);

    Texture textureLevelDesign;
    if (!textureLevelDesign.loadFromFile("../images/main_screen/level_icon.png"))
        return EXIT_FAILURE;

    Texture textureBackButton;
    if (!textureBackButton.loadFromFile("../images/main_screen/go_back_arrow.png"))
        return EXIT_FAILURE;

    Texture textureHtpBackground;
    if (!textureHtpBackground.loadFromFile("../images/Board/Help.jpg"))
        return EXIT_FAILURE;


    RectangleShape squares[16], squares2[12];
    int fencesSquaresOrigins[8] = {-1}, fenceOnSquares[16];
    int levelSelected = -1;

    for(int i = 0; i < 16; i++) fenceOnSquares[i] = 0;

    Texture fenceTextures[8];


    if (!fenceTextures[0].loadFromFile("../images/Board/gardColt1.png") ||
        !fenceTextures[1].loadFromFile("../images/Board/gardColt2.png") ||
        !fenceTextures[2].loadFromFile("../images/Board/gardColt3 (1).png") ||
        !fenceTextures[3].loadFromFile("../images/Board/gardColt4 (1).png") ||
        !fenceTextures[4].loadFromFile("../images/Board/gardOrizontalL2 (1).png") ||
        !fenceTextures[5].loadFromFile("../images/Board/gardOrizontalL3 (1).png") ||
        !fenceTextures[6].loadFromFile("../images/Board/gardVerticalL3 (1).png") ||
        !fenceTextures[7].loadFromFile("../images/Board/gardVerticalL2 (1).png")) {
        return EXIT_FAILURE;
    }

    int boardMatrix[7][9] = {0}, boardMatrixRes[7][9] = {0}, boardMatrixDefault[7][9] = {0}, boardMatrixFences[7][9] = {0};
    int levelsCompleted[60];

    for(int i = 0; i < 60; i++)
        levelsCompleted[i] = 0;

    Sprite fenceSprites[8], spriteFencesBoard[8];

    for(int i = 0; i < 8; i++) {
        fenceSprites[i].setTexture(fenceTextures[i]);
        fenceSprites[i].setScale(.55f, .65f);
        spriteFencesBoard[i].setTexture(fenceTextures[i]);
    }

    int isSelectedFences[8] = {0};
    int isUsableFences[8];

    for(int i = 0; i < 8; i++)
        isUsableFences[i] = 1;

    //Garduri
    for(int i = 0; i < 16; i++) {
        squares[i].setFillColor(Color::Transparent);
        if (i >= 4 && i <= 6 || i >= 9 && i <= 11)
            squares[i].setSize(Vector2f(70, 70));
        else {
            squares[i].setSize(Vector2f(30, 30));
        }
        switch (i) {
            case 0: //prima linie
                squares[i].setPosition(Vector2f(310, 155));
                break;
            case 1:
                squares[i].setPosition(Vector2f(435, 155));
                break;
            case 2:
                squares[i].setPosition(Vector2f(555, 150));
                break;
            case 3: //a doua linie
                squares[i].setPosition(Vector2f(210, 270));
                break;
            case 4:
                squares[i].setPosition(Vector2f(290, 240));
                break;
            case 5:
                squares[i].setPosition(Vector2f(415, 245));
                break;
            case 6:
                squares[i].setPosition(Vector2f(540, 245));
                break;
            case 7:
                squares[i].setPosition(Vector2f(665, 250));
                break;
            case 8: //a treia linie
                squares[i].setPosition(Vector2f(210, 390));
                break;
            case 9:
                squares[i].setPosition(Vector2f(290, 365));
                break;
            case 10:
                squares[i].setPosition(Vector2f(420, 365));
                break;
            case 11:
                squares[i].setPosition(Vector2f(545, 365));
                break;
            case 12:
                squares[i].setPosition(Vector2f(670, 390));
                break;
            case 13: //a patra linie
                squares[i].setPosition(Vector2f(310, SCREEN_Y - 210));
                break;
            case 14:
                squares[i].setPosition(Vector2f(430, SCREEN_Y - 210));
                break;
            case 15:
                squares[i].setPosition(Vector2f(560, SCREEN_Y - 210));
                break;
            default:
                squares[i].setPosition(Vector2f(0, 0));
                break;
        }
    }

    //Animale
    for(int j = 0; j < 12; j++)
    {
        squares2[j].setFillColor(Color::Transparent);
        squares2[j].setSize(Vector2f(40, 40));
        switch (j)
        {
            case 0: //prima linie
                squares2[j].setPosition(Vector2f(263, 223));
                break;
            case 1:
                squares2[j].setPosition(Vector2f(390, 220));
                break;
            case 2:
                squares2[j].setPosition(Vector2f(515, 220));
                break;
            case 3:
                squares2[j].setPosition(Vector2f(635, 220));
                break;
            case 4: // a doua linie
                squares2[j].setPosition(Vector2f(260, 350));
                break;
            case 5:
                squares2[j].setPosition(Vector2f(390, 350));
                break;
            case 6:
                squares2[j].setPosition(Vector2f(515, 350));
                break;
            case 7:
                squares2[j].setPosition(Vector2f(645, 350));
                break;
            case 8: //a treia linie
                squares2[j].setPosition(Vector2f(265, 470));
                break;
            case 9:
                squares2[j].setPosition(Vector2f(390, 465));
                break;
            case 10:
                squares2[j].setPosition(Vector2f(520, 465));
                break;
            case 11:
                squares2[j].setPosition(Vector2f(645, 465));
                break;
        }
    }

    //Sus
    fenceSprites[0].setPosition(150, 10);
    fenceSprites[2].setPosition(SCREEN_X - 370, 10);

    //Stanga
    fenceSprites[1].setPosition(20, 120);
    fenceSprites[3].setPosition(20, 370);

    //Jos
    fenceSprites[4].setPosition(200, SCREEN_Y - 85);
    fenceSprites[5].setPosition(SCREEN_X - 420, SCREEN_Y - 100);

    //Dreapta
    fenceSprites[6].setPosition(SCREEN_X - 160, 210);
    fenceSprites[7].setPosition(SCREEN_X - 90, 260);

    Sprite* topSprite[8] = { nullptr };

    int selectedFence = -1;

    Color selectedFenceColor(172, 195, 52, 128), usedFenceColor(104, 104, 104, 128),
            unusableFenceColor(255, 0, 0, 128);

    RectangleShape shapeSelectedFences[8];
    RectangleShape shapeUsedFences[8];
    RectangleShape shapeUnusableFences[8];


    RectangleShape *topRectShapeSelected[8];
    RectangleShape *topRectShapeUsed[8];
    RectangleShape *topRectShapeUnusable[8];

    for(int i = 0; i < 8; i++) {
        shapeSelectedFences[i].setPosition(fenceSprites[i].getPosition());
        shapeSelectedFences[i].setSize(Vector2f(fenceSprites[i].getGlobalBounds().width, fenceSprites[i].getGlobalBounds().height));
        shapeSelectedFences[i].setFillColor(selectedFenceColor);

        shapeUsedFences[i].setPosition(fenceSprites[i].getPosition());
        shapeUsedFences[i].setSize(Vector2f(fenceSprites[i].getGlobalBounds().width, fenceSprites[i].getGlobalBounds().height));
        shapeUsedFences[i].setFillColor(usedFenceColor);

        shapeUnusableFences[i].setPosition(fenceSprites[i].getPosition());
        shapeUnusableFences[i].setSize(Vector2f(fenceSprites[i].getGlobalBounds().width, fenceSprites[i].getGlobalBounds().height));
        shapeUnusableFences[i].setFillColor(unusableFenceColor);
    }

    Texture boardTexture;
    if (!boardTexture.loadFromFile("../images/Board/board (1).png")) {
        return EXIT_FAILURE;
    }


    Sprite boardSprite;
    boardSprite.setTexture(boardTexture);
    boardSprite.setScale(1.f, 1.f);
    boardSprite.setPosition(80, 30);

    int win;

    bool winMusicPlayed = false, loseMusicPlayed = false;

    Texture soundOffTexture;
    if (!soundOffTexture.loadFromFile("../images/main_screen/sound_off.png")) {
        return EXIT_FAILURE;
    }

    Texture soundOnTexture;
    if (!soundOnTexture.loadFromFile("../images/main_screen/sound_on.png")) {
        return EXIT_FAILURE;
    }

    Sprite soundSprite(soundOnTexture);
    soundSprite.setScale(1.25, 1.25);
    soundSprite.setPosition(30, 30);

    bool soundOn = true;

    while (window.isOpen() && currentState != 3)
    {
        Event event;
        int mouseClicked = 0;
        Vector2i mousePos = Mouse::getPosition(window);

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
                mouseClicked = 1;

            if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
                mouseClicked = 0;

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && currentState == 4) {
                //Verifica daca mouse-ul a apasat pe vreun gard
                for(int i = 0; i < 8; i++)
                    if (fenceSprites[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && topSprite[i] == nullptr && isUsableFences[i]) {
                        isSelectedFences[i] = !isSelectedFences[i];

                        if(isSelectedFences[i]) {
                            selectedFence = i;
                            topRectShapeSelected[i] = &shapeSelectedFences[i];
                        }
                        else {
                            selectedFence = -1;
                            topRectShapeSelected[i] = nullptr;

                        }
                        for(int j = 0; j < 8; j++) {
                            if (j == i) continue;
                            isSelectedFences[j] = 0;
                            topRectShapeSelected[j] = nullptr;
                        }
                    }
            }

            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left && currentState == 4) {
                //Verifica daca mouse-ul a apasat pe vreun gard
                if(selectedFence != -1) {
                    for (int i = 0; i < 16; i++) {
                        if (squares[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {

                            spriteFencesBoard[selectedFence].setPosition(squares[i].getGlobalBounds().left - 50 + (selectedFence == 3 ? -100.f : 0.f) + (selectedFence == 4 ? 50.f : 0.f) + (selectedFence == 6 ? -40.f : 0.f) + (selectedFence == 7 ? -10.f : 0.f),
                                                                         squares[i].getGlobalBounds().top +
                                                                         ((i >= 4 && i <= 6 || i >= 9 && i <= 11) ? squares[i].getGlobalBounds().height / 3 - 5 : squares[i].getGlobalBounds().height / 3 - 20));
                            spriteFencesBoard[selectedFence].setScale(1.15f, 1.f);

                            if(selectedFence == 0 && (i < 3 || i == 5 || i > 10) || selectedFence == 1 && (i < 3 || i == 3 || i == 7) ||
                               selectedFence == 2 && (i >= 8 && i <= 11 || i == 3) || (selectedFence == 4 || selectedFence== 5) && (i > 12 || i < 3) ||
                               selectedFence == 7 && (i == 3 || i == 7 || i >= 8) || selectedFence == 3 && i >= 3) break;


                            if(spriteFencesBoard[selectedFence].getGlobalBounds().width + spriteFencesBoard[selectedFence].getGlobalBounds().left < boardSprite.getGlobalBounds().width + boardSprite.getGlobalBounds().left - 40 &&
                               spriteFencesBoard[selectedFence].getGlobalBounds().height + spriteFencesBoard[selectedFence].getGlobalBounds().top < boardSprite.getGlobalBounds().height + boardSprite.getGlobalBounds().top - 30) {

                                float positionXFence0 = (i == 3 ? 145.f : (i == 4 ? 270.f : (i == 8 ? 145.f : 270.f)));
                                float positionYFence0 = (i == 3 ? 250.f : (i == 4 ? 250.f : (i == 8 ? 373.f : 374.f)));

                                float positionXFence1 = (i == 4 ? 277.f : (i == 5 ? 403.f : (i == 6 ? 528.f : 0.f)));
                                float positionYFence1 = (i == 4 ? 245.f : (i == 5 ? 245.f : (i == 6 ? 245.f : 0.f)));

                                float positionXFence2 = (i == 0 ? 278.f : (i == 1 ? 402.f : (i == 4 ? 277.f : (i == 5 ? 402.f : 0.f))));
                                float positionYFence2 = (i == 0 ? 132.f : (i == 1 ? 132.f : (i == 4 ? 255.f : (i == 5 ? 255.f : 0.f))));

                                float positionXFence3 = (i == 0 ? 174.f : (i == 1 ? 298.f : (i == 2 ? 423.f : 0.f)));
                                float positionYFence3 = (i == 0 ? 155.f : (i == 1 ? 155.f : (i == 2 ? 155.f : 0.f)));

                                float positionXFence4 = (i == 3 || i == 8 ? 198.f : (i == 4 || i == 9 ? 323.f : (i == 5 || i == 10 ? 447.f : 0)));
                                float positionYFence4 = (i >= 3 && i <= 5 ? 260.f : 383.f);

                                float positionXFence5 = (i == 3 || i == 8 ? 172.f : 293.f);
                                float positionYFence5 = (i == 3 || i == 4 ? 238.f : 360.f);

                                float positionXFence6 = (i == 0 ? 235.f : (i == 1 ? 360.f : (i == 2 ? 485.f : 0.f)));
                                float positionYFence6 = (i == 0 ? 145.f : (i == 1 ? 145.f : (i == 2 ? 145.f : 0.f)));

                                float positionXFence7 = (i == 0 ? 252.f : (i == 1 ? 375.f : (i == 2 ? 500.f : (i == 4 ? 250.f : (i == 5 ? 377.f : (i == 6 ? 502.f : 0.f))))));
                                float positionYFence7 = (i == 0 ? 137.f : (i == 1 ? 137.f : (i == 2 ? 137.f : (i == 4 ? 262.f : (i == 5 ? 262.f : (i == 6 ? 262.f : 0.f))))));

                                spriteFencesBoard[selectedFence].setPosition(
                             selectedFence == 0 ? positionXFence0 : (
                                selectedFence == 1 ? positionXFence1 : (
                                selectedFence == 2 ? positionXFence2 : (
                                selectedFence == 3 ? positionXFence3 : (
                                selectedFence == 4 ? positionXFence4 : (
                                selectedFence == 5 ? positionXFence5 : (
                                selectedFence == 6 ? positionXFence6 : (
                                selectedFence == 7 ? positionXFence7 : 0
                                ))))))),
                             selectedFence == 0 ? positionYFence0 : (
                                selectedFence == 1 ? positionYFence1 : (
                                selectedFence == 2 ? positionYFence2 : (
                                selectedFence == 3 ? positionYFence3 : (
                                selectedFence == 4 ? positionYFence4 : (
                                selectedFence == 5 ? positionYFence5 : (
                                selectedFence == 6 ? positionYFence6 : (
                                selectedFence == 7 ? positionYFence7 : 0
                                ))))))));

                                spriteFencesBoard[selectedFence].setScale(1.f, 1.f);


                                int lineForFence = (i <= 2 ? 0 : (i >= 3 && i <= 7 ? 2 : (i >= 8 && i <= 12 ? 4 : 6)));
                                int colForFence = (i == 3 || i == 8 ? 0 : (i == 0 || i == 4 || i == 9 || i == 13 ? 2 : (i == 1 || i == 5 || i == 10 || i == 14 ? 4 : (i == 2 || i == 6 || i == 11 || i == 15 ? 6 : 8))));

                                int boardMatrixFencesMock[7][9];
                                for(int k = 0; k < 7; k++)
                                    for(int j = 0; j < 9; j++)
                                        boardMatrixFencesMock[k][j] = boardMatrixFences[k][j];

                                if(selectedFence == 0 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence + 2, 0, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence + 4, 0, lineForFence, colForFence + 2, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence + 4, 1, lineForFence, colForFence + 4, lineForFence, colForFence + 2)
                                   ) ||
                                   selectedFence == 1 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence + 2, 0, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence, 1, lineForFence, colForFence + 2, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 4), colForFence, 1, (lineForFence + 2), colForFence, lineForFence, colForFence + 2)
                                   ) ||
                                   selectedFence == 2 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence, 1, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence + 2, 0, (lineForFence + 2), colForFence, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence + 4, 0, (lineForFence + 2), colForFence + 2, (lineForFence + 2), colForFence)
                                   ) ||
                                   selectedFence == 3 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence, 1, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 4), colForFence, 1, (lineForFence + 2), colForFence, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 4), colForFence - 2, 0, (lineForFence + 4), colForFence, (lineForFence + 2), colForFence)
                                   ) ||
                                   selectedFence == 4 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence), colForFence + 2, 0, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence), colForFence + 4, 0, (lineForFence), colForFence + 2, lineForFence, colForFence)
                                   ) ||
                                   selectedFence == 5 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence), colForFence + 2, 0, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence), colForFence + 4, 0, (lineForFence), colForFence + 2, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence), colForFence + 6, 0, (lineForFence), colForFence + 4, (lineForFence), colForFence + 2)
                                   ) ||
                                   selectedFence == 6 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence, 1, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 4), colForFence, 1, (lineForFence + 2), colForFence, lineForFence, colForFence) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 6), colForFence, 1, (lineForFence + 4), colForFence, (lineForFence + 2), colForFence)
                                   ) ||
                                   selectedFence == 7 &&
                                   (
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, lineForFence, colForFence, 0, 0, 0, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 2), colForFence, 1, lineForFence, colForFence, 0, 0) ||
                                           isFenceNotBlocked(boardMatrix, boardMatrixFences, boardMatrixFencesMock, (lineForFence + 4), colForFence, 1, (lineForFence + 2), colForFence, lineForFence, colForFence)
                                   )
                                        ) break;

                                switch (selectedFence) {
                                    case 0:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence][colForFence + 2] += 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] += 2; boardMatrixFences[lineForFence + 2][colForFence + 4] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence][colForFence + 2] = 2;
                                        boardMatrix[lineForFence][colForFence + 4] = 2; boardMatrix[lineForFence + 2][colForFence + 4] = 2;
                                        break;
                                    case 1:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence][colForFence + 2] += 2;
                                        boardMatrixFences[lineForFence + 2][colForFence] += 2; boardMatrixFences[lineForFence + 4][colForFence] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence][colForFence + 2] = 2;
                                        boardMatrix[lineForFence + 2][colForFence] = 2; boardMatrix[lineForFence + 4][colForFence] = 2;
                                        break;
                                    case 2:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence + 2][colForFence] += 2;
                                        boardMatrixFences[lineForFence + 2][colForFence + 2] += 2; boardMatrixFences[lineForFence + 2][colForFence + 4] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence + 2][colForFence] = 2;
                                        boardMatrix[lineForFence + 2][colForFence + 2] = 2; boardMatrix[lineForFence + 2][colForFence + 4] = 2;
                                        break;
                                    case 3:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence + 2][colForFence] += 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] += 2; boardMatrixFences[lineForFence + 4][colForFence - 2] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence + 2][colForFence] = 2;
                                        boardMatrix[lineForFence + 4][colForFence] = 2; boardMatrix[lineForFence + 4][colForFence - 2] = 2;
                                        break;
                                    case 4:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence][colForFence + 2] += 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence][colForFence + 2] = 2;
                                        boardMatrix[lineForFence][colForFence + 4] = 2;
                                        break;
                                    case 5:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence][colForFence + 2] += 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] += 2; boardMatrixFences[lineForFence][colForFence + 6] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence][colForFence + 2] = 2;
                                        boardMatrix[lineForFence][colForFence + 4] = 2; boardMatrix[lineForFence][colForFence + 6] = 2;
                                        break;
                                    case 6:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence + 2][colForFence] += 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] += 2; boardMatrixFences[lineForFence + 6][colForFence] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence + 2][colForFence] = 2;
                                        boardMatrix[lineForFence + 4][colForFence] = 2; boardMatrix[lineForFence + 6][colForFence] = 2;
                                        break;
                                    case 7:
                                        boardMatrixFences[lineForFence][colForFence] += 2; boardMatrixFences[lineForFence + 2][colForFence] += 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] += 2;

                                        boardMatrix[lineForFence][colForFence] = 2; boardMatrix[lineForFence + 2][colForFence] = 2;
                                        boardMatrix[lineForFence + 4][colForFence] = 2;
                                        break;
                                    default:
                                        boardMatrixFences[lineForFence + 4][colForFence] += 2;

                                        boardMatrix[lineForFence + 4][colForFence] = 2;
                                        break;
                                }
                                fencesSquaresOrigins[selectedFence] = i;

                                if(selectedFence == 0) {
                                    isUsableFences[selectedFence] = isUsableFences[1] = isUsableFences[2] = isUsableFences[3] = 0;
                                    topRectShapeUnusable[1] = &shapeUnusableFences[1];
                                    topRectShapeUnusable[2] = &shapeUnusableFences[2];
                                    topRectShapeUnusable[3] = &shapeUnusableFences[3];
                                } else if(selectedFence == 1){
                                    isUsableFences[selectedFence] = isUsableFences[0] = isUsableFences[2] = isUsableFences[3] = 0;
                                    topRectShapeUnusable[0] = &shapeUnusableFences[0];
                                    topRectShapeUnusable[2] = &shapeUnusableFences[2];
                                    topRectShapeUnusable[3] = &shapeUnusableFences[3];
                                } else if(selectedFence == 2) {
                                    isUsableFences[selectedFence] = isUsableFences[0] = isUsableFences[1] = isUsableFences[3] = 0;
                                    topRectShapeUnusable[0] = &shapeUnusableFences[0];
                                    topRectShapeUnusable[1] = &shapeUnusableFences[1];
                                    topRectShapeUnusable[3] = &shapeUnusableFences[3];
                                } else if(selectedFence == 3) {
                                    isUsableFences[selectedFence] = isUsableFences[0] = isUsableFences[1] = isUsableFences[2] = 0;
                                    topRectShapeUnusable[0] = &shapeUnusableFences[0];
                                    topRectShapeUnusable[1] = &shapeUnusableFences[1];
                                    topRectShapeUnusable[2] = &shapeUnusableFences[2];
                                }else if(selectedFence == 5) {
                                    isUsableFences[selectedFence] = isUsableFences[selectedFence + 1] = 0;
                                    topRectShapeUnusable[selectedFence + 1] = &shapeUnusableFences[selectedFence + 1];
                                } else if(selectedFence == 6) {
                                    isUsableFences[selectedFence] = isUsableFences[selectedFence - 1] = 0;
                                    topRectShapeUnusable[selectedFence - 1] = &shapeUnusableFences[selectedFence - 1];
                                } else if(selectedFence == 4) {
                                    isUsableFences[selectedFence] = isUsableFences[selectedFence + 3] = 0;
                                    topRectShapeUnusable[selectedFence + 3] = &shapeUnusableFences[selectedFence + 3];
                                } else if(selectedFence == 7) {
                                    isUsableFences[selectedFence] = isUsableFences[selectedFence - 3] = 0;
                                    topRectShapeUnusable[selectedFence - 3] = &shapeUnusableFences[selectedFence - 3];
                                } else {
                                    isUsableFences[selectedFence] = isUsableFences[selectedFence - 2] = 0;
                                    topRectShapeUnusable[selectedFence - 2] = &shapeUnusableFences[selectedFence - 2];
                                }

                                fenceMusic.play();

                                topRectShapeSelected[selectedFence] = nullptr;
                                topRectShapeUsed[selectedFence] = &shapeUsedFences[selectedFence];
                                topSprite[selectedFence] = &spriteFencesBoard[selectedFence];

                                if (isLevelCompleted(boardMatrix, boardMatrixRes)) {
                                    levelsCompleted[levelSelected - 1] = 1;
                                    delayTime = 1;
                                }

                                selectedFence = -1;
                            }

                            break;
                        }
                    }
                } else {
                    for (int i = 0; i < 8; i++)
                        if (topSprite[i] != nullptr)
                            if (topSprite[i]->getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                                topSprite[i] = nullptr;
                                topRectShapeUsed[i] = nullptr;

                                if(i == 0) {
                                    isUsableFences[i] = isUsableFences[1] = isUsableFences[2] = isUsableFences[3] = 1;
                                    topRectShapeUnusable[i] = topRectShapeUnusable[1] = topRectShapeUnusable[2] = topRectShapeUnusable[3] = nullptr;
                                } else if(i == 1) {
                                    isUsableFences[i] = isUsableFences[0] = isUsableFences[2] = isUsableFences[3] = 1;
                                    topRectShapeUnusable[i] = topRectShapeUnusable[0] = topRectShapeUnusable[2] = topRectShapeUnusable[3] = nullptr;
                                } else if(i == 2) {
                                    isUsableFences[i] = isUsableFences[0] = isUsableFences[1] = isUsableFences[3] = 1;
                                    topRectShapeUnusable[i] = topRectShapeUnusable[0] = topRectShapeUnusable[1] = topRectShapeUnusable[3] = nullptr;
                                } else if(i == 3) {
                                    isUsableFences[i] = isUsableFences[0] = isUsableFences[1] = isUsableFences[2] = 1;
                                    topRectShapeUnusable[i] = topRectShapeUnusable[0] = topRectShapeUnusable[1] = topRectShapeUnusable[2] = nullptr;
                                } else if(i == 5) {
                                    isUsableFences[i] = isUsableFences[i + 1] = 1;
                                    topRectShapeUnusable[i] = nullptr;
                                    topRectShapeUnusable[i + 1] = nullptr;
                                } else if(i == 6) {
                                    isUsableFences[i] = isUsableFences[i - 1] = 1;
                                    topRectShapeUnusable[i] = nullptr;
                                    topRectShapeUnusable[i - 1] = nullptr;
                                } else if(i == 4) {
                                    isUsableFences[i] = isUsableFences[i + 3] = 1;
                                    topRectShapeUnusable[i] = nullptr;
                                    topRectShapeUnusable[i + 3] = nullptr;
                                } else if(i == 7) {
                                    isUsableFences[i] = isUsableFences[i - 3] = 1;
                                    topRectShapeUnusable[i] = nullptr;
                                    topRectShapeUnusable[i - 3] = nullptr;
                                } else {
                                    isUsableFences[i] = isUsableFences[i - 2] = 1;
                                    topRectShapeUnusable[i] = nullptr;
                                    topRectShapeUnusable[i - 2] = nullptr;
                                }

                                int square = fencesSquaresOrigins[i];

                                int lineForFence = (square <= 2 ? 0 : (square >= 3 && square <= 7 ? 2 : (square >= 8 && square <= 12 ? 4 : 6)));
                                int colForFence = (square == 3 || square == 8 ? 0 : (square == 0 || square == 4 || square == 9 || square == 13 ? 2 : (square == 1 || square == 5 || square == 10 || square == 14 ? 4 : (square == 2 || square == 6 || square == 11 || square == 15 ? 6 : 8))));

                                switch (i) {
                                    case 0:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence][colForFence + 2] -= 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] -= 2; boardMatrixFences[lineForFence + 2][colForFence + 4] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence][colForFence + 2] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 2]);
                                        boardMatrix[lineForFence][colForFence + 4] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 4]);
                                        boardMatrix[lineForFence + 2][colForFence + 4] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence + 4]);
                                        break;
                                    case 1:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence][colForFence + 2] -= 2;
                                        boardMatrixFences[lineForFence + 2][colForFence] -= 2; boardMatrixFences[lineForFence + 4][colForFence] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence][colForFence + 2] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 2]);
                                        boardMatrix[lineForFence + 2][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence]);
                                        boardMatrix[lineForFence + 4][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence]);
                                        break;
                                    case 2:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence + 2][colForFence] -= 2;
                                        boardMatrixFences[lineForFence + 2][colForFence + 2] -= 2; boardMatrixFences[lineForFence + 2][colForFence + 4] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence + 2][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence]);
                                        boardMatrix[lineForFence + 2][colForFence + 2] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence + 2]);
                                        boardMatrix[lineForFence + 2][colForFence + 4] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence + 4]);
                                        break;
                                    case 3:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence + 2][colForFence] -= 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] -= 2; boardMatrixFences[lineForFence + 4][colForFence - 2] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence + 2][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence]);
                                        boardMatrix[lineForFence + 4][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence]);
                                        boardMatrix[lineForFence + 4][colForFence - 2] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence - 2]);
                                        break;
                                    case 4:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence][colForFence + 2] -= 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence][colForFence + 2] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 2]);
                                        boardMatrix[lineForFence][colForFence + 4] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 4]);
                                        break;
                                    case 5:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence][colForFence + 2] -= 2;
                                        boardMatrixFences[lineForFence][colForFence + 4] -= 2; boardMatrixFences[lineForFence][colForFence + 6] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence][colForFence + 2] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 2]);
                                        boardMatrix[lineForFence][colForFence + 4] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 4]);
                                        boardMatrix[lineForFence][colForFence + 6] = isThereStillFence(boardMatrixFences[lineForFence][colForFence + 6]);
                                        break;
                                    case 6:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence + 2][colForFence] -= 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] -= 2; boardMatrixFences[lineForFence + 6][colForFence] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence + 2][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence]);
                                        boardMatrix[lineForFence + 4][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence]);
                                        boardMatrix[lineForFence + 6][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 6][colForFence]);
                                        break;
                                    case 7:
                                        boardMatrixFences[lineForFence][colForFence] -= 2; boardMatrixFences[lineForFence + 2][colForFence] -= 2;
                                        boardMatrixFences[lineForFence + 4][colForFence] -= 2;

                                        boardMatrix[lineForFence][colForFence] = isThereStillFence(boardMatrixFences[lineForFence][colForFence]);
                                        boardMatrix[lineForFence + 2][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 2][colForFence]);
                                        boardMatrix[lineForFence + 4][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence]);
                                        break;
                                    default:
                                        boardMatrixFences[lineForFence + 4][colForFence] -= 2;

                                        boardMatrix[lineForFence + 4][colForFence] = isThereStillFence(boardMatrixFences[lineForFence + 4][colForFence]);
                                        break;
                                }

                                fenceUnplaceMusic.play();

                                fencesSquaresOrigins[i] = -1;
                                isSelectedFences[i] = 0;

                                if(isLevelCompleted(boardMatrix, boardMatrixRes)) {
                                    levelsCompleted[levelSelected - 1] = 1;
                                    delayTime = 1;
                                }

                                break;
                            }
                }
            }
        }

        if((currentState == 0 || currentState == 1 || currentState == 2 || currentState == 4) && music.getStatus() != SoundSource::Playing) {
            winMusic.stop();
            loseMusic.stop();
            music.play();
            winMusicPlayed = loseMusicPlayed = false;
        }


        if (currentState == 0) {  // Main_Menu
            window.clear();
            window.draw(spriteBackground);
            window.draw(gameTitle);
            window.draw(buttonStart);
            window.draw(textStart);
            window.draw(buttonHTP);
            window.draw(textHTP);
            window.draw(buttonExit);
            window.draw(textExit);
            window.draw(soundSprite);

            if(soundOn) {
                soundSprite.setTexture(soundOnTexture);

                if(soundSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);

                    if (mouseClicked) {
                        soundOn = false;
                        music.setVolume(0);
                    }
                } else if(!buttonStart.getGlobalBounds().contains(mousePos.x, mousePos.y) && !buttonHTP.getGlobalBounds().contains(mousePos.x, mousePos.y) && !buttonExit.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    window.setMouseCursor(cursorDefault);
            } else {
                soundSprite.setTexture(soundOffTexture);

                if(soundSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);

                    if (mouseClicked) {
                        soundOn = true;
                        music.setVolume(50);
                    }
                } else if(!buttonStart.getGlobalBounds().contains(mousePos.x, mousePos.y) && !buttonHTP.getGlobalBounds().contains(mousePos.x, mousePos.y) && !buttonExit.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    window.setMouseCursor(cursorDefault);
            }


            Texture creditsTexture;
            if(!creditsTexture.loadFromFile("../images/main_screen/credits.png"))
                return EXIT_FAILURE;

            Sprite creditsSprite(creditsTexture);
            creditsSprite.setPosition(Vector2f(30, SCREEN_Y - 120));
            creditsSprite.setScale(1, 1);

            Text textNameP1;
            textNameP1.setString("Prof. coord. Bogdan Patrut");
            textNameP1.setCharacterSize(15);
            textNameP1.setPosition(Vector2f(140, SCREEN_Y - 120));
            textNameP1.setFillColor(Color::White);
            textNameP1.setOutlineThickness(2);
            textNameP1.setOutlineColor(Color::Black);
            textNameP1.setStyle(Text::Bold);
            textNameP1.setFont(fontConvergence);

            Text textNameP2;
            textNameP2.setString("Prof. coord. Hadimbu Stelian");
            textNameP2.setCharacterSize(15);
            textNameP2.setPosition(Vector2f(140, SCREEN_Y - 100));
            textNameP2.setFillColor(Color::White);
            textNameP2.setOutlineThickness(2);
            textNameP2.setOutlineColor(Color::Black);
            textNameP2.setStyle(Text::Bold);
            textNameP2.setFont(fontConvergence);

            Text textNameE1;
            textNameE1.setString("Ipatov Ioan Alexandru");
            textNameE1.setCharacterSize(15);
            textNameE1.setPosition(Vector2f(140, SCREEN_Y - 80));
            textNameE1.setFillColor(Color::White);
            textNameE1.setOutlineThickness(2);
            textNameE1.setOutlineColor(Color::Black);
            textNameE1.setStyle(Text::Bold);
            textNameE1.setFont(fontConvergence);

            Text textNameE2;
            textNameE2.setString("Stefan Nicholas");
            textNameE2.setCharacterSize(15);
            textNameE2.setPosition(Vector2f(140, SCREEN_Y - 60));
            textNameE2.setFillColor(Color::White);
            textNameE2.setOutlineThickness(2);
            textNameE2.setOutlineColor(Color::Black);
            textNameE2.setStyle(Text::Bold);
            textNameE2.setFont(fontConvergence);

            window.draw(creditsSprite);
            window.draw(textNameE1);
            window.draw(textNameE2);
            window.draw(textNameP1);
            window.draw(textNameP2);

            if (buttonStart.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);
                if (mouseClicked) {
                    mouseClicked = 0;
                    buttonPressMusic.play();
                    currentState = 1;  //Start
                }
            } else if (buttonHTP.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);
                if (mouseClicked) {
                    mouseClicked = 0;
                    buttonPressMusic.play();
                    currentState = 2;  //How to Play
                }
            } else if (buttonExit.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);
                if (mouseClicked) {
                    mouseClicked = 0;
                    buttonPressMusic.play();
                    currentState = 3;  //Exit
                }
            } else if(!soundSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorDefault);
            }

        } else if (currentState == 1) {  //Start
            window.clear();

            window.draw(spriteLevelsBackground);

            int isHoverLevel = 0;
            int isHoverBack = 0;

            int numLevels = 60;
            int levelsPerColumn = 7;
            int numColumns = 9;
            float offsetX = 85.0f;
            float offsetY = 70.0f;
            float startX = 60.0f;
            float startY = 60.0f;


            Texture textureHovLevel;
            if(!textureHovLevel.loadFromFile("../images/main_screen/hov_level_icon.png"))
                return EXIT_FAILURE;

            Texture textureCheckmark;
            if (!textureCheckmark.loadFromFile("../images/main_screen/checkmark (2).png"))
                return EXIT_FAILURE;

            Sprite spriteLevelDesign[numLevels];
            Sprite spriteCheckmark;
            spriteCheckmark.setTexture(textureCheckmark);

            for (int i = 0; i < numLevels; i++) {
                int col = i % numColumns;
                int row = i / numColumns;
                spriteLevelDesign[i].setTexture(textureLevelDesign);
                spriteLevelDesign[i].setPosition(startX + col * offsetX, startY + row * offsetY);
                window.draw(spriteLevelDesign[i]);

                //Numar nivel
                Text levelNumber(std::to_string(i + 1), fontConvergence);
                levelNumber.setCharacterSize(18);
                levelNumber.setFillColor(Color::White);
                levelNumber.setStyle(Text::Bold);


                //Centrare numar nivel
                FloatRect levelBounds = spriteLevelDesign[i].getGlobalBounds();
                FloatRect textBounds = levelNumber.getLocalBounds();
                float textX = levelBounds.left + (levelBounds.width - textBounds.width) / 2.0f - textBounds.left;
                float textY = levelBounds.top + (levelBounds.height - textBounds.height) / 2.0f - textBounds.top;
                levelNumber.setPosition(textX, textY);


                if (spriteLevelDesign[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);
                    isHoverLevel = 1;

                    if (mouseClicked) {
                        mouseClicked = 0;
                        buttonPressMusic.play();
                        levelSelected = i + 1;
                        generateBoardMatrix(boardMatrix, boardMatrixRes, levelSelected, boardMatrixDefault, boardMatrixFences, timeLimit);
                        clock.restart();
                        currentState = 4;
                    }

                    spriteLevelDesign[i].setTexture(textureHovLevel);
                    window.draw(spriteLevelDesign[i]);
                } else {
                    spriteLevelDesign[i].setTexture(textureLevelDesign);
                    window.draw(spriteLevelDesign[i]);
                }

                window.draw(levelNumber);

                if (levelsCompleted[i]) {
                    spriteCheckmark.setPosition(levelBounds.left + levelBounds.width - 50.f, levelBounds.top + 5.0f);
                    spriteCheckmark.setScale(0.1f, 0.1f);
                    window.draw(spriteCheckmark);
                }
            }


            Sprite backButton(textureBackButton);
            backButton.setScale(0.5f, 0.5f);
            backButton.setPosition(20, 20);
            window.draw(backButton);


            if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);
                isHoverBack = 1;

                if (mouseClicked) {
                    buttonPressMusic.play();
                    mouseClicked = 0;
                    currentState = 0;  //Main Menu
                }
            }

            if(!isHoverLevel && !isHoverBack)
                window.setMouseCursor(cursorDefault);

        } else if (currentState == 2) {  //How to Play
            window.clear();
            float offsetYText = 20;
            Sprite spriteHtpBackground(textureHtpBackground);
            spriteHtpBackground.setScale(1.25f, 1.40f);
            window.draw(spriteHtpBackground);

            Sprite backButton(textureBackButton);
            backButton.setScale(.5f, .5f);
            backButton.setPosition(20, 20);
            window.draw(backButton);

            Text textH1Obj("Obiectiv: ", fontConvergence);
            textH1Obj.setPosition(Vector2f(50, offsetYText + 120));
            textH1Obj.setCharacterSize(40);
            textH1Obj.setFillColor(Color::White);
            textH1Obj.setOutlineColor(Color::Black);
            textH1Obj.setOutlineThickness(2);

            Text textP1Obj_1("separa animalele folosind gardurile, astfel incat fiecare ", fontConvergence);
            textP1Obj_1.setPosition(Vector2f(230, offsetYText + 133));
            textP1Obj_1.setCharacterSize(25);
            textP1Obj_1.setFillColor(Color::White);
            textP1Obj_1.setOutlineColor(Color::Black);
            textP1Obj_1.setOutlineThickness(2);

            Text textP1Obj_2("tip de animal sa fie intr-un tarc separat. Toate tarcurile trebuie", fontConvergence);
            textP1Obj_2.setPosition(Vector2f(50, offsetYText + 173));
            textP1Obj_2.setCharacterSize(25);
            textP1Obj_2.setFillColor(Color::White);
            textP1Obj_2.setOutlineColor(Color::Black);
            textP1Obj_2.setOutlineThickness(2);

            Text textP1Obj_3(" sa aiba acces la o sursa de apa.", fontConvergence);
            textP1Obj_3.setPosition(Vector2f(50, offsetYText + 213));
            textP1Obj_3.setCharacterSize(25);
            textP1Obj_3.setFillColor(Color::White);
            textP1Obj_3.setOutlineColor(Color::Black);
            textP1Obj_3.setOutlineThickness(2);

            Text textH1Reg("Reguli: ", fontConvergence);
            textH1Reg.setPosition(Vector2f(50, offsetYText + 263));
            textH1Reg.setCharacterSize(40);
            textH1Reg.setFillColor(Color::White);
            textH1Reg.setOutlineColor(Color::Black);
            textH1Reg.setOutlineThickness(2);

            Text textP1Reg_1("Toate animalele de acelasi tip trebuie sa fie in acelasi tarc.", fontConvergence);
            textP1Reg_1.setPosition(Vector2f(190, offsetYText + 275));
            textP1Reg_1.setCharacterSize(25);
            textP1Reg_1.setFillColor(Color::White);
            textP1Reg_1.setOutlineColor(Color::Black);
            textP1Reg_1.setOutlineThickness(2);

            Text textP1Reg_2("Animalele de tipuri diferite nu pot imparti acelasi tarc.", fontConvergence);
            textP1Reg_2.setPosition(Vector2f(50, offsetYText + 315));
            textP1Reg_2.setCharacterSize(25);
            textP1Reg_2.setFillColor(Color::White);
            textP1Reg_2.setOutlineColor(Color::Black);
            textP1Reg_2.setOutlineThickness(2);

            Text textP1Reg_3("Fiecare tarc trebuie sa includa cel putin o sursa de apa.", fontConvergence);
            textP1Reg_3.setPosition(Vector2f(50, offsetYText + 350));
            textP1Reg_3.setCharacterSize(25);
            textP1Reg_3.setFillColor(Color::White);
            textP1Reg_3.setOutlineColor(Color::Black);
            textP1Reg_3.setOutlineThickness(2);

            Text textH1Rez("Rezolvare: ", fontConvergence);
            textH1Rez.setPosition(Vector2f(50, offsetYText + 390));
            textH1Rez.setCharacterSize(40);
            textH1Rez.setFillColor(Color::White);
            textH1Rez.setOutlineColor(Color::Black);
            textH1Rez.setOutlineThickness(2);

            Text textP1Rez_1("Utilizeaza gardurile disponibile pentru a crea ", fontConvergence);
            textP1Rez_1.setPosition(Vector2f(260, offsetYText + 405));
            textP1Rez_1.setCharacterSize(25);
            textP1Rez_1.setFillColor(Color::White);
            textP1Rez_1.setOutlineColor(Color::Black);
            textP1Rez_1.setOutlineThickness(2);

            Text textP1Rez_2("tarcurile necesare. Gardurile trebuie plasate pe grila jocului astfel", fontConvergence);
            textP1Rez_2.setPosition(Vector2f(50, offsetYText + 445));
            textP1Rez_2.setCharacterSize(25);
            textP1Rez_2.setFillColor(Color::White);
            textP1Rez_2.setOutlineColor(Color::Black);
            textP1Rez_2.setOutlineThickness(2);

            Text textP1Rez_3("incat sa respecte cerintele din provocare.", fontConvergence);
            textP1Rez_3.setPosition(Vector2f(50, offsetYText + 485));
            textP1Rez_3.setCharacterSize(25);
            textP1Rez_3.setFillColor(Color::White);
            textP1Rez_3.setOutlineColor(Color::Black);
            textP1Rez_3.setOutlineThickness(2);



            window.draw(textH1Obj);
            window.draw(textP1Obj_1);
            window.draw(textP1Obj_2);
            window.draw(textP1Obj_3);
            window.draw(textH1Reg);
            window.draw(textP1Reg_1);
            window.draw(textP1Reg_2);
            window.draw(textP1Reg_3);
            window.draw(textH1Rez);
            window.draw(textP1Rez_1);
            window.draw(textP1Rez_2);
            window.draw(textP1Rez_3);

            if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);
                if (mouseClicked) {
                    mouseClicked = 0;
                    buttonPressMusic.play();
                    currentState = 0;  //Main Menu
                }
            } else {
                window.setMouseCursor(cursorDefault);
            }
        } else if(currentState == 4) { //Level
            if(levelSelected == -1) {
                currentState = 1;
                continue;
            }

            window.clear();

            window.draw(boardSprite);

            RectangleShape rectBG;
            rectBG.setFillColor(customLevelColor);
            rectBG.setSize(Vector2f(SCREEN_X, SCREEN_Y));
            window.draw(rectBG);


            Sprite backButton(textureBackButton);
            backButton.setScale(0.5f, 0.5f);
            backButton.setPosition(20, 20);
            window.draw(backButton);

            //Deseneaza Board-ul
            window.draw(boardSprite);

            //Deseneaza gardurile
            window.draw(fenceSprites[0]);
            window.draw(fenceSprites[2]);

            window.draw(fenceSprites[1]);
            window.draw(fenceSprites[3]);

            window.draw(fenceSprites[4]);
            window.draw(fenceSprites[5]);

            window.draw(fenceSprites[6]);
            window.draw(fenceSprites[7]);

            for(int i = 0; i < 16; i++)
                window.draw(squares[i]);

            for(int j = 0; j < 12; j++)
                window.draw(squares2[j]);

            for (int i=0; i<7;i++)
                for (int j=0;j<9;j++)
                    if (boardMatrixDefault[i][j] != 0) {
                        Texture animalTexture;
                        if (!animalTexture.loadFromFile(animalAddress(boardMatrixDefault[i][j]))) {
                            return EXIT_FAILURE;
                        }
                        Sprite animalSprite;
                        animalSprite.setTexture(animalTexture);
                        animalSprite.setScale(0.8f, 0.8f);
                        animalSprite.setOrigin(
                                animalSprite.getLocalBounds().width / 2,
                                animalSprite.getLocalBounds().height / 2
                        );
                        animalSprite.setPosition(squares2[(i / 3 + (i > 4 ? 1 : 0)) * 4 + j / 2].getGlobalBounds().left, squares2[(i / 3 + (i > 4 ? 1 : 0)) * 4 + j / 2].getGlobalBounds().top );
                        window.draw(animalSprite);
                    }

            for(int i = 0; i < 8; i++)
                if(topSprite[i] != nullptr)
                    window.draw(*topSprite[i]);

            for(int i = 0; i < 8; i++)
                if(topRectShapeSelected[i] != nullptr)
                    window.draw(*topRectShapeSelected[i]);

            for(int i = 0; i < 8; i++)
                if(topRectShapeUsed[i] != nullptr)
                    window.draw(*topRectShapeUsed[i]);

            for(int i = 0; i < 8; i++)
                if(topRectShapeUnusable[i] != nullptr)
                    window.draw(*topRectShapeUnusable[i]);

            Text preLevelNumberText;
            preLevelNumberText.setString("Nivel:");
            preLevelNumberText.setCharacterSize(40);
            preLevelNumberText.setPosition(Vector2f(SCREEN_X - 180.f, 15.f));
            preLevelNumberText.setFillColor(Color::White);
            preLevelNumberText.setOutlineThickness(2);
            preLevelNumberText.setOutlineColor(Color::Black);
            preLevelNumberText.setStyle(Text::Bold);
            preLevelNumberText.setFont(fontConvergence);

            Text levelNumberText;
            levelNumberText.setString(to_string(levelSelected));
            levelNumberText.setCharacterSize(40);
            levelNumberText.setPosition(Vector2f(SCREEN_X - 60.f, 15.f));
            levelNumberText.setFillColor(Color::White);
            levelNumberText.setOutlineThickness(2);
            levelNumberText.setOutlineColor(Color::Black);
            levelNumberText.setStyle(Text::Bold);
            levelNumberText.setFont(fontConvergence);

            window.draw(levelNumberText);
            window.draw(preLevelNumberText);

            //TIMER
            Time elapsedTime = clock.getElapsedTime();

            if(elapsedTime.asSeconds() > timeLimit) {
                currentState = 5;
                win = 0;
            }


            Text timerText;
            timerText.setString(to_string(int(elapsedTime.asSeconds())));
            timerText.setFont(fontConvergence);
            timerText.setPosition(elapsedTime.asSeconds() >= 10 ? SCREEN_X - 155 : SCREEN_X - 130, SCREEN_Y - 100);
            timerText.setCharacterSize(50);
            timerText.setFillColor(Color::Black);
            timerText.setStyle(Text::Bold);

            Text timerTextLimit;
            timerTextLimit.setString("/" + to_string(int(timeLimit)));
            timerTextLimit.setFont(fontConvergence);
            timerTextLimit.setPosition(SCREEN_X - 100, SCREEN_Y - 100);
            timerTextLimit.setCharacterSize(50);
            timerTextLimit.setFillColor(Color::Black);
            timerTextLimit.setStyle(Text::Bold);

            window.draw(timerTextLimit);
            window.draw(timerText);


            if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                window.setMouseCursor(cursorHand);

                if (mouseClicked) {
                    mouseClicked = 0;
                    buttonPressMusic.play();
                    levelSelected = -1;
                    for(int i = 0; i < 7; i++)
                        for(int j = 0; j < 9; j++)
                            boardMatrix[i][j] = boardMatrixDefault[i][j] = boardMatrixRes[i][j] = boardMatrixFences[i][j] = 0;

                    for(int i = 0; i < 8; i++) {
                        topRectShapeUnusable[i] = nullptr;
                        topSprite[i] = nullptr;
                        topRectShapeSelected[i] = nullptr;
                        topRectShapeUsed[i] = nullptr;
                        isUsableFences[i] = 1;
                        isSelectedFences[i] = 1;
                        fencesSquaresOrigins[i] = -1;
                    }

                    currentState = 1;
                }
            } else {
                window.setMouseCursor(cursorDefault);
            }

            window.draw(backButton);
        } else if (currentState == 5) {
            window.clear();

            music.stop();

            Texture background1Texture;
            if (!background1Texture.loadFromFile("../images/Board/WinLose.jpg")) {
                return EXIT_FAILURE;
            }

            Sprite backgroundSprite(background1Texture);
            backgroundSprite.setScale(
                    SCREEN_X / backgroundSprite.getGlobalBounds().width,
                    SCREEN_Y / backgroundSprite.getGlobalBounds().height
            );

            window.draw(backgroundSprite);

            RectangleShape menuButton(Vector2f(200, 60));
            menuButton.setFillColor(customButtonColor);
            menuButton.setPosition(SCREEN_X / 2 - menuButton.getSize().x / 2, 350);

            Text menuButtonText;
            menuButtonText.setFont(fontConvergence);
            menuButtonText.setString("Levels Menu");
            menuButtonText.setCharacterSize(30);
            menuButtonText.setFillColor(Color::White);
            menuButtonText.setPosition(
                    SCREEN_X / 2 - menuButtonText.getGlobalBounds().width / 2,
                    350 + menuButton.getSize().y / 2 - 20
            );

            if (win == 1) {
                if (!winMusicPlayed) {
                    winMusicPlayed = true;
                    winMusic.play();
                }

                Text winText;
                winText.setFont(fontConvergence);
                winText.setString("You Won!");
                winText.setCharacterSize(80);
                winText.setFillColor(Color::White);
                winText.setOutlineColor(Color::Black);
                winText.setOutlineThickness(2);
                winText.setStyle(Text::Bold);
                winText.setPosition(SCREEN_X / 2 - winText.getGlobalBounds().width / 2, 100);
                window.draw(winText);


                RectangleShape nextButton(Vector2f(200, 60));
                nextButton.setFillColor(customButtonColor);
                nextButton.setPosition(SCREEN_X / 2 - nextButton.getSize().x / 2, 250);

                Text nextButtonText;
                nextButtonText.setFont(fontConvergence);
                nextButtonText.setString("Next Level");
                nextButtonText.setCharacterSize(30);
                nextButtonText.setFillColor(Color::White);
                nextButtonText.setPosition(
                        SCREEN_X / 2 - nextButtonText.getGlobalBounds().width / 2,
                        250 + nextButton.getSize().y / 2 - 20
                );
                if(levelSelected < 60) {
                    window.draw(nextButton);
                    window.draw(nextButtonText);
                }

                window.draw(menuButton);
                window.draw(menuButtonText);


                // Urmatorul nivel
                if (levelSelected < 60 && nextButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);
                    if (mouseClicked) {
                        mouseClicked = false;

                        buttonPressMusic.play();

                        winMusicPlayed = false;

                        for(int i = 0; i < 7; i++)
                            for(int j = 0; j < 9; j++)
                                boardMatrix[i][j] = boardMatrixDefault[i][j] = boardMatrixRes[i][j] = boardMatrixFences[i][j] = 0;

                        for(int i = 0; i < 8; i++) {
                            topRectShapeUnusable[i] = nullptr;
                            topSprite[i] = nullptr;
                            topRectShapeSelected[i] = nullptr;
                            topRectShapeUsed[i] = nullptr;
                            isUsableFences[i] = 1;
                            isSelectedFences[i] = 1;
                            fencesSquaresOrigins[i] = -1;
                        }

                        levelSelected++;

                        generateBoardMatrix(boardMatrix, boardMatrixRes, levelSelected, boardMatrixDefault, boardMatrixFences, timeLimit);
                        clock.restart();

                        currentState = 4;
                    }
                } else if(menuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);
                    if (mouseClicked) {
                        mouseClicked = false;
                        buttonPressMusic.play();

                        for (int i = 0; i < 7; i++)
                            for (int j = 0; j < 9; j++)
                                boardMatrix[i][j] = boardMatrixDefault[i][j] = boardMatrixRes[i][j] = boardMatrixFences[i][j] = 0;

                        for (int i = 0; i < 8; i++) {
                            topRectShapeUnusable[i] = nullptr;
                            topSprite[i] = nullptr;
                            topRectShapeSelected[i] = nullptr;
                            topRectShapeUsed[i] = nullptr;
                            isUsableFences[i] = 1;
                            isSelectedFences[i] = 1;
                            fencesSquaresOrigins[i] = -1;
                        }

                        levelSelected = -1;

                        currentState = 1;
                    }
                } else {
                    window.setMouseCursor(cursorDefault);
                }
            } else if (win == 0) {
                if (!loseMusicPlayed) {
                    loseMusicPlayed = true;
                    loseMusic.play();
                }

                Text loseText;
                loseText.setFont(fontConvergence);
                loseText.setString("You Lose!");
                loseText.setCharacterSize(60);
                loseText.setFillColor(Color::White);
                loseText.setOutlineColor(Color::Black);
                loseText.setOutlineThickness(2);
                loseText.setStyle(Text::Bold);
                loseText.setPosition(SCREEN_X / 2 - loseText.getGlobalBounds().width / 2, 100);
                window.draw(loseText);

                RectangleShape retryButton(Vector2f(200, 60));
                retryButton.setFillColor(customButtonColor);
                retryButton.setPosition(SCREEN_X / 2 - retryButton.getSize().x / 2, 250);
                window.draw(retryButton);

                Text retryButtonText;
                retryButtonText.setFont(fontConvergence);
                retryButtonText.setString("Retry");
                retryButtonText.setCharacterSize(30);
                retryButtonText.setFillColor(Color::White);
                retryButtonText.setPosition(
                        SCREEN_X / 2 - retryButtonText.getGlobalBounds().width / 2,
                        250 + retryButton.getSize().y / 2 - 20
                );
                window.draw(retryButtonText);

                window.draw(menuButton);
                window.draw(menuButtonText);

                if (retryButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);
                    if (mouseClicked) {
                        mouseClicked = false;
                        buttonPressMusic.play();

                        loseMusicPlayed = false;

                        for(int i = 0; i < 7; i++)
                            for(int j = 0; j < 9; j++)
                                boardMatrix[i][j] = boardMatrixDefault[i][j] = boardMatrixRes[i][j] = boardMatrixFences[i][j] = 0;

                        for(int i = 0; i < 8; i++) {
                            topRectShapeUnusable[i] = nullptr;
                            topSprite[i] = nullptr;
                            topRectShapeSelected[i] = nullptr;
                            topRectShapeUsed[i] = nullptr;
                            isUsableFences[i] = 1;
                            isSelectedFences[i] = 1;
                            fencesSquaresOrigins[i] = -1;
                        }

                        generateBoardMatrix(boardMatrix, boardMatrixRes, levelSelected, boardMatrixDefault, boardMatrixFences, timeLimit);
                        clock.restart();

                        currentState = 4;
                    }
                } else if(menuButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.setMouseCursor(cursorHand);
                    if (mouseClicked) {
                        mouseClicked = false;
                        buttonPressMusic.play();

                        for (int i = 0; i < 7; i++)
                            for (int j = 0; j < 9; j++)
                                boardMatrix[i][j] = boardMatrixDefault[i][j] = boardMatrixRes[i][j] = boardMatrixFences[i][j] = 0;

                        for (int i = 0; i < 8; i++) {
                            topRectShapeUnusable[i] = nullptr;
                            topSprite[i] = nullptr;
                            topRectShapeSelected[i] = nullptr;
                            topRectShapeUsed[i] = nullptr;
                            isUsableFences[i] = 1;
                            isSelectedFences[i] = 1;
                            fencesSquaresOrigins[i] = -1;
                        }

                        levelSelected = -1;

                        currentState = 1;
                    }
                } else {
                    window.setMouseCursor(cursorDefault);
                }
            }
        }

        window.display();

        if(delayTime) {
            sleep(seconds(.1));
            delayTime = 0;
            win = 1;
            currentState = 5;
        }
    }

    return 0;
}