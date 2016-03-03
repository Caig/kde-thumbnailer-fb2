/*
This file is part of kde-thumbnailer-fb2
Copyright (C) 2013-2016 Giacomo Barazzetti <giacomosrv@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "fb2thumbnail.h"
#include "fb2.h"

#include <QtCore/QFileInfo>
#include <QtCore/QXmlStreamReader>
#include <QtGui/QImage>
#include <QtCore/QDebug>

extern "C"
{
    Q_DECL_EXPORT ThumbCreator *new_creator()
    {
        return new Fb2Creator();
    }
}

fb2::OpenStrategy* openFile(const QString &path);
bool loadCover(QIODevice *device, QImage &coverImage, int width, int height);

//--------------------

bool Fb2Creator::create(const QString &path, int width, int height, QImage &img)
{       
    fb2::OpenStrategy *openStrategy = openFile(path);
    if (openStrategy == NULL) {
        return false;
    }
    
    // I shall not delete this device!
    QIODevice *device = openStrategy->createDevice();
    if (device == NULL) {
        return false;
    }

    bool foundCover = loadCover(device, img, width, height);
    if (!foundCover) {
        qDebug() << "[fb2 thumbnailer]" << "Cover not found";
    }

    delete openStrategy;
    openStrategy = NULL;

    return foundCover;
}

ThumbCreator::Flags Fb2Creator::flags() const
{
    return ThumbCreator::None;
}

//---------

fb2::OpenStrategy* openFile(const QString &path)
{
    const QString fileExt = QFileInfo(path).suffix().toLower();
    if (fileExt == fb2::FB2_EXT) {
        return new fb2::OpenFb2(path);
    } else if (fileExt == fb2::FB2ZIP_EXT) {
        return new fb2::OpenZippedFb2(path);
    }

    return NULL;
}

//---------

bool loadCover(QIODevice *device, QImage &coverImage, int width, int height)
{
    QXmlStreamReader qxml(device);

    bool inCoverpage = false;
    QString coverId;
    
    while(!qxml.atEnd() && !qxml.hasError()) {
        qxml.readNext();

        if (qxml.name() == "coverpage") {
            inCoverpage = qxml.isStartElement();
        }

        if (qxml.name() == "image" && qxml.isStartElement() && inCoverpage == true) {
            //various namespaces: xlink, l, NS2
            QXmlStreamAttributes qxmlAttributes = qxml.attributes();

            for (int pos = 0; pos < qxmlAttributes.size(); pos++) {
                if (qxmlAttributes.at(pos).name() == "href") {
                    coverId = qxmlAttributes.at(pos).value().toString();
                    break;
                }
            }

            if (!coverId.isEmpty()) {
                coverId.remove("#");
                qDebug() << "[fb2 thumbnailer]" << "Found cover id:" << coverId;
            }
        }

        if (qxml.name() == "binary" && qxml.isStartElement()) {
            if (!coverId.isEmpty()) {
                if (qxml.attributes().value("id") == coverId) {
                    qDebug() << "[fb2 thumbnailer]" << "Found cover data";

                    coverImage.loadFromData(QByteArray::fromBase64(qxml.readElementText().toLatin1()));
                    coverImage = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                    break;
                }
            } else { //if coverId not found then the file doesn't follow the specification, try a workaround
                qDebug() << "[fb2 thumbnailer]" << "Cover id not found => using first image";

                coverImage.loadFromData(QByteArray::fromBase64(qxml.readElementText().toLatin1()));
                coverImage = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                break;
            }
        }
    }

    if (qxml.hasError()) {
        qDebug() << "[fb2 thumbnailer]" << "Parsing error:" << qxml.errorString();
    }

    qxml.clear();
    
    return !coverImage.isNull();
}
