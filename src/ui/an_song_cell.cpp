#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/cocos.hpp>

#include <Geode/Geode.hpp>
#include <fleym.nongd/include/jukebox.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/binding/CustomSongWidget.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/binding/SongInfoObject.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/Loader.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/IDManager.hpp>
#include <Geode/ui/Popup.hpp>
#include "../types/an_song.hpp"
#include "list_cell.hpp"
#include "../utils/utils.hpp"

#include "Geode/loader/Log.hpp"

using namespace geode::prelude;

#include "an_song_cell.hpp"

bool ANSongCell::init(int songId, ANSong* song, ANDropdownLayer* parentPopup, CCSize const& size) {
    if (!JBListCell::init(size)) return false;

    m_songId = songId;
    m_anSong = song;
    m_parentPopup = parentPopup;

    CCMenuItemSpriteExtra* button;
    CCSprite* spr;
    CCMenu* menu;

    // Download button
    spr = CCSprite::createWithSpriteFrameName("GJ_downloadBtn_001.png");
    spr->setScale(0.7f);
    button = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(ANSongCell::onDownload)
    );
    button->setAnchorPoint(ccp(0.5f, 0.5f));
    button->setID("set-button");

    menu = CCMenu::create();
    menu->addChild(button);

    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(295.f, 18.f));
    menu->setID("button-menu1");
    this->addChild(menu);

    // Set button
    spr = ButtonSprite::create("Set");
    spr->setScale(0.6f);
    button = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(ANSongCell::onSetSong)
    );

    menu = CCMenu::create();
    menu->addChild(button);

    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(290.f, 42.f));
    menu->setID("button-menu2");
    this->addChild(menu);

    // Delete button
    spr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
    spr->setScale(0.65f);
    button = CCMenuItemSpriteExtra::create(
        spr,
        this,
        menu_selector(ANSongCell::onDeleteSong)
    );
    menu = CCMenu::create();
    menu->addChild(button);

    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(265.f, 18.f));
    menu->setID("button-menu2");
    this->addChild(menu);
    m_trashButton = button;

    setButtonsVisible();

    m_songInfoLayer = CCLayer::create();
    m_songNameLabel = CCLabelBMFont::create(m_anSong->name.c_str(), "bigFont.fnt");
    m_songNameLabel->limitLabelWidth(220.f, 0.7f, 0.1f);
    m_songNameLabel->setID("song-name");

    m_artistNameLabel = CCLabelBMFont::create(m_anSong->artist.c_str(), "goldFont.fnt");
    m_artistNameLabel->limitLabelWidth(120.f, 0.7f, 0.1f);
    m_artistNameLabel->setID("artist-name");

    m_sourceLabel = CCLabelBMFont::create(std::format("{} : {}", getSubstringAfterSlash(m_anSong->index), m_anSong->getSource()).c_str(), "goldFont.fnt");
    m_sourceLabel->limitLabelWidth(120.f, 0.7f, 0.1f);
    m_sourceLabel->setID("artist-name");

    m_songInfoLayer->addChild(m_sourceLabel);
    m_songInfoLayer->addChild(m_artistNameLabel);
    m_songInfoLayer->addChild(m_songNameLabel);
    m_songInfoLayer->setID("song-info");
    auto layout = ColumnLayout::create();
    layout->setAutoScale(false);
    layout->setAxisAlignment(AxisAlignment::Center);
    layout->setCrossAxisLineAlignment(AxisAlignment::Start);
    m_songInfoLayer->setContentSize(ccp(240.f, this->getContentSize().height - 6.f));
    m_songInfoLayer->setAnchorPoint(ccp(0.f, 0.f));
    m_songInfoLayer->setPosition(ccp(12.f, 1.5f));
    m_songInfoLayer->setLayout(layout);

    this->addChild(m_songInfoLayer);
    return true;
}

void ANSongCell::onSetSong(CCObject* target) {
  setSong();
}

void ANSongCell::setButtonsVisible() {
  m_trashButton->setVisible(fs::exists(getFileDownloadPath(false)));
}

