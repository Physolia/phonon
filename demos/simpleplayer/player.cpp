#include "player.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFileDialog>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/VideoWidget>

Player::Player(QWidget* parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    m_media = new Phonon::MediaObject(this);

    Phonon::AudioOutput* audioOut = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::VideoWidget* videoOut = new Phonon::VideoWidget(this);

    Phonon::createPath(m_media, audioOut);
    Phonon::createPath(m_media, videoOut);

    QWidget* buttonBar = new QWidget(this);

    m_playPause = new QPushButton(tr("Play"), buttonBar);
    m_stop = new QPushButton(tr("Stop"), buttonBar);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(videoOut);
    layout->addWidget(buttonBar);
    setLayout(layout);

    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->addWidget(m_stop);
    buttonLayout->addWidget(m_playPause);
    buttonBar->setLayout(buttonLayout);

    m_stop->setEnabled(false);

    connect(m_stop, SIGNAL(clicked(bool)), this, SLOT(play()));
    connect(m_playPause, SIGNAL(clicked(bool)), this, SLOT(playPause()));
}

void Player::playPause()
{
    if (m_media->state() == Phonon::PausedState) {
        play();
    } else {
        pause();
    }
}

void Player::load(const QUrl &url)
{
    m_media->setCurrentSource(url);
}

void Player::play()
{
    if (m_media->currentSource().type() == Phonon::MediaSource::Empty || m_media->currentSource().type() == Phonon::MediaSource::Invalid) {
        QString url = QFileDialog::getOpenFileName(this);
        if (url.isEmpty())
            return
        m_media->setCurrentSource(QUrl(url));
    }
    m_media->play();
}

void Player::pause()
{
    m_media->pause();
}

void Player::mediaStateChange(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch(newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_playPause->setText(tr("Play"));
        m_stop->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playPause->setText(tr("Play"));
        m_stop->setEnabled(true);
        break;
    case Phonon::BufferingState:
        break;
    case Phonon::PausedState:
        m_playPause->setText(tr("Pause"));
        break;
    case Phonon::ErrorState:
        break;
    }
}

#include "player.moc"

