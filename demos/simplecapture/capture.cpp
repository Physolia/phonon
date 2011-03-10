/*  This file is part of the KDE project
    Copyright (C) 2011 Casian Andrei <skeletk13@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "capture.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QMessageBox>

#include <phonon/AudioOutput>
#include <phonon/AvCapture>
#include <phonon/MediaObject>
#include <phonon/VideoWidget>
#include <phonon/VolumeSlider>

CaptureWidget::CaptureWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    // Create the objects used for capture and set up a default capture method
    m_media = new Phonon::MediaObject(this);
    m_avcapture = new Phonon::Experimental::AvCapture(this);
    m_captureNode = m_avcapture;

    // Create the audio and video outputs (sinks)
    m_audioOutput = new Phonon::AudioOutput(this);
    m_videoWidget = new Phonon::VideoWidget(this);

    /*
     * Set up the buttons and layouts and widgets
     */
    m_playButton = new QPushButton(this);
    m_playButton->setText(tr("Play"));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(playPause()));

    m_stopButton = new QPushButton(this);
    m_stopButton->setText(tr("Stop"));
    m_stopButton->setEnabled(false);
    connect(m_stopButton, SIGNAL(clicked()), this, SLOT(stop()));

    m_moButton = new QRadioButton(this);
    m_moButton->setText(tr("Use MediaObject"));
    m_moButton->setAutoExclusive(true);
    connect(m_moButton, SIGNAL(toggled(bool)), this, SLOT(enableMOCapture(bool)));

    m_avcapButton = new QRadioButton(this);
    m_avcapButton->setText(tr("Use Audio Video Capture (AvCapture)"));
    m_avcapButton->setAutoExclusive(true);
    m_avcapButton->setChecked(true);
    connect(m_avcapButton, SIGNAL(toggled(bool)), this, SLOT(enableAvCapture(bool)));

    setLayout(new QVBoxLayout);

    // Configure the video widget a bit
    m_videoWidget->setMinimumSize(QSize(400, 300));
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout()->addWidget(m_videoWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_moButton);
    buttonsLayout->addWidget(m_avcapButton);
    layout()->addItem(buttonsLayout);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_playButton);
    buttonsLayout->addWidget(m_stopButton);
    layout()->addItem(buttonsLayout);

    // Set up capture and start it
    setupCaptureSource();
    playPause();
}

void CaptureWidget::enableMOCapture(bool enable)
{
    if (!enable)
        return;

    stop();
    m_captureNode = m_media;
    setupCaptureSource();
}

void CaptureWidget::enableAvCapture(bool enable)
{
    if (!enable)
        return;

    stop();
    m_captureNode = m_avcapture;
    setupCaptureSource();
}

void CaptureWidget::setupCaptureSource()
{
    // Disconnect the old paths, if they exist
    if (m_audioPath.isValid()) {
        m_audioPath.disconnect();
    }
    if (m_videoPath.isValid()) {
        m_videoPath.disconnect();
    }

    // Reconnect the paths using the object used for capture
    if (m_captureNode == m_media) {
        m_audioPath = Phonon::createPath(m_media, m_audioOutput);
        m_videoPath = Phonon::createPath(m_media, m_videoWidget);
    }
    if (m_captureNode == m_avcapture) {
        m_audioPath = Phonon::createPath(m_avcapture, m_audioOutput);
        m_videoPath = Phonon::createPath(m_avcapture, m_videoWidget);
    }

    if (!m_audioPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support audio capturing.");
    }
    if (!m_videoPath.isValid()) {
        QMessageBox::critical(this, "Error", "Your backend may not support video capturing.");
    }

    /*
     * Set up the devices used for capture
     * Phonon can easily get you the devices appropriate for a specific category.
     */
    if (m_captureNode == m_media) {
        Phonon::MediaSource source(Phonon::CaptureDevice::VideoType, Phonon::NoCaptureCategory);
        m_media->setCurrentSource(source);
    }
    if (m_captureNode == m_avcapture) {
        m_avcapture->setCaptureDevices(Phonon::NoCaptureCategory);
    }

    // Connect the stateChanged signal from the object used for capture to our handling slot
    if (m_captureNode == m_media) {
        disconnect(m_avcapture, SIGNAL(stateChanged(Phonon::State,Phonon::State)));
        connect(m_media, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State)));
    }
    if (m_captureNode == m_avcapture) {
        disconnect(m_media, SIGNAL(stateChanged(Phonon::State,Phonon::State)));
        connect(m_avcapture, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State)));
    }
}

void CaptureWidget::playPause()
{
    if (m_captureNode == m_media) {
        if (m_media->state() == Phonon::PlayingState) {
            m_media->pause();
        } else {
            m_media->play();
        }
    }

    if (m_captureNode == m_avcapture) {
        if (m_avcapture->state() == Phonon::PlayingState) {
            m_avcapture->pause();
        } else {
            m_avcapture->start();
        }
    }
}

void CaptureWidget::stop()
{
    if (m_captureNode == m_media) {
        m_media->stop();
    }

    if (m_captureNode == m_avcapture) {
        m_avcapture->stop();
    }
}

void CaptureWidget::mediaStateChanged(Phonon::State newState)
{
    switch(newState) {
    case Phonon::LoadingState:
        break;
    case Phonon::StoppedState:
        m_playButton->setText(tr("Play"));
        m_stopButton->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playButton->setText(tr("Pause"));
        m_stopButton->setEnabled(true);
        break;
    case Phonon::BufferingState:
        break;
    case Phonon::PausedState:
        m_playButton->setText(tr("Play"));
        break;
    case Phonon::ErrorState:
        break;
    }
}

#include "capture.moc"