void ANSongCell::onDeleteSong(CCObject* target) {
  const fs::path downloadPath = getFileDownloadPath(false);
  if (!fs::exists(downloadPath)) {
    Notification::create("File doesn't exists", NotificationIcon::Error)->show();
    return;
  }
  fs::remove(downloadPath);
  setButtonsVisible();
}

void ANSongCell::setSong() {
  setButtonsVisible();
  fs::path downloadPath = getFileDownloadPath(false);
  const Ref<CustomSongWidget> customSongWidget = m_parentPopup->m_parentWidget;

  if (!fs::exists(downloadPath)) {
    Notification::create("File doesn't exists", NotificationIcon::Error)->show();
    return;
  }

  SongInfo song = {
      .path = downloadPath,
      .songName = m_anSong->name,
      .authorName = m_anSong->artist,
      .songUrl = "local",
  };
  jukebox::addNong(song, m_songId);
  jukebox::setActiveSong(song, m_songId);
  customSongWidget->m_songInfoObject->m_artistName = m_anSong->artist;
  customSongWidget->m_songInfoObject->m_songName = m_anSong->name;
  customSongWidget->updateSongObject(customSongWidget->m_songInfoObject);
}

fs::path ANSongCell::getFileDownloadPath(bool create) {
  if (typeid(*m_anSong) == typeid(ANYTSong)) {
    const ANYTSong* ytSong = static_cast<ANYTSong*>(m_anSong);
    const std::string videoId = ytSong->yt_id;
    if (create) {
      fs::create_directory(std::format("{}\\youtube", Mod::get()->getSaveDir().string()));
    }
    return std::format("{}\\youtube\\{}.mp3", Mod::get()->getSaveDir().string(), videoId);
  }
  if (typeid(*m_anSong) == typeid(ANHostSong)) {
    const ANHostSong* hostSong = static_cast<ANHostSong*>(m_anSong);
    if (create) {
      fs::create_directory(std::format("{}\\host", Mod::get()->getSaveDir().string()));
    }
    return std::format("{}\\host\\{}.mp3", Mod::get()->getSaveDir().string(), urlToFilename(hostSong->url));
  }
  return "";
}

void ANSongCell::downloadFromYtDlp() {
  m_currentlyDownloading = true;
  const ANYTSong* ytSong = static_cast<ANYTSong*>(m_anSong);
  const fs::path ytDlpPath = Mod::get()->getSettingValue<std::string>("yt-dlp-path");
  const std::string videoId = ytSong->yt_id;
  const fs::path downloadPath = getFileDownloadPath(true);

  if (!fs::exists(ytDlpPath)) {
    log::info("yt-dlp.exe not found at specified path: {}", ytDlpPath);
    Notification::create("yt-dlp.exe not found at specified path", NotificationIcon::Error)->show();
    return;
  }

  std::string ytDlpCommand = fmt::format("\"{}\" -x --audio-format mp3 \"{}\" -o \"{}\"", ytDlpPath, videoId, downloadPath);

  auto notif = Notification::create(
      "Downloading using yt dlp", NotificationIcon::Loading, 0
  );
  notif->setTime(NOTIFICATION_DEFAULT_TIME);
  notif->show();

  std::thread commandThread([this, ytSong, ytDlpCommand]() { 
    std::string command = ytDlpCommand;

    int result = std::system(std::format("\"{}\"", command).c_str());

    log::info("command({}): {}", result, command);

    m_currentlyDownloading = false;

    if (result != 0) {
      log::info("Failed to execute command({}): {}", result, command);
      Loader::get()->queueInMainThread([](){
        Notification::create("Failed to download from YouTube", NotificationIcon::Error)->show();
      });
      return;
    }

    log::info("Successfully executed command: {}", command);

    Loader::get()->queueInMainThread([this](){
      Notification::create("Downloaded", NotificationIcon::Success)->show();
      setSong();
    });
  });

  commandThread.detach();
}

