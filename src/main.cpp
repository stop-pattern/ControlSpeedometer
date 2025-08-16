#include <Arduino.h>

#include "WiFiManager.h"

WiFiManager wifiManager; ///< WiFi管理インスタンス

/**
 * @brief システム初期化処理
 *
 * 各機能を初期化する
 * 初期化順序：シリアル通信 -> WiFi
 */
void setup() {
    Serial.begin(115200);
    Serial.println();
    log_d("setup start");

    wifiManager.begin();

    log_d("setup end");
}

/**
 * @brief メインループ処理
 *
 * 処理を定期的に実行する
 * WiFi接続状態の確認を行い、必要に応じて再接続を試みる
 */
void loop() {
    wifiManager.loop();
    delay(100);
}
