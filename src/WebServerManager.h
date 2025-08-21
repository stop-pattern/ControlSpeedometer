#pragma once
#include <ESPAsyncWebServer.h>

namespace {
/**
 * @brief 404 Not Found ハンドラー
 *
 * 存在しないリソースへのアクセス時に呼び出される
 *
 * @param request HTTPリクエストオブジェクト
 */
static void notFound(AsyncWebServerRequest *request);
} // namespace

/**
 * @class WebServerManager
 * @brief Webサーバー・WebSocket管理クラス
 *
 * HTTPサーバー、WebSocket通信、SPIFFSファイルシステムを管理し、
 * Webインターフェースを提供するクラス
 */
class WebServerManager {
  public:
    /**
     * @brief コンストラクタ
     */
    WebServerManager();

    /**
     * @brief Webサーバーシステムの初期化
     *
     * SPIFFS初期化、HTTPルート設定、WebSocket設定、サーバー開始を行う
     */
    void begin();

    /**
     * @brief クライアント処理のハンドリング
     *
     * 定期的に呼び出されるクライアント処理メソッド
     * （現在の実装では処理内容なし）
     */
    void handleClient();

    /**
     * @brief 緑LEDピンの状態設定
     *
     * @param state 設定する状態（true: ON, false: OFF）
     */
    void setPinGreen(bool state);

    /**
     * @brief 赤LEDピンの状態設定
     *
     * @param state 設定する状態（true: ON, false: OFF）
     */
    void setPinRed(bool state);

  private:
    /**
     * @brief 全クライアントへの状態通知
     *
     * 現在の状態をJSON形式でWebSocketを通じて全接続クライアントに送信
     */
    void notifyAllClients();

    /**
     * @brief WebSocketメッセージ処理
     *
     * クライアントから受信したWebSocketメッセージを解析・処理する
     *
     * @param client WebSocketクライアントオブジェクト
     * @param message 受信したメッセージ
     */
    void handleWebSocketMessage(AsyncWebSocketClient *client,
                                const String &message);

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
    void onWebSocketMessage(AsyncWebSocket *server,
                            AsyncWebSocketClient *client, AwsFrameInfo *info,
                            char *data, size_t len, AwsFrameType type,
                            bool fin);

    /**
     * @brief バーグラフモードの変更
     *
     * 現在のモードに応じてバーグラフの表示を切り替える
     */
    void changeMode();

    // サーバーインスタンスをメンバ変数として管理
    AsyncWebServer *server; ///< HTTPサーバーインスタンス
    AsyncWebSocket *ws;     ///< WebSocketインスタンス

    // GPIOピンの状態管理
    bool pinGreen;
    bool pinRed;
};
