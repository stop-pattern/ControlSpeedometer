#include <Arduino.h>

#include "BarGraph.h"
#include "Settings.h"
#include "WebServerManager.h"
#include "WiFiManager.h"

BarGraph barGraph; ///< バーグラフ制御インスタンス

namespace {
WiFiManager wifiManager;           ///< WiFi管理インスタンス
WebServerManager webServerManager; ///< Webサーバー管理インスタンス
} // namespace

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

    barGraph.begin();
    wifiManager.begin();
    webServerManager.begin();

    log_d("setup end");
    if (settings::debug) {
        log_d("Debug mode is enabled");
        barGraph.changeMode(BarGraphMode::MODE_DEMO);
    } else {
        log_d("Debug mode is disabled");
        barGraph.changeMode(BarGraphMode::MODE_WEB);
    }
}

/**
 * @brief メインループ処理
 *
 * 処理を定期的に実行する
 * WiFi接続状態の確認を行い、必要に応じて再接続を試みる
 */
void loop() {

    barGraph.loop();
    wifiManager.loop();
    // webServerManager.handleClient();

    delay(50);
}
