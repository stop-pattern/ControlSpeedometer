
#include "WiFiManager.h"
#include "settings.h"
#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>

namespace {
void onWiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);
} // namespace

/**
 * @brief WiFiManagerクラスのコンストラクタ
 */
WiFiManager::WiFiManager() {}

/**
 * @brief WiFiシステムの初期化と接続
 *
 * 1. WiFiをAPSTAモード（アクセスポイント+ステーション同時動作）に設定
 * 2. ソフトアクセスポイントを開始
 * 3. AP設定（IP、サブネット）を適用
 * 4. 既存のWiFiネットワークへの接続を試行（タイムアウト付き）
 * 5. mDNSサービスを開始してホスト名を設定（タイムアウト付き）
 */
void WiFiManager::begin() {
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.softAP(settings::ssid);
    delay(100);
    WiFi.onEvent(::onWiFiDisconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.softAPConfig(settings::ip, settings::ip, settings::subnet);
    WiFi.begin();

    // WiFi接続試行（タイムアウト付き）
    int wifiTimeout = 0;
    while (WiFi.status() != WL_CONNECTED && wifiTimeout < settings::timeout) {
        Serial.print(".");
        delay(settings::polling);
        wifiTimeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("connected to WiFi");
        log_d("IP address: %s", WiFi.localIP().toString().c_str());
    } else {
        Serial.println("WiFi connection failed, continuing with AP mode only");
    }

    // mDNS初期化試行（タイムアウト付き）
    int mdnsTimeout = 0;
    while (!MDNS.begin(settings::hostname) && mdnsTimeout < settings::timeout) {
        delay(settings::polling);
        mdnsTimeout++;
    }

    if (mdnsTimeout < settings::timeout) {
        log_d("mdns started: %s.local", settings::hostname);
    } else {
        log_e("mdns initialization failed");
    }
}

/**
 * @brief WiFi接続状態の確認
 *
 * WiFiステーション接続の状態を返す
 *
 * @return bool 接続されている場合true、そうでなければfalse
 */
bool WiFiManager::isConnected() { return WiFi.status() == WL_CONNECTED; }

/**
 * @brief メインループ処理
 *
 * WiFi接続状態の確認を行う
 * 必要に応じて再接続を試みる
 */
void WiFiManager::loop() {
    // WiFi接続状態の確認
    if (!isConnected()) {
        log_w("WiFi disconnected, attempting to reconnect");
        WiFi.disconnect();
        WiFi.reconnect();
    }
}

namespace {
/**
 * @brief WiFi切断イベントハンドラ
 *
 * WiFi接続が切断された際に呼び出される
 *
 * @param event WiFiイベント
 * @param info イベント情報
 */
void onWiFiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
    log_e("WiFi disconnected, reason: %d", info.wifi_sta_disconnected.reason);
    WiFi.begin();
    WiFi.reconnect();
}
} // namespace
