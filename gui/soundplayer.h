#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QMediaPlayer>
#include <QString>
#include <QMap>

class SoundPlayer
{
public:
    SoundPlayer();
    enum Sounds {
        MOVE_WHITE,
        MOVE_BLACK,
        BREAK_CELL,
        CONNECT,
        WIN_GAME,
        LOSE_GAME,
        DRAW_GAME,
        NEW_GAME
    };

    void play(Sounds sound);

private:
    QMediaPlayer *player;
    QMap<Sounds, QMediaContent> soundMap = {
        {MOVE_WHITE, QUrl("qrc:/resources/sound/move_white.mp3")},
        {MOVE_BLACK, QUrl("qrc:/resources/sound/move_black.mp3")},
        {BREAK_CELL, QUrl("qrc:/resources/sound/break.mp3")},
        {CONNECT, QUrl("qrc:/resources/sound/connect.wav")},
        {LOSE_GAME, QUrl("qrc:/resources/sound/lost.mp3")},
        {WIN_GAME, QUrl("qrc:/resources/sound/win.mp3")},
        {DRAW_GAME, QUrl("qrc:/resources/sound/draw.mp3")},
        {NEW_GAME, QUrl("qrc:/resources/sound/new.mp3")}
    };
};

#endif // SOUNDPLAYER_H
