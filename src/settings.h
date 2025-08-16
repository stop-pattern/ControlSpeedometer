#pragma once

namespace settings {
// ----- WiFi設定 -----
constexpr char ssid[] = "HollyElectric";    ///< アクセスポイントのSSID
constexpr char password[] = "password";     ///< アクセスポイントのパスワード
constexpr unsigned int ip = 0xC0A80001;     ///< アクセスポイントのIPアドレス
constexpr unsigned int subnet = 0xFFFFFF00; ///< サブネットマスク
constexpr unsigned int polling = 100;       ///< ポーリング間隔（100ms）
constexpr unsigned int timeout = 100;       ///< タイムアウト（10s）
constexpr char hostname[] = "esp32";        ///< mDNSホスト名
// constexpr char hostname[] = "bargraph";     ///< mDNSホスト名
// constexpr char hostname[] = "speedometer";  ///< mDNSホスト名
} // namespace settings
