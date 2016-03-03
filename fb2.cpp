/*
This file is part of kde-thumbnailer-fb2
Copyright (C) 2016 Giacomo Barazzetti <giacomosrv@gmail.com>

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

#include "fb2.h"

#include <QtCore/QDebug>

namespace fb2 {
    
OpenFb2::OpenFb2(const QString &path) {
    file = new QFile(path);
}

OpenFb2::~OpenFb2() {
    file->close();
    delete file;
    file = NULL;
}

QIODevice* OpenFb2::createDevice() {
    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[fb2 thumbnailer]" << "Reading" << file->fileName();
        return file;
    }
    
    qDebug() << "[fb2 thumbnailer]" << "Couldn't open" << file->fileName();
    return NULL;
}

OpenZippedFb2::OpenZippedFb2(const QString &path) : zip(path), device(NULL) { }

OpenZippedFb2::~OpenZippedFb2() {
    device->close();
    delete device;
    device = NULL;
}

QIODevice* OpenZippedFb2::createDevice() {
    if (zip.open(QIODevice::ReadOnly)) {
        qDebug() << "[fb2 thumbnailer]" << "Reading" << zip.fileName();
        
        const KArchiveDirectory *dir = zip.directory();
        QStringList fileList = dir->entries();
        
        for (int i=0; i < fileList.count(); i++) {
            if (fileList.at(i).endsWith("." + FB2_EXT)) {
                device = (static_cast<const KZipFileEntry*>(dir->entry(fileList.at(i))))->createDevice();
                return device;
            }
        }
    }
    
    qDebug() << "[fb2 thumbnailer]" << "Couldn't open" << zip.fileName();
    return NULL;
}

}
