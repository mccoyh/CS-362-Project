#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

class MediaPlayer {
public:
  MediaPlayer();
  ~MediaPlayer();

  void run();

private:
  void loadCaptions();

  void update();

  void handleKeyInput();

  void displayGui();

  void menuBarGui();

  void timelineGui();

  void volumeGui();

  void navigateFrames();
};

#endif //MEDIAPLAYER_H
