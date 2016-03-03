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
        return new fb2Creator();
    }
}

fb2::OpenStrategy* openFile(const QString &path);

//--------------------

bool fb2Creator::create(const QString &path, int width, int height, QImage &img)
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
    
    QXmlStreamReader qxml(device);

    bool inCoverpage = false;
    QString coverId = "";
    QByteArray coverBase64;
    
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

            if (coverId != "") {
                coverId.remove("#");
                qDebug() << "[fb2 thumbnailer]" << "Found cover id:" << coverId;
            }
        }

        if (qxml.name() == "binary" && qxml.isStartElement()) {
            if (coverId != "") {
                if (qxml.attributes().value("id") == coverId) {
                    qDebug() << "[fb2 thumbnailer]" << "Found cover data";

                    coverBase64 = qxml.readElementText().toLatin1();

                    QImage coverImage;
                    coverImage.loadFromData(QByteArray::fromBase64(coverBase64));
                    
                    img = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                    break;
                }
            } else { //if coverId not found then the file doesn't follow the specification, try a workaround
                qDebug() << "[fb2 thumbnailer]" << "Cover id not found";
                qDebug() << "[fb2 thumbnailer]" << "Using first image as cover";

                coverBase64 = qxml.readElementText().toLatin1();

                QImage coverImage;
                coverImage.loadFromData(QByteArray::fromBase64(coverBase64));

                img = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                break;
            }
        }
    }

    if (coverBase64.isEmpty()) {
        qDebug() << "[fb2 thumbnailer]" << "Cover data not found";
    }

    if (qxml.hasError()) {
        qDebug() << "[fb2 thumbnailer]" << "Parsing error:" << qxml.errorString();
    }

    qxml.clear();
    
    delete openStrategy;
    openStrategy = NULL;

    return !img.isNull();
}

ThumbCreator::Flags fb2Creator::flags() const
{
    return None;
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
