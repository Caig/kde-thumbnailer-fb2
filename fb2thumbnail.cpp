/*
This file is part of kde-thumbnailer-fb2
Copyright (C) 2013 Caig <giacomosrv@gmail.com>

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

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QImage>
#include <QDebug>

extern "C"
{
    KDE_EXPORT ThumbCreator *new_creator()
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
    //check if it's a fb2.zip file, currently not supported
    QString fileExt = QFileInfo(path).suffix().toLower();
    if (fileExt != "fb2")
    {
        qDebug() << "[fb2 thumbnailer]" << "Couldn't parse" << path;
        qDebug() << "[fb2 thumbnailer]" << "Currently no support for" << fileExt << "files";
        return false;
    }
        
    QFile file(path);
    
    QXmlStreamReader qxml(&file);

    bool inCoverpage = false;
    
    QString coverId = "";

    QByteArray coverBase64;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << "[fb2 thumbnailer]" << "Couldn't open" << path;
    else
        qDebug() << "[fb2 thumbnailer]" << "Reading" << path;
    
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

        if (qxml.name() == "binary" && qxml.isStartElement() && coverId != "")
        {
            if (qxml.attributes().value("id") == coverId)
            {
                qDebug() << "[fb2 thumbnailer]" << "Found cover data";

                coverBase64 = qxml.readElementText().toAscii();

                QImage coverImage;
                coverImage.loadFromData(QByteArray::fromBase64(coverBase64));
                
                img = coverImage.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                break;
            }
        }
    }

    if (coverId == "")
        qDebug() << "[fb2 thumbnailer]" << "Cover id not found";
    if (coverBase64.isEmpty())
        qDebug() << "[fb2 thumbnailer]" << "Cover data not found";

    if (qxml.hasError())
        qDebug() << "[fb2 thumbnailer]" << "Parsing error";

    file.close();

    return !img.isNull();
}

ThumbCreator::Flags fb2Creator::flags() const
{
    return None;
}