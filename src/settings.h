#pragma once

namespace settings {
// ----- デバッグ設定 -----
constexpr bool debug = false; ///< デバッグモード（未使用）
// ----- WiFi設定 -----
constexpr char ssid[] = "HollyElectric";    ///< アクセスポイントのSSID
constexpr char password[] = "password";     ///< アクセスポイントのパスワード
constexpr unsigned int ip = 0x0100A8C0;     ///< アクセスポイントのIPアドレス
                                            //// 192.168.0.1/24
constexpr unsigned int subnet = 0x00FFFFFF; ///< サブネットマスク
constexpr unsigned int polling = 100;       ///< ポーリング間隔（100ms）
constexpr unsigned int timeout = 100;       ///< タイムアウト（10s）
constexpr char hostname[] = "esp32";        ///< mDNSホスト名
// constexpr char hostname[] = "bargraph";     ///< mDNSホスト名
// constexpr char hostname[] = "speedometer";  ///< mDNSホスト名
// ----- bargraph設定 -----
constexpr int minSpeed = 0;         ///< 最小速度（0km/h）
constexpr int maxSpeed = 180;       ///< 最大速度（180km/h）
constexpr int speedStep = 1;        ///< 速度変化量（1km/h）
constexpr int updateInterval = 300; ///< 更新間隔（300ms）
constexpr int threshold = 10000;    ///< モード変更の閾値（10s）
// ----- pin設定 -----
constexpr int pin_out = 4;   ///< PFM出力ピン
constexpr int pin_ledR = 26; ///< 赤LED制御ピン
constexpr int pin_ledG = 25; ///< 緑LED制御ピン
} // namespace settings
