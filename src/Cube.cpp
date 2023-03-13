// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

// Connect the Unit_Glass to Port A.
// On Unit_Gass, press the buttons on the left/right side of the Unit Glass to
// zoom/dezoom the 3D cube. On M5Stack, press the buttons left/right of the
// M5Stack to zoom/dezoom the 3D cube.

#include <M5Stack.h>
#include <M5GFX.h>
#include <UNIT_GLASS.h>

#define ROTATION_SPEED 10
#define MAX_LCD_SIZE   10000
#define MAX_GLASS_SIZE 2400

UNIT_GLASS glass;
M5GFX display;

LGFX_Sprite cubeSprite(&display);

boolean btnLcdA, btnLcdB, btnLcdC;
boolean btnGlassA, btnGlassB;

float px[] = {-1, 1, 1, -1, -1, 1, 1, -1};
float py[] = {-1, -1, 1, 1, -1, -1, 1, 1};
float pz[] = {-1, -1, -1, -1, 1, 1, 1, 1};

int16_t p2x[] = {0, 0, 0, 0, 0, 0, 0, 0};
int16_t p2y[] = {0, 0, 0, 0, 0, 0, 0, 0};

float r[] = {0, 0, 0};

float zoomLcd   = 1;
float zoomGlass = 1;

void getButton() {
    // Manage buttons on M5Stack Lcd
    M5.update();

    btnLcdA = M5.BtnA.isPressed();
    btnLcdB = M5.BtnB.isPressed();
    btnLcdC = M5.BtnC.isPressed();

    // Manage buttons on M5Stack Glass Unit
    glass.getKeyA() == 0 ? btnGlassA = true : btnGlassA = false;
    glass.getKeyB() == 0 ? btnGlassB = true : btnGlassB = false;
}

void drawCube(uint8_t type, uint16_t width, int16_t height, int16_t size) {
    for (uint8_t i = 0; i < 3; i++) {
        r[i] = r[i] + PI / 180.0;
        if (r[i] >= 360.0 * PI / 180.0) r[i] = 0;
    }

    for (uint8_t i = 0; i < 8; i++) {
        float px2 = px[i];
        float py2 = cos(r[0]) * py[i] - sin(r[0]) * pz[i];
        float pz2 = sin(r[0]) * py[i] + cos(r[0]) * pz[i];

        float px3 = cos(r[1]) * px2 + sin(r[1]) * pz2;
        float py3 = py2;
        float pz3 = -sin(r[1]) * px2 + cos(r[1]) * pz2;

        float ax = cos(r[2]) * px3 - sin(r[2]) * py3;
        float ay = sin(r[2]) * px3 + cos(r[2]) * py3;
        float az = pz3 - 150;

        p2x[i] = (int16_t)(width / 2 + ax * size / az);
        p2y[i] = (int16_t)(height / 2 + ay * size / az);
    }

    if (type == 0)  // M5Stack Lcd screen
    {
        cubeSprite.clear();
        for (uint8_t i = 0; i < 3; i++) {
            cubeSprite.drawLine(p2x[i], p2y[i], p2x[i + 1], p2y[i + 1],
                                TFT_WHITE);
            cubeSprite.drawLine(p2x[i + 4], p2y[i + 4], p2x[i + 5], p2y[i + 5],
                                TFT_WHITE);
            cubeSprite.drawLine(p2x[i], p2y[i], p2x[i + 4], p2y[i + 4],
                                TFT_WHITE);
        }

        cubeSprite.drawLine(p2x[3], p2y[3], p2x[0], p2y[0], TFT_WHITE);
        cubeSprite.drawLine(p2x[7], p2y[7], p2x[4], p2y[4], TFT_WHITE);
        cubeSprite.drawLine(p2x[3], p2y[3], p2x[7], p2y[7], TFT_WHITE);

        cubeSprite.pushSprite(40, 0, TFT_TRANSPARENT);
    } else  // M5Stack Glass Unit
    {
        glass.clear();
        for (uint8_t i = 0; i < 3; i++) {
            glass.draw_line(p2x[i], p2y[i], p2x[i + 1], p2y[i + 1], 1);
            glass.show();
            glass.draw_line(p2x[i + 4], p2y[i + 4], p2x[i + 5], p2y[i + 5], 1);
            glass.show();
            glass.draw_line(p2x[i], p2y[i], p2x[i + 4], p2y[i + 4], 1);
            glass.show();
        }

        glass.draw_line(p2x[3], p2y[3], p2x[0], p2y[0], 1);
        glass.draw_line(p2x[7], p2y[7], p2x[4], p2y[4], 1);
        glass.draw_line(p2x[3], p2y[3], p2x[7], p2y[7], 1);
        glass.show();
    }
}

void setup() {
    M5.begin();

    // M5Stack Lcd
    display.begin();
    display.clear();

    cubeSprite.setColorDepth(8);
    cubeSprite.createSprite(240, 240);

    // M5Stack Glass Unit
    glass.begin(&Wire, GLASS_ADDR, 21, 22, 100000UL);
    glass.clear();
}

void loop() {
    getButton();

    // Manage zoom on M5Stack Lcd
    if (btnLcdA) {
        zoomLcd -= 0.01;
        if (zoomLcd < 0) zoomLcd = 0;
    } else if (btnLcdC) {
        zoomLcd += 0.01;
        if (zoomLcd > 1) zoomLcd = 1;
    }

    // Manage zoom on M5Stack Glass Unit
    if (btnGlassA || btnGlassB) {
        if (btnGlassA) {
            zoomGlass += 0.01;
            if (zoomGlass > 1) zoomGlass = 1;
        } else if (btnGlassB) {
            zoomGlass -= 0.01;
            if (zoomGlass < 0) zoomGlass = 0;
        }
        // Sound alert in limit
        if (zoomGlass == 0 || zoomGlass == 1) {
            glass.setBuzzer(1200, 8);
            glass.enable_buzz();
            delay(50);
            glass.disable_buzz();
        }
    }

    // Serial.printf("Lcd %d %d %d %f / Glass %d %d %f\n", btnLcdA, btnLcdB,
    // btnLcdC, zoomLcd, btnGlassA, btnGlassB, zoomGlass);

    drawCube(0, 240, 240, MAX_LCD_SIZE * zoomLcd);
    drawCube(1, 128, 58, MAX_GLASS_SIZE * zoomGlass);
    delay(ROTATION_SPEED);
}