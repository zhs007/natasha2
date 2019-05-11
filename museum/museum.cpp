#include "museum.h"
#include "game_museum.h"

namespace natasha {

#ifdef NATASHA_RUNINCPP
void Museum::initConfig() {
  m_cfg.Clear();

  auto maprtp = m_cfg.mutable_rtp();

  ::natashapb::MuseumRTPConfig rtp96;

  rtp96.set_fgnums(12);

  rtp96.add_bgbonusprize(1);
  rtp96.add_bgbonusprize(1);
  rtp96.add_bgbonusprize(1);
  rtp96.add_bgbonusprize(1);
  rtp96.add_bgbonusprize(1);
  rtp96.add_bgbonusprize(1);

  rtp96.add_bgmultipliers(1);
  rtp96.add_bgmultipliers(1);
  rtp96.add_bgmultipliers(1);
  rtp96.add_bgmultipliers(1);
  rtp96.add_bgmultipliers(1);
  rtp96.add_bgmultipliers(1);

  // rtp96.add_bgbonusprize(5);
  // rtp96.add_bgbonusprize(8);
  // rtp96.add_bgbonusprize(10);
  // rtp96.add_bgbonusprize(15);
  // rtp96.add_bgbonusprize(30);
  // rtp96.add_bgbonusprize(50);

  // rtp96.add_bgmultipliers(1);
  // rtp96.add_bgmultipliers(2);
  // rtp96.add_bgmultipliers(3);
  // rtp96.add_bgmultipliers(5);
  // rtp96.add_bgmultipliers(10);
  // rtp96.add_bgmultipliers(15);

  {
    auto mw0 = rtp96.add_bgmysterywild();
    mw0->add_weights(0);
    mw0->add_weights(185);
    mw0->set_totalweight(sumWeightConfig(*mw0));

    auto mw1 = rtp96.add_bgmysterywild();
    mw1->add_weights(0);
    mw1->add_weights(15);
    mw1->set_totalweight(sumWeightConfig(*mw1));

    auto mw2 = rtp96.add_bgmysterywild();
    mw2->add_weights(0);
    mw2->add_weights(12);
    mw2->set_totalweight(sumWeightConfig(*mw2));

    auto mw3 = rtp96.add_bgmysterywild();
    mw3->add_weights(0);
    mw3->add_weights(75);
    mw3->set_totalweight(sumWeightConfig(*mw3));

    auto mw4 = rtp96.add_bgmysterywild();
    mw4->add_weights(0);
    mw4->add_weights(100);
    mw4->set_totalweight(sumWeightConfig(*mw4));

    auto mw5 = rtp96.add_bgmysterywild();
    mw5->add_weights(0);
    mw5->add_weights(200);
    mw5->set_totalweight(sumWeightConfig(*mw5));
    // auto mw0 = rtp96.add_bgmysterywild();
    // mw0->add_weights(1);
    // mw0->add_weights(185);
    // mw0->set_totalweight(sumWeightConfig(*mw0));

    // auto mw1 = rtp96.add_bgmysterywild();
    // mw1->add_weights(1);
    // mw1->add_weights(15);
    // mw1->set_totalweight(sumWeightConfig(*mw1));

    // auto mw2 = rtp96.add_bgmysterywild();
    // mw2->add_weights(1);
    // mw2->add_weights(12);
    // mw2->set_totalweight(sumWeightConfig(*mw2));

    // auto mw3 = rtp96.add_bgmysterywild();
    // mw3->add_weights(1);
    // mw3->add_weights(75);
    // mw3->set_totalweight(sumWeightConfig(*mw3));

    // auto mw4 = rtp96.add_bgmysterywild();
    // mw4->add_weights(1);
    // mw4->add_weights(100);
    // mw4->set_totalweight(sumWeightConfig(*mw4));

    // auto mw5 = rtp96.add_bgmysterywild();
    // mw5->add_weights(1);
    // mw5->add_weights(200);
    // mw5->set_totalweight(sumWeightConfig(*mw5));
  }

  rtp96.add_fgbonusprize(10);
  rtp96.add_fgbonusprize(15);
  rtp96.add_fgbonusprize(20);
  rtp96.add_fgbonusprize(25);
  rtp96.add_fgbonusprize(50);
  rtp96.add_fgbonusprize(100);

  rtp96.add_fgmultipliers(2);
  rtp96.add_fgmultipliers(4);
  rtp96.add_fgmultipliers(6);
  rtp96.add_fgmultipliers(10);
  rtp96.add_fgmultipliers(20);
  rtp96.add_fgmultipliers(30);

  {
    auto mw0 = rtp96.add_fgmysterywild();
    mw0->add_weights(1);
    mw0->add_weights(90);
    mw0->set_totalweight(sumWeightConfig(*mw0));

    auto mw1 = rtp96.add_fgmysterywild();
    mw1->add_weights(1);
    mw1->add_weights(15);
    mw1->set_totalweight(sumWeightConfig(*mw1));

    auto mw2 = rtp96.add_fgmysterywild();
    mw2->add_weights(1);
    mw2->add_weights(13);
    mw2->set_totalweight(sumWeightConfig(*mw2));

    auto mw3 = rtp96.add_fgmysterywild();
    mw3->add_weights(1);
    mw3->add_weights(75);
    mw3->set_totalweight(sumWeightConfig(*mw3));

    auto mw4 = rtp96.add_fgmysterywild();
    mw4->add_weights(1);
    mw4->add_weights(100);
    mw4->set_totalweight(sumWeightConfig(*mw4));

    auto mw5 = rtp96.add_fgmysterywild();
    mw5->add_weights(1);
    mw5->add_weights(200);
    mw5->set_totalweight(sumWeightConfig(*mw5));
  }

  ::google::protobuf::MapPair< ::std::string, ::natashapb::MuseumRTPConfig> p(
      "rtp96", rtp96);

  maprtp->insert(p);
}
#endif  // NATASHA_RUNINCPP

::natashapb::CODE Museum::init(const char* cfgpath) {
#ifdef NATASHA_RUNINCPP
  initConfig();
#endif  // NATASHA_RUNINCPP

  FileNameList lst;

  loadNormalReels3X5(pathAppend(cfgpath, "game462_payout97.csv").c_str(),
                     m_reels);
  if (m_reels.isEmpty()) {
    return ::natashapb::INVALID_REELS_CFG;
  }

#ifdef NATASHA_COUNTRTP
  m_reels.output("reels", MUSEUM_SYMBOL_S);
#endif  // NATASHA_COUNTRTP

  loadPaytables3X5(pathAppend(cfgpath, "game462_paytables.csv").c_str(),
                   m_paytables);
  if (m_paytables.isEmpty()) {
    return ::natashapb::INVALID_PAYTABLES_CFG;
  }

  m_lstBet.push_back(1);

  addGameMod(::natashapb::BASE_GAME,
             new MuseumBaseGame(*this, m_reels, m_paytables, m_lstBet, m_cfg));
  addGameMod(::natashapb::FREE_GAME,
             new MuseumFreeGame(*this, m_reels, m_paytables, m_lstBet, m_cfg));

  return GameLogic::init(cfgpath);
}

// getMainGameMod - get current main game module
GameMod* Museum::getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                bool isComeInGame) {
  auto pBG = getGameMod(::natashapb::BASE_GAME);
  assert(pBG != NULL);

  auto pFG = getGameMod(::natashapb::FREE_GAME);
  assert(pFG != NULL);

  auto pUserBG = getConstUserGameModInfo(pLogicUser, ::natashapb::BASE_GAME);
  assert(pUserBG != NULL);

  auto pUserFG = getConstUserGameModInfo(pLogicUser, ::natashapb::FREE_GAME);
  assert(pUserFG != NULL);

  if (pFG->isIn(pUserFG)) {
    // printf("TLOD FG\n");

    return pFG;
  }

  return pBG;
}

