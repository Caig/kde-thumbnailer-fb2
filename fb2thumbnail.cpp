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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <kzip.h>
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

fb2Creator::fb2Creator()
{
}

fb2Creator::~fb2Creator()
{
}

bool fb2Creator::create(const QString& path, int width, int height, QImage& img)
{
    QFile file(path);
    
    KZip zip(path);
    QIODevice *device;
    const KArchiveDirectory *dir;
    const KZipFileEntry *fb2File;
    
    QXmlStreamReader qxml;
    
    QString fileExt = QFileInfo(path).suffix().toLower();
    if (fileExt == "fb2")
    {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "[fb2 thumbnailer]" << "Couldn't open" << path;
            return false;
        }
        else
        {
            qDebug() << "[fb2 thumbnailer]" << "Reading" << path;
            qxml.setDevice(&file);
        }
    }
    else //if *.fb2.zip
    {
        if (!zip.open(QIODevice::ReadOnly))
        {
            qDebug() << "[fb2 thumbnailer]" << "Couldn't open" << path;
            return false;
        }
        else
        {
            qDebug() << "[fb2 thumbnailer]" << "Reading" << path;

            dir = zip.directory();

            QStringList fileList = dir->entries();

            for (int i=0; i < fileList.count(); i++)
            {
                if (fileList.at(i).endsWith(".fb2"))
                {
                    fb2File = static_cast<const KZipFileEntry*>(dir->entry(fileList.at(i)));
                    device = fb2File->createDevice();
                    qxml.setDevice(device);

                    break;
                }
            }
        }
    }
    
    //----

    bool inCoverpage = false;
    QString coverId = "";
    QByteArray coverBase64;
    
    while(!qxml.atEnd() && !qxml.hasError())
    {
        qxml.readNext();

        if (qxml.name() == "coverpage")
        {
            if (qxml.isStartElement())
                inCoverpage = true;
            else
                inCoverpage = false;
        }

        if (qxml.name() == "image" && qxml.isStartElement() && inCoverpage == true)
        {
            //various namespaces: xlink, l, NS2
            QXmlStreamAttributes qxmlAttributes = qxml.attributes();

            for (int pos = 0; pos < qxmlAttributes.size(); pos++)
            {
                if (qxmlAttributes.at(pos).name() == "href")
                {
                    coverId = qxmlAttributes.at(pos).value().toString();
                    break;
                }
            }

            if (coverId != "")
            {
                coverId.remove("#");
                qDebug() << "[fb2 thumbnailer]" << "Found cover id:" << coverId;
            }
        }

        if (qxml.name() == "binary" && qxml.isStartElement())
        {
            if (coverId != "")
            {
                if (qxml.attributes().value("id") == coverId)
                {
                    qDebug() << "[fb2 thumbnailer]" << "Found cover data";

                    coverBase64 = qxml.readElementText().toLatin1();

                    QImage coverImage;
                    coverImage.loadFromData(QByteArray::fromBase64(coverBase64));
                    
                    img = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                    break;
                }
            }
            else //if coverId not found then the file doesn't follow the specification, try a workaround
            {
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

    if (coverBase64.isEmpty())
        qDebug() << "[fb2 thumbnailer]" << "Cover data not found";

    if (qxml.hasError())
        qDebug() << "[fb2 thumbnailer]" << "Parsing error:" << qxml.errorString();

    qxml.clear();

    if (fileExt == "fb2")
        file.close();
    else
    {
        device->close();
        delete device;
        //delete fb2File;
        //delete dir;
    }

    return !img.isNull();
}

ThumbCreator::Flags fb2Creator::flags() const
{
    return None;
}
