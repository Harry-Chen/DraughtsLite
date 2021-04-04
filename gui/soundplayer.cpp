#include "soundplayer.h"

SoundPlayer::SoundPlayer():
    player(new QMediaPlayer())
{

}

void SoundPlayer::play(SoundPlayer::Sounds sound)
{
    if(player->state() == QMediaPlayer::PlayingState){
        player->stop();
    }
    player->setMedia(soundMap.value(sound));
    player->play();
}
