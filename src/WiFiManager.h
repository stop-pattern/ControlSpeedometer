#pragma once

/**
 * @class WiFiManager
 * @brief WiFi接続管理クラス
 *
 * WiFiアクセスポイント・ステーションモードの設定と
 * mDNSサービスの管理を行うクラス
 */
class WiFiManager {
  public:
    /**
     * @brief コンストラクタ
     */
    WiFiManager();

    /**
     * @brief WiFiシステムの初期化と接続
     *
     * APモードとSTAモードの同時動作（APSTA）を設定し、
     * アクセスポイントの開始とWiFi接続、mDNSサービスの開始を行う
     */
    void begin();

    /**
     * @brief WiFi接続状態の確認
     *
     * @return bool WiFiに接続されている場合true、そうでなければfalse
     */
    bool isConnected();

    /**
     * @brief メインループ処理
     *
     * WiFi接続状態の確認を行う
     */
    void loop();
};
