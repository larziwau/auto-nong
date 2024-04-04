#pragma once 

#include <Geode/utils/web.hpp>
#include <Geode/binding/SongInfoObject.hpp>
#include <Geode/loader/Event.hpp>
#include "../types/an_song.hpp"
#include "../ui/an_multi_string_setting.hpp"
#include "../types/serializable_vector.hpp"
#include "../ui/an_clear_popup_blacklist.hpp"
#include "../utils/utils.hpp"

using namespace geode::prelude;

class ANYTSong;
class ANHostSong;

class AutoNongManager : public CCNode {
protected:
  inline static AutoNongManager* m_instance = nullptr;
  int m_currentLevelID = 0;
  std::map<int, std::vector<std::shared_ptr<ANSong>>> m_songIDToNongs;
public:
  std::vector<std::shared_ptr<ANSong>> getNongsFromSongID(int songID);
  bool anySongExists(std::set<int> songIDs);
  void loadIndexes();
  int getCurrentLevelID();
  void setCurrentLevelID(int levelID);
  void loadIndexesSchedule(float);

  static AutoNongManager* get() {
      if (m_instance == nullptr) {
          m_instance = new AutoNongManager;
          m_instance->retain();
      }

      return m_instance;
  }
};