#ifdef NATASHA_COUNTRTP

// countRTP_museum - count rtp
void countRTP_museum() {
  Museum museum;

  printf("%ld\n", time(NULL));

  auto c = museum.init("./csv");
  if (c != natashapb::OK) {
    printf("init fail(%d)!\n", c);
  }

  auto pUGI = new ::natashapb::UserGameLogicInfo();

  pUGI->set_configname("rtp96");

  c = museum.userComeIn(pUGI);
  if (c != natashapb::OK) {
    printf("userComeIn fail(%d)!\n", c);
  }

  int64_t totalbet = 0;
  int64_t totalpay = 0;

  auto pGameCtrl = new ::natashapb::GameCtrl();
  int64_t ctrlid = 1;

  for (int i = 0; i <= 100; ++ctrlid) {
    // continue ;

    if (pUGI->nextgamemodtype() == natashapb::BASE_GAME) {
      auto spin = pGameCtrl->mutable_spin();
      spin->set_bet(1);
      spin->set_lines(natasha::MUSEUM_DEFAULT_PAY_LINES);
      spin->set_times(natasha::MUSEUM_DEFAULT_TIMES);
    } else if (pUGI->nextgamemodtype() == natashapb::FREE_GAME) {
      auto freespin = pGameCtrl->mutable_freespin();
      freespin->set_bet(1);
      freespin->set_lines(natasha::MUSEUM_DEFAULT_PAY_LINES);
      freespin->set_times(natasha::MUSEUM_DEFAULT_TIMES);
    }

    pGameCtrl->set_ctrlid(ctrlid);

    c = museum.gameCtrl(pGameCtrl, pUGI);
    if (c != natashapb::OK) {
      printf("gameCtrl fail(%d)!\n", c);
    }

    if (pUGI->iscompleted()) {
      printf("totalbet is %lld\n", museum.getRTP().rtp.totalbet());

      ++i;
    }
  }

  museum.outputRTP();

  delete pGameCtrl;

  printf("%ld\n", time(NULL));

  printf("end!\n");
}

#endif  // NATASHA_COUNTRTP

}  // namespace natasha