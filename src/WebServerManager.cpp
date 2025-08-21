
#include "WebServerManager.h"
#include "BarGraph.h"
#include "Settings.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <FS.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

extern BarGraph barGraph; ///< バーグラフ制御インスタンス

/**
 * @brief WebServerManagerクラスのコンストラクタ
 */
WebServerManager::WebServerManager() {
    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");
}

// /**
//  * @brief 全クライアントへの状態通知
//  *
//  * 現在のシステム状態をJSON形式でWebSocketを通じて
//  * 接続中の全クライアントに送信する
//  */
// void WebServerManager::notifyAllClients() {
//     // 状態管理は必要に応じて外部から取得
//     JsonDocument doc;
//     doc["type"] = "state";
//     // doc["states"] = ...
//     // doc["speed"] = ...
//     String json;
//     serializeJson(doc, json);
//     ws->textAll(json);
// }

/**
 * @brief 全クライアントへの状態通知
 */
void WebServerManager::notifyAllClients() {
    JsonDocument doc;
    doc["type"] = "state";
    JsonObject states = doc["states"].to<JsonObject>();
    states["green"] = pinGreen;
    states["red"] = pinRed;
    doc["speed"] = barGraph.getSpeed();
    String json;
    serializeJson(doc, json);
    ws->textAll(json);
}

/**
 * @brief WebSocketメッセージ受信時の処理
 *
 * クライアントから受信したWebSocketメッセージを解析し、適切な処理を実行する
 *
 * @param server WebSocketサーバーオブジェクト
 * @param client WebSocketクライアントオブジェクト
 * @param info フレーム情報
 * @param data 受信データ
 * @param len データ長
 * @param type メッセージタイプ
 * @param fin フレーム終了フラグ
 */
void WebServerManager::onWebSocketMessage(AsyncWebSocket *server,
                                          AsyncWebSocketClient *client,
                                          AwsFrameInfo *info, char *data,
                                          size_t len, AwsFrameType type,
                                          bool fin) {
    if (type == WS_TEXT) {
        JsonDocument doc;
        deserializeJson(doc, data);
        String id = doc["id"];
        String typeStr = doc["type"];

        if (typeStr == "toggle" && id == "green") {
            setPinGreen(!pinGreen);
        } else if (typeStr == "toggle" && id == "red") {
            setPinRed(!pinRed);
        } else if (typeStr == "speed") {
            int speed = doc["value"];
            if (speed < 0) {
                speed = 0; // 最小値
            } else if (speed > 180) {
                speed = 180; // 最大値
            }
            barGraph.update(speed);
        }
        notifyAllClients();
    }
}

/**
 * @brief Webサーバーシステムの初期化
 *
 * 1. SPIFFSファイルシステムを初期化
 * 2. HTTPルートハンドラーを設定（/、静的ファイル、404エラー）
 * 3. WebSocketイベントハンドラーを設定
 * 4. WebSocketをサーバーに追加
 * 5. Webサーバーを開始
 */
void WebServerManager::begin() {
    // SPIFFSの初期化
    if (!SPIFFS.begin(true)) { // FORMAT_SPIFFS_IF_FAILED
        log_e("SPIFFS Mount Failed");
        esp_restart();
    }
    SPIFFS.begin();
    log_d("SPIFFS mounted");

    // HTTPハンドラーの設定
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/test.html", "text/html");
    });
    server->serveStatic("/", SPIFFS, "/www/").setDefaultFile("test.html");
    server->onNotFound(notFound);

    // WebSocket設定
    ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                       AwsEventType type, void *arg, uint8_t *data,
                       size_t len) {
        if (type == WS_EVT_DATA) {
            onWebSocketMessage(server, client, (AwsFrameInfo *)arg,
                               (char *)data, len, WS_TEXT, true);
        } else if (type == WS_EVT_CONNECT) {
            notifyAllClients();
        }
    });
    server->addHandler(ws);

    // Webサーバーを開始
    server->begin();
    log_d("web server started");
}

/**
 * @brief クライアント処理のハンドリング
 *
 * 定期的に呼び出されるクライアント処理メソッド
 * 現在の実装では具体的な処理は行わない
 */
void WebServerManager::handleClient() {
    // 必要に応じてクライアント処理
}

/**
 * @brief 緑LEDピンの状態設定
 *
 * @param state 設定する状態（true: ON, false: OFF）
 */
void WebServerManager::setPinGreen(bool state) {
    pinGreen = state;
    digitalWrite(settings::pin_ledG, state ? HIGH : LOW);
    changeMode();
}

/**
 * @brief 赤LEDピンの状態設定
 *
 * @param state 設定する状態（true: ON, false: OFF）
 */
void WebServerManager::setPinRed(bool state) {
    pinRed = state;
    digitalWrite(settings::pin_ledR, state ? HIGH : LOW);
    changeMode();
}

/**
 * @brief バーグラフモードの変更
 */
void WebServerManager::changeMode() {
    if (pinGreen && pinRed) {
        barGraph.changeMode(BarGraphMode::MODE_WEB);
    } else if (pinGreen) {
        barGraph.changeMode(BarGraphMode::MODE_MAX);
    } else if (pinRed) {
        barGraph.changeMode(BarGraphMode::MODE_0);
    } else {
        barGraph.changeMode(BarGraphMode::MODE_DEMO);
    }
}

namespace {
/**
 * @brief 404 Not Found ハンドラー
 *
 * 存在しないリソースへのアクセス時に呼び出される
 */
void notFound(AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
        request->send(200);
    } else {
        request->send(404, "text/plain", "File not found");
    }
}
} // namespace