void ANSongCell::downloadFromCobalt() {
    m_currentlyDownloading = true;
    const std::string apiUrl = "https://co.wuk.sh/api/json";
    const ANYTSong* ytSong = static_cast<ANYTSong*>(m_anSong);
    const std::string videoId = ytSong->yt_id;

    std::unordered_map<std::string, std::string> parameters = {
        {"url", std::format("https://www.youtube.com/watch?v={}", videoId)},
        {"aFormat", "mp3"},
        {"isAudioOnly", "true"}
    };

    matjson::Value parameters_json = matjson::Object();
    for (auto const& [key, value] : parameters) {
      parameters_json[key] = value;
    }
    std::string body = parameters_json.dump(matjson::NO_INDENTATION);

    auto notif = Notification::create(
        "Downloading using Cobalt Tools", NotificationIcon::Loading, 0
    );
    notif->setTime(NOTIFICATION_DEFAULT_TIME);
    notif->show();

    geode::utils::web::AsyncWebRequest()
        .header("Accept", "application/json")
        .header("Content-Type", "application/json")
        .bodyRaw(body)
        .post(apiUrl)
        .text()
        .then([this](const std::string& r) {
            matjson::Value data = matjson::parse(r);

            if (data["status"] == "stream") {
              std::string audio_url = data["url"].as_string();
              
              geode::utils::web::AsyncWebRequest()
                  .get(audio_url)
                  .bytes()
                  .then([this](const geode::ByteVector& audio_data) {
                    std::ofstream file(getFileDownloadPath(true), std::ios::out | std::ios::binary);
                    file.write(reinterpret_cast<const char*>(audio_data.data()), audio_data.size());
                    file.close();

                    std::cout << "Downloaded the song successfully!" << std::endl;

                    Loader::get()->queueInMainThread([this](){
                      Notification::create("Downloaded", NotificationIcon::Success)->show();
                      setSong();
                    });
                    m_currentlyDownloading = false;
                  }).expect([this](auto _) {
                    m_currentlyDownloading = false;
                  });
            } else {
              std::cout << "Failed to download the song: " << r << std::endl;
              m_currentlyDownloading = false;
            }
        }).expect([this](auto _) {
          m_currentlyDownloading = false;
        });

}

void ANSongCell::onDownload(CCObject* target) {
  if (m_currentlyDownloading) {
    FLAlertLayer::create(
      "Currently downloading",
      "Please wait for the current download to finish before downloading the same song again.",
      "OK"
    )->show();
    return;
  }

  if (fs::exists(getFileDownloadPath(false))) {
    createQuickPopup("Song already downloaded", 
      "Are you sure you want to redownload the song?", 
      "No", 
      "Yes",
      [this, target](auto, bool btn2) {
        if (!btn2) {
          return;
        }
        fs::remove(getFileDownloadPath(false));
        onDownload(target);
      }
    );
    return;
  }

  if (typeid(*m_anSong) == typeid(ANYTSong)) {
    if (Mod::get()->getSettingValue<bool>("use-cobalt")) {
      downloadFromCobalt();
    } else if (Mod::get()->getSettingValue<bool>("use-yt-dlp")) {
      downloadFromYtDlp();
    } else {
      Notification::create("No download method for YouTube", NotificationIcon::Error)->show();
    }
  } else if (typeid(*m_anSong) == typeid(ANHostSong)) {
    const ANHostSong* hostSong = static_cast<ANHostSong*>(m_anSong);
    const fs::path downloadPath = getFileDownloadPath(true);
    m_currentlyDownloading = true;
    Notification::create("Downloading from URL", NotificationIcon::Loading)->show();
    geode::utils::web::AsyncWebRequest()
        .get(hostSong->url)
        .bytes()
        .then([this, downloadPath](const geode::ByteVector& audio_data) {
          std::ofstream file(downloadPath, std::ios::out | std::ios::binary);
          file.write(reinterpret_cast<const char*>(audio_data.data()), audio_data.size());
          file.close();

          std::cout << "Downloaded the song successfully!" << std::endl;

          Loader::get()->queueInMainThread([this](){
            Notification::create("Downloaded", NotificationIcon::Success)->show();
            setSong();
          });
          m_currentlyDownloading = false;
        }).expect([this](auto _) {
          m_currentlyDownloading = false;
        });
  }
}

ANSongCell* ANSongCell::create(int songId, ANSong* song, ANDropdownLayer* parentPopup, CCSize const& size) {
    auto ret = new ANSongCell();
    if (ret && ret->init(songId, song, parentPopup, size)) {
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
